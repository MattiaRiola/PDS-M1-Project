#include <string>
#include <iostream>
#include <boost/filesystem.hpp>
#include "FileWatcher.h"

using namespace std;
using namespace boost::filesystem;

int main() {
    FileWatcher fw{R"("inserisci path")", std::chrono::milliseconds(5000)};
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