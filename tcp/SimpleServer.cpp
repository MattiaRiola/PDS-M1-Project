#include <iostream>
#include <fstream>
#include <cstdio>
#include "olc_net.h"
#include "boost/filesystem.hpp"




class CustomServer : public olc::net::server_interface<CustomMsgTypes>
{
public:
    explicit CustomServer(uint16_t nPort) : olc::net::server_interface<CustomMsgTypes>(nPort)
    {
        login.insert(std::pair<std::string, std::string>("user1", "password"));
    }

protected:

    std::map<std::string, std::string> login;
    std::map<std::string, std::string> ft;
    bool OnClientConnect(std::shared_ptr<olc::net::connection<CustomMsgTypes>> client) override
    {
        olc::net::message<CustomMsgTypes> msg;
        msg.header.id = CustomMsgTypes::ServerAccept;
        client->Send(msg);
        return true;
    }

    // Called when a client appears to have disconnected
    void OnClientDisconnect(std::shared_ptr<olc::net::connection<CustomMsgTypes>> client) override
    {
        std::cout << "Removing client [" << client->GetID() << "]\n";
    }

    // Called when a message arrives
    void OnMessage(std::shared_ptr<olc::net::connection<CustomMsgTypes>> client, olc::net::message<CustomMsgTypes>& msg) override
    {
        switch (msg.header.id)
        {


            case CustomMsgTypes::login:
            {
                std::string a, b;
                msg >> a;
                msg >> b;
                std::cout << a << b << std::endl;
                olc::net::message<CustomMsgTypes> msg1;
                auto it = login.find(b);
                if(it->second == a) {
                    msg1.header.id = CustomMsgTypes::login;
                    client->Send(msg1);
                }
                else{
                    msg1.header.id = CustomMsgTypes::loginDeny;
                    client->Send(msg1);
                }



            }
                break;

            case CustomMsgTypes::check: {

                std::string pt;
                std::string md;
                olc::net::message<CustomMsgTypes> msg1;
                msg >> pt;
                msg >> md;
                std::map<std::string, std::string>::iterator it;
                it = ft.find(pt);
                if (it == ft.end()) {
                    msg1.header.id = CustomMsgTypes::update;
                    msg1 << pt;
                    ft.insert(std::pair<std::string, std::string>(pt, md));
                    std::ofstream ofs("/Users/samuelejakupi/Desktop/database" + pt);
                } else {

                    if (it->second == md) {
                        msg1.header.id = CustomMsgTypes::fileok;

                    } else {
                        msg1.header.id = CustomMsgTypes::update;
                        it->second = md;
                        std::filesystem::remove("/Users/samuelejakupi/Desktop/database" + pt);
                        std::ofstream ofs("/Users/samuelejakupi/Desktop/database" + pt);
                        msg1 << pt;

                    }
                }
                client->Send(msg1);
            }
                break;

            case CustomMsgTypes::update: {
                std::string pt;
                msg >> pt;
                int bytes;
                pt = std::filesystem::path("/Users/samuelejakupi/Desktop/database" + pt);

                std::filesystem::create_directories(pt.substr(0, pt.find_last_of('/')));
                std::ofstream ofs;
                ofs.open(pt, std::ios_base::app);
                unsigned char data[8192];
                msg >> bytes;
                if(bytes == 8192){
                    msg >> data;
                    ofs << data;
                }
                else{
                    msg >> data;
                    ofs.write( (char*)data, bytes );
                }
                ofs.close();
            }

                break;

            case CustomMsgTypes::erase:{
                std::string pt;
                msg >> pt;
                std::map<std::string, std::string>::iterator it;
                it = ft.find(pt);
                if(it!= ft.end()) {
                    ft.erase(it);
                    std::filesystem::remove("/Users/samuelejakupi/Desktop/database" + pt);
                }
            }
                break;
            case CustomMsgTypes::ServerDeny:
                break;
            case CustomMsgTypes::ServerMessage:
                break;
            case CustomMsgTypes::loginDeny:
                break;

            case CustomMsgTypes::ServerAccept:
                break;
            case CustomMsgTypes::fileok:
                break;
        }
    }
};

int main()
{
    CustomServer server(60000);
    server.Start();

    while (1)
    {

        server.Update(-1, true);
    }



    return 0;
}