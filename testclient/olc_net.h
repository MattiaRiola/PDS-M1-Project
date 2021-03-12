
#ifndef TCP_OLC_NET_H
#define TCP_OLC_NET_H
#pragma once

#include "net_common.h"
#include "net_tsqueue.h"
#include "net_message.h"
#include "net_client.h"
#include "net_connection.h"
#include "md5.h"
#include "boost/filesystem.hpp"



#define MAX_STR_SIZE 500


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
    erase,
    final,
    disconnetti
};

std::string pop_string(olc::net::message<CustomMsgTypes>* msg){
    char buf[MAX_STR_SIZE];
    int num_char;
    (*msg) >> num_char;
    for(int i = 0; i <= num_char; i++){
        (*msg) >> buf[i];
    }
    std::string str(buf);
    return str;
}


void push_string(olc::net::message<CustomMsgTypes>* msg, std::string str){
    char buf[MAX_STR_SIZE];
    int num_char = str.length();
    strcpy(buf,str.c_str());
    for(int i = num_char; i >= 0; i-- ){
        (*msg) << buf[i];
    }

    (*msg)<< num_char;

}

#endif //TCP_OLC_NET_H
