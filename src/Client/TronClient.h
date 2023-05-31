#ifndef TRONCLIENT_H_
#define TRONCLIENT_H_

#include <unistd.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <thread>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>

#include "../Net/Socket.h"

#include "../Net/MessageClient.h"
#include "../Net/MessageServer.h"

#include "../SDLUtils/Vector2D.h"

#include "GameUtils/Player.h"

#define PLAYER_SIZE 10 // 10x10

class GameObject;
class SDL_Renderer;

struct InitData
{
    Vector2D size;
    Vector2D position;
};

class TronClient
{
public:
    TronClient(const char *s, const char *p);
    virtual ~TronClient();

    // main methods for game loop: initialization, run and shutdown
    void init(int w, int h); 
    void run();
    void shutdown();

    // send message to server
    void sendGameMessage(MessageClient::InputType input);
    void sendMatchMessage(MessageClient::ClientMessageType msg, InitData *data = nullptr);

    inline std::vector<GameObject *> *getGOsReference()
    {
        return &objs_;
    };

private:
    Socket client_socket;

    // general game objects vector for loop
    std::vector<GameObject *> objs_;
    std::vector<GameObject *> gObjsToAdd_;

    Player* m_player_1;
    Player* m_player_2;

    MessageServer::ServerState currentState, nextState;

    // message thread
    void client_message_thread();

    // check & change gOs according to new game state
    void checkState();
    void changeState(const MessageServer::ServerState state);

    // update gOs according to data received from a message
    void updateGOsInfo(MessageServer *msg);

   // void changeState(const MessageServer::ServerState state);
    void loadBackground(const std::string &textFile);
    void loadGame();
    void playLoad();

    void refresh(); // cleans gOs if they are disabled
    void clearGameObjects(); // clears all gOs
};
#endif
