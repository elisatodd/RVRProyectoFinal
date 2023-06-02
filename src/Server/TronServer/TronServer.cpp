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

    // ctrl+c / exit handler init
    struct sigaction act;
    act.sa_handler = intHandler;
    sigaction(SIGINT, &act, NULL);

    // default player values
    m_tron_1 = m_tron_2 = nullptr;
    m_pos_p1 = m_pos_p2 = Vector2D(0, 0);
    m_dir_p1 = m_dir_p2 = Vector2D(0, 0);
    m_score_p1 = m_score_p2 = 0;

    m_state = MessageServer::ServerState::WAITING;

    // create game elements
    m_tab = new Tablero("assets/levels/level1.txt");

    // Set players in the initial position given by the board
    Coor c1 = m_tab->getPlayerOneInitialPosition(); 
    m_pos_p1 = Vector2D(c1.x, c1.y);
    Coor c2 = m_tab->getPlayerTwoInitialPosition(); 
    m_pos_p2 = Vector2D(c2.x, c2.y);

    reset();
};

void TronServer::shutdown(){
    if(m_tab != nullptr)
        delete m_tab;
}

void TronServer::server_message_thread()
{
    // main loop that receives messages from clients
    while (!m_exit)
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

            // Both players are initialized --> game can start
            if (m_tron_1 != nullptr && m_tron_2 != nullptr)
                m_state = MessageServer::ServerState::READY;
            else
            // One player is missing --> wait for them
                m_state = MessageServer::ServerState::WAITING;

            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            sendStateMessage();
        }
        break;
        case MessageClient::ClientMessageType::QUIT:
        { // remove player from game
            removePlayer(client_player_sock);

            // One or both players missing --> back to waiting
            if (m_tron_1 == nullptr || m_tron_2 == nullptr)
            {
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
    // main loop that sends messages to the clients
    while (!m_exit)
    {
        Uint32 startTime = SDL_GetTicks();

        m_input_mutex.lock(); // lock to ensure correct input
        handleInput();
        m_input_mutex.unlock();

        if (m_state == MessageServer::ServerState::READY && m_p1_ready && m_p2_ready)
        {
            // Both players are ready and server is also ready --> game can start
            m_state = MessageServer::ServerState::PLAYING;
            m_p1_ready = m_p2_ready = false;
            std::cout << "[Server]: Both players are ready: starting game.\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            sendStateMessage();
        }

        if (m_state == MessageServer::ServerState::PLAYING)
        {
            // Game is active --> game loop
            stepSimulation(); // do game logic
            updateInfoClients(); // and communicate it to the clients

            // We cause a delay so the movement isn't too fast
            Uint32 frameTime = SDL_GetTicks() - startTime;
            if (frameTime < 200)
                std::this_thread::sleep_for(std::chrono::milliseconds(200 - frameTime));
        }

        if (m_state == MessageServer::ServerState::GAME_OVER){
            // Game is finished --> start round again
            if(SDL_GetTicks() - m_timer > GAME_OVER_TIME) {
                m_timer = 0;
                m_state = MessageServer::ServerState::READY;
                sendStateMessage();
            }
        }

    }

    m_state = MessageServer::ServerState::SERVER_QUIT;
    sendStateMessage();    
    std::cout << "[Server]: Shutting down server.\n";
}

void TronServer::reset()
{
    // Sets game state to initial state

    m_input_t1 = m_input_t2 = MessageClient::InputType::NONE;

    m_p1_hit = false;
    m_p2_hit = false;

    m_p1_ready = false;
    m_p2_ready = false;

    Coor c1 = m_tab->getPlayerOneInitialPosition(); 
    Coor c2 = m_tab->getPlayerTwoInitialPosition(); 

    m_pos_p1 = Vector2D(c1.x, c1.y);
    m_pos_p2 = Vector2D(c2.x, c2.y);

    m_score_p1 = 0;
    m_score_p2 = 0;
    
    m_tab->ResetTableroToDefault();
}

void TronServer::onRoundFinished()
{
    // Resets positions so round can start again

    m_p1_hit = false;
    m_p2_hit = false;

    Coor c1 = m_tab->getPlayerOneInitialPosition(); 
    Coor c2 = m_tab->getPlayerTwoInitialPosition(); 

    m_pos_p1 = Vector2D(c1.x, c1.y);
    m_pos_p2 = Vector2D(c2.x, c2.y);

    m_dir_p1 = Vector2D(0, 0);
    m_dir_p2 = Vector2D(0, 0);

    m_tab->ResetTableroToDefault();

    // Tells clients so they render correctly
    MessageServer msg;
    msg.m_type = MessageServer::ServerMessageType::ROUND_FINISHED;
    m_server_socket.send(msg, *m_tron_1);
    m_server_socket.send(msg, *m_tron_2);
}

void TronServer::saveInput(Socket *player_sock, MessageClient::InputType input)
{
    if (*player_sock == *m_tron_1)
        m_input_t1 = input;
    else if (*player_sock == *m_tron_2)
        m_input_t2 = input;
    else
        printf("[Server]: Received input from a unregistered client.\n");
}

void TronServer::handleInput()
{
    // input player 1
    switch (m_input_t1)
    {
    case MessageClient::InputType::LEFT:
        if(m_dir_p1 != Vector2D(1, 0)){
            m_dir_p1 = Vector2D(-1, 0);
        }
    break;
    case MessageClient::InputType::RIGHT:
        if(m_dir_p1 != Vector2D(-1, 0))
            m_dir_p1 = Vector2D(1, 0);
    break;
    case MessageClient::InputType::UP:
        if(m_dir_p1 != Vector2D(0, 1))
            m_dir_p1 = Vector2D(0, -1);
    break;
    case MessageClient::InputType::DOWN:
        if(m_dir_p1 != Vector2D(0, -1))
            m_dir_p1 = Vector2D(0, 1);
    break;
    case MessageClient::InputType::PLAY:
        //std::cout << "[Server]p1  ready\n";
        m_p1_ready = true;
    break;
    }
    
    // input player 2
    switch (m_input_t2)
    {
    case MessageClient::InputType::LEFT:
        if(m_dir_p2 != Vector2D(1, 0))
            m_dir_p2 = Vector2D(-1, 0);
    break;
    case MessageClient::InputType::RIGHT:
        if(m_dir_p2 != Vector2D(-1, 0))
            m_dir_p2 = Vector2D(1, 0);
    break;
    case MessageClient::InputType::UP:
        if(m_dir_p2 != Vector2D(0, 1))
            m_dir_p2 = Vector2D(0, -1);
    break;
    case MessageClient::InputType::DOWN:
        if(m_dir_p2 != Vector2D(0, -1))
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
        printf("[Server]: Player already registered.\n");
        return false;
    }

    if (m_tron_1 != nullptr && m_tron_2 != nullptr)
    {
        printf("[Server]: Can't register player, game is full.\n");
        return false;
    }

    if (m_tron_1 == nullptr)
    {
        m_tron_1 = player_sock;
        pl = 0;
        printf("[Server]: Registered Player One.\n");
    }
    else
    {
        m_tron_2 = player_sock;
        pl = 1;
        printf("[Server]: Registered Player Two.\n");
    }
    return true;
}

void TronServer::removePlayer(Socket *player_sock)
{
    if (m_tron_1 != nullptr && *player_sock == *m_tron_1)
    {
        printf("[Server]: Player One Exit.\n");
        m_tron_1 = nullptr;
    }
    else if (m_tron_2 != nullptr && *player_sock == *m_tron_2)
    {
        printf("[Server]: Player Two Exit.\n");
        m_tron_2 = nullptr;
    }
    else
        printf("[Server]: Can't remove non-registered player.\n");
}

void TronServer::stepSimulation()
{
    checkCollisions();
    checkWinners();

    // Move player if both are moving
    if(playersAlreadyMoving())
    {
        m_tab->setWall(Coor(m_pos_p1.getX(), m_pos_p1.getY()));
        m_tab->setWall(Coor(m_pos_p2.getX(), m_pos_p2.getY()));

        // Sets a wall before to enable the collision afterwards
        m_pos_p1 += m_dir_p1;
        m_pos_p2 += m_dir_p2;
    }

    //std::cout << "P1(" << m_pos_p1.getX() << "," <<  m_pos_p1.getY() << ") ";
    //std::cout << "P2(" << m_pos_p2.getX() << "," <<  m_pos_p2.getY() << ")\n";
}

void TronServer::checkCollisions()
{
    // Only check when players have started moving
    if(!playersAlreadyMoving())
    {
        return;
    }

    if (m_pos_p1 == m_pos_p2){
        m_p1_hit = true;
        m_p2_hit = true;

        //std::cout << "Both hit in the same position:\n";
        //std::cout << "P1(" << m_pos_p1.getX() << "," <<  m_pos_p1.getY() << ")\n";
        //std::cout << "P2(" << m_pos_p2.getX() << "," <<  m_pos_p2.getY() << ")\n";
        return;
    }

    m_p1_hit = m_tab->thereIsWall({(int) m_pos_p1.getX(),(int) m_pos_p1.getY()});
    m_p2_hit = m_tab->thereIsWall({(int) m_pos_p2.getX(),(int) m_pos_p2.getY()});
}

void TronServer::checkWinners()
{
    // if no one won, return
    if(!m_p1_hit && !m_p2_hit)
        return;

    // Wait before reset to show the player who was the one who collided
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));

    // Check who collided and restart
    if(m_p1_hit && m_p2_hit)
    {
        std::cout << "[Server]: Both Hit!\n";
        onRoundFinished();
    }
    else if(m_p1_hit){
        std::cout << "[Server]: P1 Hit!\n";
        m_score_p2++;
        onRoundFinished();
    }
    else{
        std::cout << "[Server]: P2 Hit!\n";
        m_score_p1++;
        onRoundFinished();
    }
}

