#include <iostream>
#include "olc_net.h"
#include <boost/thread/thread.hpp>
#include <openssl/md5.h>
#include "FileWatcher.h"

#define PATH_CLIENT "../clientdirectory" //Define del path del client
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 60000
#define MAX_TRY 10
#define VERBOSE true

int msg_debug_counter = 0;
int n = 999999999, p, q;
std::string pathr(PATH_CLIENT);

/**
 * Print what is inside the msg
 * !warning! if there is a string, before the string an int is store in the message and this will print the int as 4 char
 * @param msg pointer of the message
 * @param mode 'r' if the msg is received, 's' if the message is sent
 */
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
            break;
        default:
            std::cout << "[id not found] : ";
            break;
    }
    for(auto i = msg->body.end(); i != msg->body.begin(); --i)
        std::cout << *i;
    std::cout << std::endl;
    msg_debug_counter++;
}



class CustomClient : public olc::net::client_interface<CustomMsgTypes>
{
public:
    bool log = false;
    std::string user;

    void login(const std::string& username, const std::string& password)
    {
        olc::net::message<CustomMsgTypes> msg1;
        msg1.header.id = CustomMsgTypes::login;
        push_string(&msg1, username);
        push_string(&msg1, password);
        if(VERBOSE)
            print_message(&msg1,'s');
        Send(msg1);
    }

    void update(olc::net::message<CustomMsgTypes>* msg){
        std::string path, path1;
        path = pop_string(msg);
        path1 = pathr + path ;
        int pkg_counter;
        char *cs = new char[path1.length() + 1];
        strcpy(cs, path1.c_str());
        FILE *inFile = fopen (cs, "rb");
        int bytes;
        unsigned char data[8192];

        if (inFile == nullptr) {
            printf ("%s can't be opened.\n", cs);
            return;
        }
        pkg_counter = 1;
        while ((bytes = fread (data, 1, 8192, inFile)) != 0){
            olc::net::message<CustomMsgTypes> msg1;
            msg1.header.id = CustomMsgTypes::update;
            msg1 << data;
            msg1 << bytes;
            push_string(&msg1,path);
            push_string(&msg1,user);
            if(VERBOSE)
                std::cout << "[update] : " << user <<  std::endl
                << path1 << " -> sending package number : " << pkg_counter << std::endl;
            Send(msg1);
            pkg_counter++;
        }
        pkg_counter = 1;
        fclose (inFile);
        q++;
    }
};

void sendallfcb(CustomClient* c){
    p = 0;
    for(auto &file : std::filesystem::recursive_directory_iterator(pathr)) {
        if(std::filesystem::is_regular_file(std::filesystem::path(file))) {
            p++;
            std::string md = md5(file.path().string());
            olc::net::message<CustomMsgTypes> fcb;
            fcb.header.id = CustomMsgTypes::check;
            push_string(&fcb,md);
            std::string s;
            s = file.path().string().erase(0, pathr.length()); // s = filename
            push_string(&fcb,s);
            push_string(&fcb,c->user);
            if(VERBOSE)
                print_message(&fcb,'s');
            c->Send(fcb);
        }
    }
    n = p;
}


void attesa(CustomClient* c){
    while(c->IsConnected()){
        while (c->Incoming().empty()) c->Incoming().wait();
        auto msg = c->Incoming().pop_front().msg;
        if(VERBOSE)
            print_message(&msg,'r');
        if(msg.header.id == CustomMsgTypes::update){
            c->update(&msg);
        }
    }
}



int main()
{
    CustomClient c;
    int n_try = 0;
    while(!c.IsConnected() && n_try < MAX_TRY) {
        //TODO: tentativo di riconnessione non funzionante perche' isConnected e' true anche se la connessione non e' avvenuta
        std::cout << "Trying to connect to " << SERVER_IP << ":" << SERVER_PORT << std::endl;
        c.Connect(SERVER_IP, SERVER_PORT);
        std::this_thread::sleep_for(std::chrono::seconds(2));
        n_try++;
    }
    if(n_try==MAX_TRY && !c.IsConnected()) {
        std::cerr << "Server not found\nquitting..." << std::endl;
        return 2;
    }
    while (c.Incoming().empty())
        c.Incoming().wait();

    auto msg = c.Incoming().pop_front().msg;
    if(VERBOSE)
        print_message(&msg,'r');
    if(msg.header.id == CustomMsgTypes::ServerAccept) {
        std::cout << "Connessione al server avvenuta con succcesso!!\n";
    }
    else{
        std::cout << "Connesione al server non riuscita\n";
    }
        if (c.IsConnected())
        {

            std::cout <<"inserisci i dati per il login\n";
            std::string us, ps;
            while(!c.log){
                std::cout <<"USER:";
                getline(std::cin, us);
                std::cout <<"PASSWORD:";
                getline(std::cin, ps);
                c.login(us, ps);
                while (c.Incoming().empty()) c.Incoming().wait();
                msg = c.Incoming().pop_front().msg;
                if(VERBOSE)
                    print_message(&msg,'r');
                if(msg.header.id == CustomMsgTypes::login){
                    c.log = true;
                    c.user = us;
                    std::cout <<"Login avvenuto con successo\n";
                } else
                    std::cout <<"Credenziali errate, riprova...\n";
            }



            std::thread t1(sendallfcb, &c);



            q = 0;
            while(q < n){
                while (c.Incoming().empty()) c.Incoming().wait();
                msg = c.Incoming().pop_front().msg;
                if(VERBOSE)
                    print_message(&msg,'r');
                if(msg.header.id == CustomMsgTypes::update)
                    c.update(&msg);
                else
                    if(msg.header.id == CustomMsgTypes::fileok)
                        q++;
            }

            t1.join();
            std::thread t2(attesa, &c);
            FileWatcher fw{PATH_CLIENT, std::chrono::milliseconds(3000)};
            // Start monitoring a folder for changes and (in case of changes)
            // run a user provided lambda function

            fw.start([&c] (const std::string& path_to_watch, FileStatus status) -> void {
                // Process only regular files, all other file types are ignored
                if(!std::filesystem::is_regular_file(std::filesystem::path(path_to_watch)) && status != FileStatus::erased) {
                    return;
                }

                if(status == FileStatus::modified || status == FileStatus::created ){
                    std::filesystem::directory_entry pt(path_to_watch);
                    std::string md = md5(path_to_watch);
                    olc::net::message<CustomMsgTypes> fcb;
                    fcb.header.id =  CustomMsgTypes::check;
                    push_string(&fcb,md);
                    std::string s;
                    s = pt.path().string().erase(0, pathr.length());
                    push_string(&fcb,s);
                    push_string(&fcb,c.user);
                    if(VERBOSE)
                        print_message(&fcb,'s');
                    c.Send(fcb);
                }
                else{
                    if(status == FileStatus::erased){
                        std::filesystem::directory_entry pt(path_to_watch);
                        olc::net::message<CustomMsgTypes> fcb;
                        fcb.header.id =  CustomMsgTypes::erase;
                        std::string s;
                        s = pt.path().string().erase(0, pathr.length());
                        push_string(&fcb,c.user);
                        push_string(&fcb,s);
                        if(VERBOSE)
                            print_message(&fcb,'s');
                        c.Send(fcb);
                    }
                    else
                        std::cout << "Error! Unknown file status.\n";
                }
            });


        }


        return 0;

    }






