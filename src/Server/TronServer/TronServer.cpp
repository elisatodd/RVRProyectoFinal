#include "TronServer.h"

#include <time.h>
#include <signal.h>
#include <iostream>
#include <cassert>

#include <SDL2/SDL.h>

// Ctrl C handler
bool m_exit = false;
void intHandler(int){
    m_exit = true;
}

TronServer::TronServer(const char* s, const char* p) : m_server_socket(s, p){
    // random seed
    srand((unsigned)time(NULL));

    // bind socket
    m_server_socket.bind();

    // ctrl c handler init
    struct sigaction act;
    act.sa_handler = intHandler;
    sigaction(SIGINT, &act, NULL);

    // default values
    m_tron_1 = m_tron_2 = nullptr;
    m_pos_p1 = m_pos_p2 = Vector2D(0, 0);
    m_dir_p1 = m_dir_p2 = Vector2D(0, 0);

    m_state = MessageServer::ServerState::WAITING;

    reset();
};

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
        m_input_mutex.lock(); // mutex lock to protect from alterations
        handleInput();
        m_input_mutex.unlock();

        if (m_state == MessageServer::ServerState::READY && m_p1_ready && m_p2_ready)
        {
            // Both players are ready and server is also ready --> game can start
            m_state = MessageServer::ServerState::PLAYING;
            m_p1_ready = m_p2_ready = false;
            sendStateMessage();
        }

        if (m_state == MessageServer::ServerState::PLAYING)
        {
            // Game is active --> game loop
            stepSimulation();
            updateInfoClients();
            usleep(TICK_RATE);
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
    m_pos_p1 = Vector2D(200, 360); // default positions
    m_pos_p2 = Vector2D(800, 360);

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
    case MessageClient::InputType::FRONT:
        m_dir_p1 = Vector2D(0, -1);
    break;
    case MessageClient::InputType::BACK:
        m_dir_p1 = Vector2D(0, 1);
    break;
    case MessageClient::InputType::PLAY:
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
    case MessageClient::InputType::FRONT:
        m_dir_p2 = Vector2D(0, -1);
    break;
    case MessageClient::InputType::BACK:
        m_dir_p2 = Vector2D(0, 1);
    break;
    case MessageClient::InputType::PLAY:
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
    m_win_widthL = msg->m_win_widthL;
    m_win_width = msg->m_win_width;
    m_win_height = msg->m_win_height;
    m_win_heightT = msg->m_win_heightT;

    // TO DO : Initial values

    // if (!pl)
    // {
    //     dim_t1 = msg->dim;
    //     rot_t1 = msg->rot;
    // }
    // else
    // {
    //     dim_t2 = msg->dim;
    //     rot_t2 = msg->rot;
    // }
}

void TronServer::stepSimulation()
{
    // TO DO : 
    //checkCollisions();
}

void TronServer::checkCollisions()
{
    // TO DO :
}

void TronServer::updateInfoClients()
{
    MessageServer msg(m_pos_p1, m_pos_p2, m_dir_p1, m_dir_p2);
    msg.m_type = MessageServer::ServerMessageType::UPDATE_INFO;
    m_server_socket.send(msg, *m_tron_1);
    m_server_socket.send(msg, *m_tron_2);
}

void TronServer::sendStateMessage()
{
    MessageServer msg(m_state);
    msg.m_type = MessageServer::ServerMessageType::UPDATE_STATE;

    // if(state == MessageServer::ServerState::GAME_OVER)
    //     msg.playerOneHasWon = life_t2 == 0;
    
    if (m_tron_1 != nullptr)
        m_server_socket.send(msg, *m_tron_1);
    if (m_tron_2 != nullptr)
        m_server_socket.send(msg, *m_tron_2);

    if (m_state != MessageServer::ServerState::PLAYING)
        reset();
}

