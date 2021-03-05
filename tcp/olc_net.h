
#ifndef TCP_OLC_NET_H
#define TCP_OLC_NET_H
#pragma once

#include "net_common.h"
#include "net_tsqueue.h"
#include "net_message.h"
#include "net_server.h"
#include "net_connection.h"


enum class CustomMsgTypes : uint32_t
{
        ServerAccept,
        fileok,
        ServerDeny,
        ServerMessage,
        login,
        update,
        loginDeny,
        check,
        erase
};
#endif //TCP_OLC_NET_H
