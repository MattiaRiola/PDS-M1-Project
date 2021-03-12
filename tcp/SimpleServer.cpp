#include <iostream>
#include <fstream>
#include <shared_mutex>
#include "olc_net.h"
#include "boost/filesystem.hpp"
#include "fcb.h"
//#define PATH_DATABASE "/Users/mattia/Desktop/database/"
//#define PATH_USER "/Users/mattia/Desktop/database/users.txt"

#define PATH_DATABASE "../database/"
#define PATH_USER "../database/users.txt"
#define VERBOSE true

#define N_MAX_THREAD 6
int msg_debug_counter = 0;


void print_message(olc::net::message<CustomMsgTypes>* msg, char mode){
    if(mode == 's')
        std::cout << msg_debug_counter << " | sending message:" << std::endl;
    if(mode == 'r')
        std::cout << msg_debug_counter << " | received message:" << std::endl;

    switch(msg->header.id) {
        case CustomMsgTypes::ServerAccept:
            std::cout << "[Server Accept] : ";
            break;
        case CustomMsgTypes::ServerDeny:
            std::cout << "[Server Deny] : ";
            break;
        case CustomMsgTypes::ServerMessage:
            std::cout << "[Server Message] : ";
            break;
        case CustomMsgTypes::check:
            std::cout << "[check] : ";
            break;
        case CustomMsgTypes::erase:
            std::cout << "[erase] : ";
            break;
        case CustomMsgTypes::fileok:
            std::cout << "[fileok] : ";
            break;
        case CustomMsgTypes::login:
            std::cout << "[login] : ";
            break;
        case CustomMsgTypes::loginDeny:
            std::cout << "[loginDeny] : ";
            break;
        case CustomMsgTypes::update:
            std::cout << "[update] : ";
            if(mode == 'r') {
                std::cout << "receiving file, the first part of the message: ";
                auto i = msg->body.end();
                for(int j = 0; j < 40 && i != msg->body.begin(); j++) {
                    std::cout << *i;
                    i--;
                }
                std::cout << std::endl;
                return;
            }
            break;
        default:
            std::cout << "[id not found] : ";
            break;
    }
    for(auto i = msg->body.end(); i != msg->body.begin(); --i)
        std::cout << *i;
//    std::string body_str_debug( msg->body.begin(),msg->body.end());
//    std::cout << body_str_debug << std::endl;
    std::cout << std::endl;
    msg_debug_counter++;
}

class CustomServer : public olc::net::server_interface<CustomMsgTypes>
{
public:
    explicit CustomServer(uint16_t nPort) : olc::net::server_interface<CustomMsgTypes>(nPort){};

protected:


    std::map<std::string, std::map<std::string ,fcb >> ft;
    bool OnClientConnect(std::shared_ptr<olc::net::connection<CustomMsgTypes>> client) override
    {
        olc::net::message<CustomMsgTypes> msg;
        msg.header.id = CustomMsgTypes::ServerAccept;
        if(VERBOSE)
            print_message(&msg,'s');
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

        print_message(&msg,'r');

        switch (msg.header.id)
        {


            case CustomMsgTypes::login:
            {
                std::string a, b;
                a = pop_string(&msg);
                b = pop_string(&msg);
                olc::net::message<CustomMsgTypes> msg1;
                std::string pt = PATH_USER;
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
                            if(VERBOSE)
                                print_message(&msg1,'s');
                            client->Send(msg1);
                            c = true;
                        }
                    }
                    myfile.close();
                }

                if (c) {
                    std::map<std::string, fcb> list;
                    ft.insert(std::pair<std::string,std::map<std::string, fcb>> (b, list));
                    pt = PATH_DATABASE + b + "/filetable.txt";
                    cs = new char[pt.length() + 1];
                    strcpy(cs, pt.c_str());
                    myfile.open(cs);
                    std::map<std::string, std::map<std:: string, fcb>>::iterator it;
                    it = ft.find(b);


                    if (myfile.is_open()) {
                        while (getline(myfile, line)) {
                            std::string line2;
                            getline(myfile, line2);
                            fcb gg(line, line2, false);
                            it->second.emplace(std::pair<std::string, fcb> (line,gg));
                        }
                        myfile.close();
                    }

                }

