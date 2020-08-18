//
// Created by Mattia Riola on 18/08/2020.
//

#ifndef TESTINGBOOSTLIB_USER_H
#define TESTINGBOOSTLIB_USER_H


#include <string>

namespace sfs = std::filesystem;

class User {


private:
    std::string rootName;
    int id;
    std::string userName;
    std::string password;

public:
    User(int id, const std::string &userName, const std::string &password);
    /**
     * The destructor will clean the backup removing all the file/directories
     * from the root of that user
     */
    virtual ~User();
    /**
     * It will remove everything from the root directory
     */
    void cleanBackup();

    const std::string &getRootName() const;

    void setRootName(const std::string &rootName);

    int getId() const;

    void setId(int id);

    const std::string &getUserName() const;

    void setUserName(const std::string &userName);

    const std::string &getPassword() const;

    void setPassword(const std::string &password);
};


#endif //TESTINGBOOSTLIB_USER_H
