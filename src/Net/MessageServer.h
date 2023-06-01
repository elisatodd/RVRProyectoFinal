#include <unistd.h>
#include <string.h>
#include <string>
#include <cctype>

#include "Serializable.h"
#include "../SDLUtils/Vector2D.h"

#define SERVER_MESSAGE_SIZE sizeof(ServerMessageType) + sizeof(ActionType) + 5 * sizeof(Vector2D) + 2 * sizeof(float) + sizeof(bool)*2 + sizeof(ServerState) + sizeof(int)

class MessageServer: public Serializable
{
public:
    enum ServerMessageType : uint8_t {
        UPDATE_STATE = 0,
        UPDATE_INFO = 1,
        ACTION = 2
    };

    enum ServerState : uint8_t {
        WAITING = 0,
        READY = 1,
        PLAYING = 2,
        GAME_OVER = 3,
        SERVER_QUIT = 4,
        EMPTY = 5
    };

    enum ActionType : uint8_t {
        NONE = 0,
        MOVE = 1
    };

    MessageServer() {};

    MessageServer(const ServerState s) : m_state(s) {};

    MessageServer(const Vector2D& pos_p1, const Vector2D& pos_p2, const Vector2D& dir_p1, const Vector2D& dir_p2, int score_p1, int score_p2) 
                : m_pos_p1(pos_p1), m_pos_p2(pos_p2), m_dir_p1(dir_p1), m_dir_p2(dir_p2), m_score_p1(score_p1), m_score_p2(score_p2) {};

    void to_bin();

    int from_bin(char* bobj);

public:

    ActionType m_action;
    ServerMessageType m_type;

    Vector2D m_pos_p1, m_pos_p2;
    Vector2D m_dir_p1, m_dir_p2;

    int m_score_p1, m_score_p2;

    bool playerOneHasWon;
    ServerState m_state;
};
