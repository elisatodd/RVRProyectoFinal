#include "MessageClient.h"

void MessageClient::setDefaultValues(const int &w_wL, const int &w_w, const int &w_hT, const int &w_h, const Vector2D &d)
{
    m_win_widthL = w_wL; m_win_heightT = w_hT;
    m_win_width = w_w; m_win_height = w_h;
    m_size = d;
};

void MessageClient::setDefaultValues(const int &w_w, const int &w_h, const Vector2D &d)
{
    m_win_width = w_w; m_win_height = w_h;
    m_size = d; 
};

void MessageClient::to_bin()
{
    alloc_data(CLIENT_MESSAGE_SIZE);
    memset(m_data, 0, CLIENT_MESSAGE_SIZE);

    char *tmp = m_data;

    memcpy(tmp, &m_type, sizeof(ClientMessageType));
    tmp += sizeof(ClientMessageType);

    memcpy(tmp, &m_input, sizeof(InputType));
    tmp += sizeof(InputType);

    memcpy(tmp, &m_win_widthL, sizeof(int));
    tmp += sizeof(int);

    memcpy(tmp, &m_win_width, sizeof(int));
    tmp += sizeof(int);

    memcpy(tmp, &m_win_heightT, sizeof(int));
    tmp += sizeof(int);

    memcpy(tmp, &m_win_height, sizeof(int));
    tmp += sizeof(int);

    memcpy(tmp, &m_size, sizeof(Vector2D));
//    tmp += sizeof(Vector2D);
}

int MessageClient::from_bin(char *bobj)
{
    alloc_data(CLIENT_MESSAGE_SIZE);
    memcpy(static_cast<void *>(m_data), bobj, CLIENT_MESSAGE_SIZE);

    char *tmp = m_data;

    memcpy(&m_type, tmp, sizeof(ClientMessageType));
    tmp += sizeof(ClientMessageType);

    memcpy(&m_input, tmp, sizeof(InputType));
    tmp += sizeof(InputType);

    memcpy(&m_win_widthL, tmp, sizeof(int));
    tmp += sizeof(int);

    memcpy(&m_win_width, tmp, sizeof(int));
    tmp += sizeof(int);

    memcpy(&m_win_heightT, tmp, sizeof(int));
    tmp += sizeof(int);

    memcpy(&m_win_height, tmp, sizeof(int));
    tmp += sizeof(int);

    memcpy(&m_size, tmp, sizeof(Vector2D));
  //  tmp += sizeof(Vector2D);

    return 0;
}