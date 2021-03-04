#include <iostream>
#include "olc_net.h"
#include <boost/thread/thread.hpp>
#include "FileWatcher.h"

int n = 999999999, p, q;
std::string pathr("/Users/samuelejakupi/Desktop/directoryclient");

class CustomClient : public olc::net::client_interface<CustomMsgTypes>
{
public:
    bool log = false;
    std::string user;

    void login(const std::string& a, const std::string& b)
    {
        olc::net::message<CustomMsgTypes> msg1;
        msg1.header.id = CustomMsgTypes::login;
        msg1 << a;
        msg1 << b;
        Send(msg1);
    }

    void update(olc::net::message<CustomMsgTypes> msg){
        std::string path, path1;
        msg >> path;
        path1 = pathr + path ;
        char *cs = new char[path1.length() + 1];
        strcpy(cs, path1.c_str());
        FILE *inFile = fopen (cs, "rb");
        int bytes;
        unsigned char data[8192];

        if (inFile == nullptr) {
            printf ("%s can't be opened.\n", cs);
            return;
        }

        while ((bytes = fread (data, 1, 8192, inFile)) != 0){
            olc::net::message<CustomMsgTypes> msg1;
            msg1.header.id = CustomMsgTypes::update;
            msg1 << data;
            msg1 << bytes;
            msg1 << path;
            msg1 << user;
            Send(msg1);
        }
        fclose (inFile);
        q++;
    }
};

void sendallfcb(CustomClient* c){
    p = 0;
    for(auto &file : std::filesystem::recursive_directory_iterator(pathr)) {
        p++;
        std::string md = md5(file.path().string());
        olc::net::message<CustomMsgTypes> fcb;
        fcb.header.id =  CustomMsgTypes::check;
        fcb << md;
        std::string s;
        s = file.path().string().erase(0, pathr.length());
        fcb << s;
        fcb << c->user;
        c->Send(fcb);
    }
    n = p;
}



void attesa(CustomClient* c){
    while(c->IsConnected()){
        while (c->Incoming().empty()) c->Incoming().wait();
        auto msg = c->Incoming().pop_front().msg;
        if(msg.header.id == CustomMsgTypes::update){
            c->update(msg);
        }
    }
}



int main()
{
    CustomClient c;
    int nx = 0, nmax = 10;
    while(nx < nmax && !c.IsConnected()) {
        c.Connect("127.0.0.1", 60000 + nx);
        nx++;
    }

    while (c.Incoming().empty()) c.Incoming().wait();
    auto msg = c.Incoming().pop_front().msg;

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
                if(msg.header.id == CustomMsgTypes::login){
                    c.log = true;
                    c.user = us;
                    std::cout <<"Login avvenuto con successo\n";
                } else
                    std::cout <<"Credenziali errate, riprova...\n";
            }



            std::thread t1(sendallfcb, &c);



            q = 0;
            while(q < nx){
                while (c.Incoming().empty()) c.Incoming().wait();
                msg = c.Incoming().pop_front().msg;
                if(msg.header.id == CustomMsgTypes::update)
                    c.update(msg);
                else
                    if(msg.header.id == CustomMsgTypes::fileok)
                        q++;


            }

            t1.join();
            std::thread t2(attesa, &c);
            FileWatcher fw{"/Users/samuelejakupi/Desktop/directoryclient", std::chrono::milliseconds(3000)};
            // Start monitoring a folder for changes and (in case of changes)
            // run a user provided lambda function

            fw.start([&c] (const std::string& path_to_watch, FileStatus status) -> void {
                // Process only regular files, all other file types are ignored
                if(!std::filesystem::is_regular_file(std::filesystem::path(path_to_watch)) && status != FileStatus::erased) {
                    return;
                }

                if(status == FileStatus::modified || status ==FileStatus::created ){
                    std::filesystem::directory_entry pt(path_to_watch);
                    std::string md = md5(path_to_watch);
                    olc::net::message<CustomMsgTypes> fcb;
                    fcb.header.id =  CustomMsgTypes::check;
                    fcb << md;
                    std::string s;
                    s = pt.path().string().erase(0, pathr.length());
                    fcb << s;
                    fcb << c.user;
                    c.Send(fcb);
                }
                else{
                    if(status == FileStatus::erased){
                        std::filesystem::directory_entry pt(path_to_watch);
                        olc::net::message<CustomMsgTypes> fcb;
                        fcb.header.id =  CustomMsgTypes::erase;
                        std::string s;
                        s = pt.path().string().erase(0, pathr.length());
                        fcb << s;
                        fcb << c.user;
                        c.Send(fcb);
                    }
                    else
                        std::cout << "Error! Unknown file status.\n";
                }
            });


        }


        return 0;

    }






