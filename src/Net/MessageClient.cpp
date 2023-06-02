#include "MessageClient.h"

void MessageClient::to_bin()
{
    alloc_data(CLIENT_MESSAGE_SIZE);
    memset(m_data, 0, CLIENT_MESSAGE_SIZE);

    char *tmp = m_data;

    memcpy(tmp, &m_type, sizeof(ClientMessageType));
    tmp += sizeof(ClientMessageType);

    memcpy(tmp, &m_input, sizeof(InputType));
    tmp += sizeof(InputType);
}

int MessageClient::from_bin(char *bobj)
{
    alloc_data(CLIENT_MESSAGE_SIZE);
    memcpy(static_cast<void *>(m_data), bobj, CLIENT_MESSAGE_SIZE);

    char *tmp = m_data;

    memcpy(&m_type, tmp, sizeof(ClientMessageType));
    tmp += sizeof(ClientMessageType);

    memcpy(&m_input, tmp, sizeof(InputType));

    return 0;
}