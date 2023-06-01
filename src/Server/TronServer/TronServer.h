#include <unistd.h>
#include <string>
#include <mutex>
#include <vector>

#include "../../Net/Socket.h"

#include "../../Net/MessageClient.h"
#include "../../Net/MessageServer.h"

#include "../../SDLUtils/Vector2D.h"


#define TICK_RATE 16666 // ms
#define GAME_OVER_TIME 2000

class Tablero;
class Coor;
class TronServer {
public:
    TronServer(const char* s, const char* p);

    // net messages handler
    void server_message_thread();
    void shutdown();
    
    // main game loop
    void run();

private:
    // sockets
    Socket m_server_socket;
    Socket* m_tron_1;
    Socket* m_tron_2;

    std::mutex m_input_mutex; // mutex lock to handle input safely

    // server state
    MessageServer::ServerState m_state;
    bool m_p1_ready, m_p2_ready;

    int m_timer; // game over timer
    int m_disconnect_timer;  //Timer to disconnect if no players

    MessageClient::InputType m_input_t1, m_input_t2; 
    
    // Players info
    Vector2D m_pos_p1, m_pos_p2;
    Vector2D m_dir_p1, m_dir_p2;
    bool m_p1_hit, m_p2_hit;

    //Relevant Game Info
    Tablero* m_tab;

    // reset to default values
    void reset();
    void onRoundFinished();

    // save & handle input
    void saveInput(Socket* player_sock, MessageClient::InputType input);
    void handleInput();

    // add & remove player from server data
    bool addPlayer(Socket* player_sock, int& pl);
    void removePlayer(Socket* player_sock);
    void initPlayer(const int& pl, const MessageClient* msg);

    // update loop simulation
    void stepSimulation();

    // physics methods for update simulation
    void checkCollisions();
    void checkWinners();

    // update data to players
    void updateInfoClients();

    // update server state to players
    void sendStateMessage();
};