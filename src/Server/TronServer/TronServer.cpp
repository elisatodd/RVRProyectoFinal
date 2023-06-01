#include "TronServer.h"

#include <time.h>
#include <signal.h>
#include <iostream>
#include <cassert>
#include <chrono>
#include <thread>
#include "../../Client/GameUtils/Tablero.h"
#include "../../Client/GameUtils/Coor.h"

#include <SDL2/SDL.h>

// Ctrl+C handler
bool m_exit = false;
void intHandler(int){
    m_exit = true;
}

TronServer::TronServer(const char* s, const char* p) : m_server_socket(s, p){
    // random seed
    srand((unsigned)time(NULL));

    // bind socket
    m_server_socket.bind();

    // ctrl+c handler init
    struct sigaction act;
    act.sa_handler = intHandler;
    sigaction(SIGINT, &act, NULL);

    // default values
    m_tron_1 = m_tron_2 = nullptr;
    m_pos_p1 = m_pos_p2 = Vector2D(0, 0);
    m_dir_p1 = m_dir_p2 = Vector2D(0, 0);

    m_state = MessageServer::ServerState::WAITING;
    m_tab = new Tablero("assets/levels/level1.txt");

    reset();
};

void TronServer::shutdown(){
    if(m_tab != nullptr)
        delete m_tab;
}

void TronServer::server_message_thread()
{
    while (true)
    { 
        MessageClient client_recv_msg;
        Socket *client_player_sock = new Socket(m_server_socket);
        m_server_socket.recv(client_recv_msg, client_player_sock);

        switch (client_recv_msg.m_type)
        {
        case MessageClient::ClientMessageType::REGISTER:
        { // add player to the game
            int pl;
            if (addPlayer(client_player_sock, pl))
                initPlayer(pl, &client_recv_msg);

            if (m_tron_1 != nullptr && m_tron_2 != nullptr)
            // Both players are initialized --> game can start
                m_state = MessageServer::ServerState::READY;
            else
            // One player is missing --> wait for them
                m_state = MessageServer::ServerState::WAITING;

            sendStateMessage();
        }
        break;
        case MessageClient::ClientMessageType::QUIT:
        { // remove player from game
            removePlayer(client_player_sock);

            if (m_tron_1 == nullptr || m_tron_2 == nullptr)
            {
                // One or both players missing --> back to waiting
                m_state = MessageServer::ServerState::WAITING;
                sendStateMessage();
            }
        }
        break;
        case MessageClient::ClientMessageType::HANDLE_INPUT:
        { // handles input sent by client for next step
            m_input_mutex.lock(); // mutex lock so values aren't modified during handleInput()
            saveInput(client_player_sock, client_recv_msg.m_input);
            m_input_mutex.unlock();
        }
        break;
        }
    }
}

void TronServer::run()
{
    while (!m_exit)
    {
        Uint32 startTime = SDL_GetTicks();

        m_input_mutex.lock(); // mutex lock to protect from alterations
        handleInput();
        m_input_mutex.unlock();

        if (m_state == MessageServer::ServerState::READY && m_p1_ready && m_p2_ready)
        {
            // Both players are ready and server is also ready --> game can start
            m_state = MessageServer::ServerState::PLAYING;
            m_p1_ready = m_p2_ready = false;
            std::cout << "[Server]: BothPlayersReady\n";
            sendStateMessage();
        }

        if (m_state == MessageServer::ServerState::PLAYING)
        {
            // Game is active --> game loop
            stepSimulation();
            updateInfoClients();

            Uint32 frameTime = SDL_GetTicks() - startTime;
            if (frameTime < 70)
                std::this_thread::sleep_for(std::chrono::milliseconds(70 - frameTime));
        }

        if (m_state == MessageServer::ServerState::GAME_OVER){
            // Game is finished --> go back to menu/show end screen
            if(SDL_GetTicks() - m_timer > GAME_OVER_TIME) {
                m_timer = 0;
                m_state = MessageServer::ServerState::READY;
                sendStateMessage();
            }
        }

    }

    m_state = MessageServer::ServerState::SERVER_QUIT;
    sendStateMessage();    
    std::cout << "Quit Server\n";
}

void TronServer::reset()
{
    m_input_t1 = m_input_t2 = MessageClient::InputType::NONE;
    // m_pos_p1 = Vector2D(200, 360); // default positions
    // m_pos_p2 = Vector2D(800, 360);

    m_p1_hit = false;
    m_p2_hit = false;

    m_p1_ready = false;
    m_p2_ready = false;

}

void TronServer::onRoundFinished()
{
    m_p1_hit = false;
    m_p2_hit = false;
}

void TronServer::saveInput(Socket *player_sock, MessageClient::InputType input)
{
    if (*player_sock == *m_tron_1)
        m_input_t1 = input;
    else if (*player_sock == *m_tron_2)
        m_input_t2 = input;
    else
        printf("Received input from a unregistered client.\n");
}

