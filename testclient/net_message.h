//
// Created by Samuele Jakupi on 22/02/21.
//

#ifndef TCP_NET_MESSAGE_H
#define TCP_NET_MESSAGE_H
#pragma once
#include "net_common.h"

namespace olc::net
    {
        ///[OLC_HEADERIFYIER] START "MESSAGE"

        // Message Header is sent at start of all messages. The template allows us
        // to use "enum class" to ensure that the messages are valid at compile time
        template <typename T>
        struct message_header
        {
            T id{};
            uint32_t size = 0;
        };

        // Message Body contains a header and a std::vector, containing raw bytes
        // of infomation. This way the message can be variable length, but the size
        // in the header must be updated.
        template <typename T>
        struct message
        {
            // Header & Body vector
            message_header<T> header{};
            std::vector<uint8_t> body;

            // returns size of entire message packet in bytes
            size_t size() const
            {
                return body.size();
            }

            // Override for std::cout compatibility - produces friendly description of message
            friend std::ostream& operator << (std::ostream& os, const message<T>& msg)
            {
                os << "ID:" << int(msg.header.id) << " Size:" << msg.header.size;
                return os;
            }

            // Convenience Operator overloads - These allow us to add and remove stuff from
            // the body vector as if it were a stack, so First in, Last Out. These are a
            // template in itself, because we dont know what data type the user is pushing or
            // popping, so lets allow them all. NOTE: It assumes the data type is fundamentally
            // Plain Old Data (POD). TLDR: Serialise & Deserialise into/from a vector

            // Pushes any POD-like data into the message buffer
            template<typename DataType>
            friend message<T>& operator << (message<T>& msg, const DataType& data)
            {
                // Check that the type of the data being pushed is trivially copyable
                static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pushed into vector");

                // Cache current size of vector, as this will be the point we insert the data
                size_t i = msg.body.size();

                // Resize the vector by the size of the data being pushed
                msg.body.resize(msg.body.size() + sizeof(DataType));

                // Physically copy the data into the newly allocated vector space
                std::memcpy(msg.body.data() + i, &data, sizeof(DataType));

                // Recalculate the message size
                msg.header.size = msg.size();

                // Return the target message so it can be "chained"
                return msg;
            }

            // Pulls any POD-like data form the message buffer
            template<typename DataType>
            friend message<T>& operator >> (message<T>& msg, DataType& data)
            {
                // Check that the type of the data being pushed is trivially copyable
                static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pulled from vector");

                // Cache the location towards the end of the vector where the pulled data starts
                size_t i = msg.body.size() - sizeof(DataType);

                // Physically copy the data from the vector into the user variable
                std::memcpy(&data, msg.body.data() + i, sizeof(DataType));

                // Shrink the vector to remove read bytes, and reset end position
                msg.body.resize(i);

                // Recalculate the message size
                msg.header.size = msg.size();

                // Return the target message so it can be "chained"
                return msg;
            }
        };


        // An "owned" message is identical to a regular message, but it is associated with
        // a connection. On a server, the owner would be the client that sent the message,
        // on a client the owner would be the server.

        // Forward declare the connection
        template <typename T>
        class connection;

        template <typename T>
        struct owned_message
        {
            std::shared_ptr<connection<T>> remote = nullptr;
            message<T> msg;

            // Again, a friendly string maker
            friend std::ostream& operator<<(std::ostream& os, const owned_message<T>& msg)
            {
                os << msg.msg;
                return os;
            }
        };

        ///[OLC_HEADERIFYIER] END "MESSAGE"
    }
#endif //TCP_NET_MESSAGE_H
