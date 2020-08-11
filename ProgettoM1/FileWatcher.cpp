//
// Created by samu on 11/08/2020.
//

#include "FileWatcher.h"

void FileWatcher::start(const std::function<void(std::string, FileStatus)> &action) {
    while(running_) {
        std::this_thread::sleep_for(delay);
        auto it = paths_.begin();
        while (it != paths_.end()) {
            if (!std::filesystem::exists(it->first)) {
                action(it->first, FileStatus::erased);
                it = paths_.erase(it);
            }
            else {
                it++;
            }
        }


        for(auto &file : std::filesystem::recursive_directory_iterator(path_to_watch)) {
            auto current_file_last_write_time = std::filesystem::last_write_time(file);
            if(!contains(file.path().string())) {
                paths_[file.path().string()] = current_file_last_write_time;
                action(file.path().string(), FileStatus::created);

            } else {
                if(paths_[file.path().string()] != current_file_last_write_time) {
                    paths_[file.path().string()] = current_file_last_write_time;
                    action(file.path().string(), FileStatus::modified);
                }
            }
        }
    }
}



