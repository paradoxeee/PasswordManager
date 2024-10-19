#ifndef _PASSWORDMANAGER_H
#define _PASSWORDMANAGER_H

#include <string>
#include <map>

class PasswordManager
{
public:
    void add(const std::string &plateform, const std::string &username, const std::string &password, const std::string &a2f);
    void remove(const std::string &plateform);
    bool exists(const std::string &plateform);
    bool existsPassword(const std::string &password);
    void print();
    void printRemove();
    void printFlemme();
    void loadFromFile(const std::string &filename);
    void deleteAll();
    void saveUserPassword(const std::string &password);
    bool verifyUserPassword(const std::string &enteredPassword);
    bool isPasswordValid(const std::string &password);
    void passwordGenerate();
    

        ~PasswordManager();

private:
    std::string userKey;
    std::map<std::string, std::tuple<std::string, std::string, std::string>> maps;
    std::map<std::string, int> passwordUsage;
    std::string generatePassword(int length);
};

#endif