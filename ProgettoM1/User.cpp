//
// Created by Mattia Riola on 18/08/2020.
//

#include <iostream>
#include <filesystem>
#include "User.h"

#define TESTUSER true

User::User(int id, const std::string &userName, const std::string &password) : id(id), userName(userName), password(password) {
    if(TESTUSER)
        std::cout << "User "<< id << "created: " << std::endl << userName << std::endl << password << std::endl;
}

int User::getId() const {
    return id;
}

void User::setId(int id) {
    User::id = id;
}

const std::string &User::getUserName() const {
    return userName;
}

void User::setUserName(const std::string &userName) {
    User::userName = userName;
}

const std::string &User::getPassword() const {
    return password;
}

void User::setPassword(const std::string &password) {
    User::password = password;
}


const std::string &User::getRootName() const {
    return rootName;
}

void User::setRootName(const std::string &rootName) {
    User::rootName = rootName;
}
void User::cleanBackup(){
    sfs::remove_all(rootName);
}
User::~User() {
    //    RIMUOVERE TUTTO IL BACK UP NEL DISTRUTTORE DELL'UTENTE
    cleanBackup();
}
