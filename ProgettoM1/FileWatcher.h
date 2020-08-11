#ifndef TESTINGBOOSTLIB_FILEWATCHER_H
#define TESTINGBOOSTLIB_FILEWATCHER_H

#include <filesystem>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <string>
#include <functional>

enum  class  FileStatus{created, modified, erased};

class FileWatcher{
public:
    std::string path_to_watch;
    std::chrono::duration<int, std::milli> delay;
    FileWatcher(const std::string& path_to_watch, std::chrono::duration<int, std::milli> delay) : path_to_watch{path_to_watch}, delay{delay} {
        for(auto &file : std::filesystem::recursive_directory_iterator(path_to_watch)) {
            paths_[file.path().string()] = std::filesystem::last_write_time(file);
        }
    }

    void start(const std::function<void (std::string, FileStatus)> &action);

private:
    std::unordered_map<std::string, std::filesystem::file_time_type> paths_;
    bool running_ = true;
    bool contains(const std::string &key){return paths_.contains(key);}
};


#endif //TESTINGBOOSTLIB_FILEWATCHER_H