bool TronServer::playersAlreadyMoving()
{
    // Checks if both players have chosen a valid direction so the game can start
    return m_dir_p1 != Vector2D(0, 0) && m_dir_p2 != Vector2D(0, 0);
}

void TronServer::updateInfoClients()
{
    Vector2D act_dir1 = m_dir_p1;
    Vector2D act_dir2 = m_dir_p2;

    // Wait for both players to choose a direction for the game to start
    if(act_dir1 == Vector2D(0, 0) || act_dir2 == Vector2D(0, 0))
    {
        act_dir1 = Vector2D(0, 0);
        act_dir2 = Vector2D(0, 0);
    }

    // Update the clients with render information
    MessageServer msg(m_pos_p1, m_pos_p2, act_dir1, act_dir2, m_score_p1, m_score_p2);
    msg.m_type = MessageServer::ServerMessageType::UPDATE_INFO;
    m_server_socket.send(msg, *m_tron_1);
    m_server_socket.send(msg, *m_tron_2);

    // Update the clients with game logic information
    MessageServer msg2;
    msg2.m_type = MessageServer::ServerMessageType::ACTION;
    msg2.m_action = MessageServer::ActionType::MOVE;
    m_server_socket.send(msg2, *m_tron_1);
    m_server_socket.send(msg2, *m_tron_2);
}

void TronServer::sendStateMessage()
{
    // Sends message to the clients telling current state

    MessageServer msg(m_state);
    msg.m_type = MessageServer::ServerMessageType ::UPDATE_STATE;
    
    if (m_tron_1 != nullptr)
        m_server_socket.send(msg, *m_tron_1);
    if (m_tron_2 != nullptr)
        m_server_socket.send(msg, *m_tron_2);

    if (m_state != MessageServer::ServerState::PLAYING)
        reset();
}

