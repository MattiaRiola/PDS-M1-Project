#include <iostream>
#include <fstream>
#include <cstdio>
#include "olc_net.h"
#include "boost/filesystem.hpp"




class CustomServer : public olc::net::server_interface<CustomMsgTypes>
{
public:
    explicit CustomServer(uint16_t nPort) : olc::net::server_interface<CustomMsgTypes>(nPort){};

protected:


    std::map<std::string, std::map<std::string, std::string>> ft;
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
        //qui andrebbe levata dalla tabella ft la entry relativa all user che si è disconnesso
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
                olc::net::message<CustomMsgTypes> msg1;

                std::string pt = "/Users/samuelejakupi/Desktop/database/users.txt";
                char *cs = new char[pt.length() + 1];
                strcpy(cs, pt.c_str());
                std::string line;
                std::ifstream myfile;
                myfile.open(cs);
                bool c = false;

                if(myfile.is_open()) {
                    while (getline(myfile, line) && !c) {
                        std::string name, password;
                        int m = 0;

                        m = line.find(' ', m);
                        name = line.substr(0, m);
                        password = line.substr(m + 1, line.length() - 1);

                        if (b == name && a == password) {
                            msg1.header.id = CustomMsgTypes::login;
                            client->Send(msg1);
                            c = true;
                        }
                    }
                    myfile.close();
                }

                if (c) {
                    std::map<std::string, std::string> list;
                    ft.insert(std::pair<std::string,std::map<std::string, std::string>> (b, list));
                    pt = "/Users/samuelejakupi/Desktop/database/" + b + "/filetable.txt";
                    cs = new char[pt.length() + 1];
                    strcpy(cs, pt.c_str());
                    myfile.open(cs);
                    std::map<std::string, std::map<std:: string, std::string>>::iterator it;
                    it = ft.find(b);


                    if (myfile.is_open()) {
                        while (getline(myfile, line)) {
                            std::string line2;
                            getline(myfile, line2);
                            it->second.insert(std::pair<std::string, std::string> (line, line2));
                        }
                        myfile.close();
                    }
                }

                else{
                    msg1.header.id = CustomMsgTypes::loginDeny;
                    client->Send(msg1);
                }



            }
                break;

            case CustomMsgTypes::check: {

                std::string user;
                std::string pt, pt1;
                std::string md;
                olc::net::message<CustomMsgTypes> msg1;
                msg >> user;
                msg >> pt;
                msg >> md;
                std::map<std::string, std::map<std:: string, std::string>>::iterator it;
                std::map<std::string , std::string>::iterator it1;
                it = ft.find(user);
                it1 = it->second.find(pt);
                if (it1 == it->second.end()) {
                    msg1.header.id = CustomMsgTypes::update;
                    msg1 << pt;
                    it->second.insert(std::pair<std::string, std::string> (pt, md));
                    std::ofstream ofs("/Users/samuelejakupi/Desktop/database/"+ user + pt);


                    pt1 = "/Users/samuelejakupi/Desktop/database/";
                    pt1.append(user+ "/filetable.txt");
                    std::ofstream outfile;
                    outfile.open(pt1, std::ios_base::app); // append instead of overwrite
                    outfile << pt + "\n" + md + "\n";
                    ofs.close();
                    outfile.close();

                } else {


                    if (it1->second == md) {
                        msg1.header.id = CustomMsgTypes::fileok;

                    } else {
                        msg1.header.id = CustomMsgTypes::update;
                        it1->second = md;
                        std::filesystem::remove("/Users/samuelejakupi/Desktop/database/"+ user + pt);
                        std::ofstream ofs("/Users/samuelejakupi/Desktop/database/" + user + pt);
                        std::filesystem::remove("/Users/samuelejakupi/Desktop/database/" + user+ "/filetable.txt");
                        std::ofstream ofs1("/Users/samuelejakupi/Desktop/database/" + user+ "/filetable.txt");
                        for(it1 = it->second.begin(); it1 != it->second.end(); it++)
                            ofs1 << it1->first << "\n" << it1->second << "\n";
                        msg1 << pt;
                        ofs.close();
                        ofs1.close();
                    }
                }
                client->Send(msg1);
            }
                break;

            case CustomMsgTypes::update: {
                std::string pt;
                std::string user;
                msg >> user;
                msg >> pt;
                int bytes;
                pt = std::filesystem::path("/Users/samuelejakupi/Desktop/database/"+ user + pt);

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
                std::string user;
                msg >> pt;
                msg >> user;
                std::map<std::string, std::map<std:: string, std::string>>::iterator it;
                std::map<std::string , std::string>::iterator it1;
                it = ft.find(user);
                it1 = it->second.find(pt);
                if(it!= ft.end()) {
                    it->second.erase(it1);
                    std::filesystem::remove("/Users/samuelejakupi/Desktop/database/"+ user + pt);
                    std::filesystem::remove("/Users/samuelejakupi/Desktop/database/" + user+ "/filetable.txt");
                    std::ofstream ofs1("/Users/samuelejakupi/Desktop/database/" + user+ "/filetable.txt");
                    for(it1 = it->second.begin(); it1 != it->second.end(); it1++)
                        ofs1 << it1->first << "\n" << it1->second << "\n";
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