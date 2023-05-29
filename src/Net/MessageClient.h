#include <unistd.h>
#include <string.h>
#include <string>
#include <cctype>

#include "Serializable.h"
#include "../SDLUtils/Vector2D.h"

#define CLIENT_MESSAGE_SIZE sizeof(ClientMessageType) + sizeof(InputType) + (4 * sizeof(int)) + sizeof(float) + (2 * sizeof(Vector2D))

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
        FRONT = 3,
        BACK = 4, 
        PLAY = 5,
        ESCAPE = 6
    };

    MessageClient() {};

    MessageClient(const ClientMessageType& t, const InputType& i)
        : m_type(t), m_input(i), m_win_widthL(0), m_win_width(0), m_win_heightT(0), m_win_height(0), m_size() {};

    void setDefaultValues(const int &w_w, const int &w_h, const Vector2D &d);
    void setDefaultValues(const int &w_wL, const int &w_w, const int &w_hT, const int &w_h, const Vector2D &d);

    void to_bin();

    int from_bin(char* bobj);

public:
    ClientMessageType m_type;

    // input value for HANDLE_INPUT
    InputType m_input;

    // initial values for REGISTER message
    int m_win_widthL, m_win_width, m_win_heightT, m_win_height;

    Vector2D m_size;
};