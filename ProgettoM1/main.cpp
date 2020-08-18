#include <iostream>
#include <boost/filesystem.hpp>
#include "FileWatcher.h"
#include "User.h"

#define TEST1 true

namespace bfs = boost::filesystem;
namespace sfs = std::filesystem;
void init_backup_folder(User usr);
void fileWatcher_start(std::string p);
int main() {
    std::cout << "Hello, World!" << std::endl;
    User u(0,"User0","password0");
    init_backup_folder(u);
    //TODO: Non mi trova il path rootName
    //fileWatcher_start(u.getRootName());

    std::cout << "ByeBye!" << std::endl;
    return 0;
}

void init_backup_folder(User usr){
//    std::cout << "This is my current directory: " << sfs::current_path() << std::endl;
    std::string root_name = "Backup" + usr.getUserName();
    usr.setRootName(root_name);
    std::cout << usr.getRootName() << std::endl;
    sfs::create_directories(usr.getRootName());
    if(TEST1) //Creo un file di testo a scopo di debug
        std::ofstream(usr.getRootName() + "/README.txt");
    if(TEST1) //Stampo tutto l'albero delle directory a partire dalla root del backup
        for(auto& p: sfs::recursive_directory_iterator(usr.getRootName())){
            std::cout << p.path() << std::endl;
        }

}

void fileWatcher_start(std::string p){
    //TODO: Non mi trova il path p passandoglielo come parametro
    FileWatcher  fw{R"("inserisci path")", std::chrono::milliseconds(5000)};
    fw.start([] (const std::string& path_to_watch, FileStatus status) -> void {
        //non sono sicuro serva a qualcosa
        if(!std::filesystem::is_regular_file(std::filesystem::path(path_to_watch))&& !std::filesystem::is_directory(std::filesystem::path(path_to_watch))) {
            return;
        }

        switch(status) {
            case FileStatus::created:
                std::cout << "File created: " << path_to_watch << '\n';
                break;
            case FileStatus::modified:
                std::cout << "File modified: " << path_to_watch << '\n';
                break;
            case FileStatus::erased:
                std::cout << "File erased: " << path_to_watch << '\n';
                break;
            default:
                std::cout << "Error! Unknown file status.\n";
        }
    });
}
