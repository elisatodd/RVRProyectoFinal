#include "MessageClient.h"

void MessageClient::setDefaultValues(const Vector2D &p, const Vector2D &d)
{
    m_pos = p; m_dir = d; 
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

    memcpy(tmp, &m_pos, sizeof(Vector2D));
    tmp += sizeof(Vector2D);

    memcpy(tmp, &m_dir, sizeof(Vector2D));
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

    memcpy(&m_pos, tmp, sizeof(Vector2D));
    tmp += sizeof(Vector2D);

    memcpy(&m_dir, tmp, sizeof(Vector2D));

    return 0;
}