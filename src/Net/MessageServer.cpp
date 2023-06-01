#include "MessageServer.h"

void MessageServer::to_bin() {
    alloc_data(SERVER_MESSAGE_SIZE);
    memset(m_data, 0, SERVER_MESSAGE_SIZE);

    char *tmp = m_data;

    memcpy(tmp, &m_pos_p1, sizeof(Vector2D));
    tmp += sizeof(Vector2D);

    memcpy(tmp, &m_pos_p2, sizeof(Vector2D));
    tmp += sizeof(Vector2D);

    memcpy(tmp, &m_dir_p1, sizeof(Vector2D));
    tmp += sizeof(Vector2D);

    memcpy(tmp, &m_dir_p2, sizeof(Vector2D));
    tmp += sizeof(Vector2D);

    memcpy(tmp, &m_score_p1, sizeof(int));
    tmp += sizeof(int);

    memcpy(tmp, &m_score_p2, sizeof(int));
    tmp += sizeof(int);

    memcpy(tmp, &m_type, sizeof(ServerMessageType));
    tmp += sizeof(ServerMessageType);

    memcpy(tmp, &m_action, sizeof(ActionType));
    tmp += sizeof(ActionType);

    memcpy(tmp, &m_state, sizeof(ServerState));
    tmp += sizeof(ServerState);

    memcpy(tmp, &playerOneHasWon, sizeof(bool));
}

int MessageServer::from_bin(char * bobj)
{
    alloc_data(SERVER_MESSAGE_SIZE);
    memcpy(static_cast<void*>(m_data), bobj, SERVER_MESSAGE_SIZE);

    char *tmp = m_data;

    memcpy(&m_pos_p1, tmp, sizeof(Vector2D));
    tmp += sizeof(Vector2D);

    memcpy(&m_pos_p2, tmp, sizeof(Vector2D));
    tmp += sizeof(Vector2D);

    memcpy(&m_dir_p1, tmp, sizeof(Vector2D));
    tmp += sizeof(Vector2D);

    memcpy(&m_dir_p2, tmp, sizeof(Vector2D));
    tmp += sizeof(Vector2D);

    memcpy(&m_type,tmp, sizeof(ServerMessageType));
    tmp += sizeof(ServerMessageType);

    memcpy(&m_action, tmp, sizeof(ActionType));
    tmp += sizeof(ActionType);

    memcpy(&m_state, tmp, sizeof(ServerState));
    tmp += sizeof(ServerState);

    memcpy(&playerOneHasWon,tmp, sizeof(bool));

    return 0;
}
