#include <unistd.h>
#include <string.h>
#include <string>
#include <cctype>

#include "Serializable.h"
#include "../SDLUtils/Vector2D.h"

#define CLIENT_MESSAGE_SIZE sizeof(ClientMessageType) + sizeof(InputType) + (2 * sizeof(Vector2D))

class MessageClient: public Serializable
{
public:
    enum ClientMessageType : uint8_t {
        REGISTER = 0,
        HANDLE_INPUT = 1,
        QUIT = 2
    };

    enum InputType : uint8_t {
        NONE = 0,
        LEFT = 1,
        RIGHT = 2,
        UP = 3,
        DOWN = 4, 
        PLAY = 5,
        ESCAPE = 6
    };

    MessageClient() {};

    MessageClient(const ClientMessageType& t, const InputType& i)
        : m_type(t), m_input(i), m_pos(Vector2D(0,0)), m_dir(Vector2D(1,0)) {};

    void setDefaultValues(const Vector2D &p, const Vector2D &d);

    void to_bin();

    int from_bin(char* bobj);

public:
    ClientMessageType m_type;

    // input value for HANDLE_INPUT
    InputType m_input;

    Vector2D m_pos;
    Vector2D m_dir;
};