void TronServer::handleInput()
{
    // input player 1
    switch (m_input_t1)
    {
    case MessageClient::InputType::LEFT:
        m_dir_p1 = Vector2D(-1, 0);
    break;
    case MessageClient::InputType::RIGHT:
        m_dir_p1 = Vector2D(1, 0);
    break;
    case MessageClient::InputType::UP:
        m_dir_p1 = Vector2D(0, -1);
    break;
    case MessageClient::InputType::DOWN:
        m_dir_p1 = Vector2D(0, 1);
    break;
    case MessageClient::InputType::PLAY:
        //std::cout << "[Server]p2  ready\n";
        m_p1_ready = true;
    break;
    }
    
    // input player 2
    switch (m_input_t2)
    {
    case MessageClient::InputType::LEFT:
        m_dir_p2 = Vector2D(-1, 0);
    break;
    case MessageClient::InputType::RIGHT:
        m_dir_p2 = Vector2D(1, 0);
    break;
    case MessageClient::InputType::UP:
        m_dir_p2 = Vector2D(0, -1);
    break;
    case MessageClient::InputType::DOWN:
        m_dir_p2 = Vector2D(0, 1);
    break;
    case MessageClient::InputType::PLAY:
        //std::cout << "[Server]: p2 ready\n";
        m_p2_ready = true;
    break;
    }

    // reset input
    m_input_t1 = m_input_t2 = MessageClient::InputType::NONE;
};

bool TronServer::addPlayer(Socket *player_sock, int &pl)
{
    if ((m_tron_1 && *player_sock == *m_tron_1) || (m_tron_2 && *player_sock == *m_tron_2))
    {
        printf("Player already registered.\n");
        return false;
    }

    if (m_tron_1 != nullptr && m_tron_2 != nullptr)
    {
        printf("Can't register player, game is full.\n");
        return false;
    }

    if (m_tron_1 == nullptr)
    {
        m_tron_1 = player_sock;
        pl = 0;
        printf("Registered Player One.\n");
    }
    else
    {
        m_tron_2 = player_sock;
        pl = 1;
        printf("Registered Player Two.\n");
    }
    return true;
}

void TronServer::removePlayer(Socket *player_sock)
{
    if (m_tron_1 != nullptr && *player_sock == *m_tron_1)
    {
        printf("Player One Exit.\n");
        m_tron_1 = nullptr;
    }
    else if (m_tron_2 != nullptr && *player_sock == *m_tron_2)
    {
        printf("Player Two Exit.\n");
        m_tron_2 = nullptr;
    }
    else
        printf("Can't remove non-registered player.\n");
}

void TronServer::initPlayer(const int &pl, const MessageClient *msg)
{
    // TO DO : Initial values

    if (!pl)
    {
       m_pos_p1 = msg->m_pos;
       m_dir_p1 = msg->m_dir;
    }
    else
    {
       m_pos_p2 = msg->m_pos;
       m_dir_p2 = msg->m_dir;
    }
}

void TronServer::stepSimulation()
{
    // TO DO : 
    checkCollisions();
    
}

void TronServer::checkCollisions()
{
    // TO DO :
    // CollisionState colState = NONE_COLLIDED;

    if (m_pos_p1 == m_pos_p2){
        m_p1_hit = true;
        m_p2_hit = true;
        return;
    }

    m_p1_hit = m_tab->thereIsWall({(int) m_pos_p1.getX(),(int) m_pos_p1.getY()});
    m_p2_hit = m_tab->thereIsWall({(int) m_pos_p2.getX(),(int) m_pos_p2.getY()});


    // int i = 0;
    // while (i < players.size() && colState != BOTH_COLLIDED) {
    //     Coor head = players[i]->player->getPlayerHead();
    //     if (thisCollide(head, tab)) {
    //         switch (colState) {
    //             case NONE_COLLIDED:
    //                 if (i == 0)
    //                     colState = PLAYER1_COLLIDED;
    //                 else
    //                     colState = PLAYER2_COLLIDED;
    //                 break;
    //             case PLAYER1_COLLIDED:
    //                 colState = BOTH_COLLIDED;
    //                 break;
    //             case PLAYER2_COLLIDED:
    //             case BOTH_COLLIDED:
    //             default:
    //                 break;
    //         }
    //     }
    //     i++;
    // }
}

void TronServer::checkWinners()
{
    if(m_p1_hit && m_p2_hit)
    {
        std::cout << "Both Hit!\n";
    }
    else if(m_p1_hit){
        std::cout << "P1 Hit!\n";
    }
    else if(m_p2_hit){
        std::cout << "P2 Hit!\n";
    }
}

void TronServer::updateInfoClients()
{
    Vector2D act_dir1 = m_dir_p1;
    Vector2D act_dir2 = m_dir_p2;
    //Si los dos jugadores no han decidido dirección para empezar, enviamos dirección nula
    if(act_dir1 == Vector2D(0, 0) || act_dir2 == Vector2D(0, 0))
    {
        act_dir1 = Vector2D(0, 0);
        act_dir2 = Vector2D(0, 0);
    }

    MessageServer msg(m_pos_p1, m_pos_p2, act_dir1, act_dir2);
    msg.m_type = MessageServer::ServerMessageType::UPDATE_INFO;
    m_server_socket.send(msg, *m_tron_1);
    m_server_socket.send(msg, *m_tron_2);

    MessageServer msg2;
    msg2.m_type = MessageServer::ServerMessageType::ACTION;
    msg2.m_action = MessageServer::ActionType::MOVE;
    m_server_socket.send(msg2, *m_tron_1);
    m_server_socket.send(msg2, *m_tron_2);
}

void TronServer::sendStateMessage()
{
    MessageServer msg(m_state);
    msg.m_type = MessageServer::ServerMessageType ::UPDATE_STATE;

    // if(state == MessageServer::ServerState::GAME_OVER)
    //     msg.playerOneHasWon = life_t2 == 0;
    
    if (m_tron_1 != nullptr)
        m_server_socket.send(msg, *m_tron_1);
    if (m_tron_2 != nullptr)
        m_server_socket.send(msg, *m_tron_2);

    if (m_state != MessageServer::ServerState::PLAYING)
        reset();
}