                else{
                    msg1.header.id = CustomMsgTypes::loginDeny;
                    if(VERBOSE)
                        print_message(&msg1,'s');
                    client->Send(msg1);
                }



            }
                break;

            case CustomMsgTypes::final:{
                int n, z = 0;
                std::string us;

                us = pop_string(&msg);
                msg >> n;
                std::map<std::string, std::map<std:: string, fcb>>::iterator it;
                std::map<std:: string, fcb>::iterator it1;
                it = ft.find(us);

                while(z < n){
                    z = 0;
                    for(it1 = it->second.begin(); it1 != it->second.end(); it1++){
                        if(it1->second.ok)
                            z++;
                    }
                  std::this_thread::sleep_for(std::chrono::seconds(2));
                }

                for(it1 = it->second.begin(); it1 != it->second.end(); it1++) {
                    if(!it1->second.ok){
                        std::filesystem::remove(PATH_DATABASE+ us + it1->second.name);
                        it->second.erase(it1->second.name);
                    }

                }
                std::filesystem::remove(PATH_DATABASE + us+ "/filetable.txt");
                std::ofstream ofs1(PATH_DATABASE + us+ "/filetable.txt");
                for(it1 = it->second.begin(); it1 != it->second.end(); it1++)
                    ofs1 << it1->second.name << "\n" << it1->second.md5 << "\n";
                }

                break;

            case CustomMsgTypes::check: {

                std::string user;
                std::string path_client, path_server;
                std::string md;
                olc::net::message<CustomMsgTypes> msg_response;
                user = pop_string(&msg);
                path_client = pop_string(&msg);
                md = pop_string(&msg);

                //TODO: MATTIA PROVA IL CHECK SENZA ITERATOR INIZIO
                //take the map file,md5 related to the user and save it in user_file_table variable
               // std::map<std::string,fcb> user_file_table = ft[user];
                //TODO: MATTIA PROVA IL CHECK SENZA ITERATOR FINE

                std::map<std::string, std::map<std:: string, fcb>>::iterator it;
                std::map<std::string , fcb>::iterator it1;
                it = ft.find(user);
                it1 = it->second.find(path_client); //find the file and save the pair path and md5  in it1
                if (it1 == it->second.end()) { //check if the file is already in the server
                    msg_response.header.id = CustomMsgTypes::update;
                    push_string(&msg_response,path_client);
                    fcb ff = fcb(path_client, md, true);
                    it->second.insert(std::pair<std::string, fcb> (path_client, ff));
                    std::string s(PATH_DATABASE+ user + path_client);
                    std::filesystem::path p = std::filesystem::path(s);
                    std::filesystem::create_directories(p.parent_path());
                    std::ofstream ofs(p);

                    path_server = PATH_DATABASE;
                    path_server.append(user + "/filetable.txt");
                    std::ofstream outfile;
                    outfile.open(path_server, std::ios_base::app); // append instead of overwrite
                    outfile << path_client + "\n" + md + "\n";
                    ofs.close();
                    outfile.close();

                } else {


                    if (it1->second.md5 == md) {
                        msg_response.header.id = CustomMsgTypes::fileok;

                    } else { //file already exist in the server repository but it has a different md5
                        msg_response.header.id = CustomMsgTypes::update;
                        it1->second.md5 = md;
                        std::filesystem::remove(PATH_DATABASE + user + path_client);
                        std::ofstream ofs(PATH_DATABASE + user + path_client);
                        std::filesystem::remove(PATH_DATABASE + user+ "/filetable.txt");
                        std::ofstream ofs1(PATH_DATABASE + user+ "/filetable.txt");


                        for(it1 = it->second.begin(); it1!= it->second.end(); it1++ )
                            ofs1 << it1->second.name << "\n" << it1->second.md5 << "\n";
                        //OLD VERSION:
//                        for(it1 = it->second.begin(); it1 != it->second.end(); ++it)
//                            ofs1 << it1->first << "\n" << it1->second << "\n";
                        push_string(&msg_response, path_client); //TODO : pt o era path_server?
                        ofs.close();
                        ofs1.close();
                    }
                    it1->second.ok = true;
                }
                if(VERBOSE)
                    print_message(&msg_response, 's');
                client->Send(msg_response);
            }
                break;

            case CustomMsgTypes::update: {
                std::string pt, pt1;
                std::string user;
                user = pop_string(&msg);
                pt = pop_string(&msg);
                pt1 = pt;
                int bytes, nblocco;
                pt = std::filesystem::path(PATH_DATABASE+ user + pt);

                std::filesystem::create_directories(pt.substr(0, pt.find_last_of('/')));
                std::ofstream ofs;
                ofs.open(pt, std::ios_base::app);
                unsigned char data[8192];
                msg >> bytes;
                msg >> nblocco;

                auto it = ft.find(user);
                auto it1 = it->second.find(pt1);
                if(VERBOSE) {
                    std::cout << "[update log] : " << " received data of " << pt << std::endl;
                    std::cout << "\t block number " << nblocco << "\t data waiting for block number " << it1->second.bloccoatteso << std::endl;
                }
                std::unique_lock<std::mutex> lk (it1->second.m) ;

                while(nblocco != it1->second.bloccoatteso) {
                    if(VERBOSE)
                        std::cout << "[update log] : " << "waiting block number " << it1->second.bloccoatteso << std::endl;
                    it1->second.cv.wait(lk);
                }


                if(bytes == 8192){
                    msg >> data;
                    ofs << data;
                }
                else{
                    msg >> data;
                    ofs.write( (char*)data, bytes );
                }

                ofs.close();
                it1->second.updatebloccoatteso();
                lk.unlock();
                it1->second.cv.notify_all();
            }

                break;

            case CustomMsgTypes::erase:{
                std::string pt;
                std::string user;
                pt = pop_string(&msg);
                user = pop_string(&msg);
                std::map<std::string, fcb>::iterator it1;
                std::map<std::string, std::map<std::string, fcb>>::iterator it;
                it = ft.find(user);
                it1 = it->second.find(pt);
                if(it!= ft.end() && !it1->first.empty() ) {
                    it->second.erase(it1);
                    std::filesystem::remove(PATH_DATABASE+ user + pt);
                    std::filesystem::remove(PATH_DATABASE + user+ "/filetable.txt");
                    std::ofstream ofs1(PATH_DATABASE + user+ "/filetable.txt");
                    for(it1 = it->second.begin(); it1 != it->second.end(); it1++)
                        ofs1 << it1->second.name << "\n" << it1->second.md5 << "\n";
                }
            }
                break;

            case CustomMsgTypes::disconnetti:{
                std::string user;
                user = pop_string(&msg);
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                ft.erase(user);
                client->Disconnect();
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

CustomServer server(60000);

void lanciathread(){

    while (1)
    {
        server.Update(-1, true);
    }

}




int main()
{

    server.Start();
    std::vector<std::thread> th;
    std::vector<std::thread> t(N_MAX_THREAD);

for(int i = 0; i < N_MAX_THREAD; i++){
    t[i] = std::thread(lanciathread);
    }

    while (1)
    {
        server.Update(-1, true);
    }



    return 0;
}