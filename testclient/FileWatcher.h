
#ifndef TESTCLIENT_FILEWATCHER_H
#define TESTCLIENT_FILEWATCHER_H
#pragma once
#include <filesystem>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <string>
#include <functional>
#include <sys/types.h>
#include "md5.h"
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <boost/uuid/detail/md5.hpp>
#include <map>
#include <fstream>
#include <boost/algorithm/hex.hpp>
// Define available file changes
enum class FileStatus {created, modified, erased};

class FileWatcher {
public:
    std::string path_to_watch;
    // Time interval at which we check the base folder for changes
    std::chrono::duration<int, std::milli> delay;
    // Keep a record of files from the base directory and their last modification time
    FileWatcher(const std::string& path_to_watch, std::chrono::duration<int, std::milli> delay) : path_to_watch{path_to_watch}, delay{delay} {
        for(auto &file : std::filesystem::recursive_directory_iterator(path_to_watch)) {
            path[file.path().string()] = md5(file.path().string(), file.file_size());
        }
    }
    // ...
    void start(const std::function<void (std::string, FileStatus)> &action) {
        while(running_) {
            // Wait for "delay" milliseconds
            std::this_thread::sleep_for(delay);
            auto it = path.begin();
            while (it != path.end()) {
                if (!std::filesystem::exists(it->first)) {
                    action(it->first, FileStatus::erased);
                    it = path.erase(it);
                }
                else {
                    it++;
                }
            }
            // Check if a file was created or modified
            for(auto &file : std::filesystem::recursive_directory_iterator(path_to_watch)) {

                // File creation
                if(!contains(file.path().string())) {
                    path[file.path().string()] = md5(file.path(), file.file_size());
                    action(file.path().string(), FileStatus::created);
                    // File modification
                } else {
                    auto current_file_md5 = md5(file.path(), file.file_size());
                    if(path[file.path().string()] != current_file_md5) {
                        path[file.path().string()] = current_file_md5;
                        action(file.path().string(), FileStatus::modified);
                    }
                }
            }
        }
    }
private:
    std::unordered_map<std::string ,std::string> path;
    bool running_ = true;
    // Check if "paths_" contains a given key
    // If your compiler supports C++20 use paths_.contains(key) instead of this function
    bool contains(const std::string &key) {
        auto el = path.find(key);
        return el != path.end();
    }


};
#endif //TESTCLIENT_FILEWATCHER_H
