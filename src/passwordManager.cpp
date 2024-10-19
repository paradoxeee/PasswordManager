#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include "passwordManager.hpp"
#include "AES.hpp"

void PasswordManager::add(const std::string &platform, const std::string &username, const std::string &password, const std::string &a2f)
{
    auto it = maps.find(platform);
    if (it != maps.end())
    {
        const auto &info = it->second;
        const std::string &usernameVerif = std::get<0>(info);

        if (usernameVerif == username)
        {
            std::cerr << "Error: Username already exists for platform '" << platform << "'. Please choose another." << std::endl;
            return;
        }
    }

    if (passwordUsage[password] >= 2)
    {
        std::cerr << "Error: This password has already been used twice. It's useless to use a password manager to use the same one everywhere :)" << std::endl;
        return;
    }

    std::string message = platform + " " + username + " " + password + " " + a2f;
    EasyAES aes;
    std::string cipher = aes.encrypt(message, userKey);

    if (!isPasswordValid(password))
    {
        std::cerr << "The password is not secure enough. Please try again." << std::endl;
        return;
    }
    maps[platform] = std::make_tuple(username, password, a2f);
    passwordUsage[password]++;

    std::ofstream file("password.txt", std::ios::app);
    if (file)
    {
        file << cipher << std::endl;
        file.close();
    }
    else
    {
        std::cerr << "Error opening the file" << std::endl;
    }
}

void PasswordManager::print()
{
    std::string platform;
    std::cout << "Enter the name of the platform for which you want to display information: ";
    std::cin >> platform;

    auto it = maps.find(platform);
    if (it != maps.end())
    {
        const auto &info = it->second;
        const std::string &username = std::get<0>(info);
        const std::string &password = std::get<1>(info);
        const std::string &a2fVerif = std::get<2>(info);

        std::string a2fTest;
        std::ifstream keyFile("key.txt");

        std::cout << "Enter your 2FA key for platform " << platform << ": ";
        std::cin >> a2fTest;
        std::cout << "\n";

        if (a2fVerif == a2fTest)
        {
            std::cout << "Platform: " << platform << std::endl;
            std::cout << "Username: " << username << std::endl;
            std::cout << "Password: " << password << std::endl;
            std::cout << "2FA: " << a2fVerif << std::endl;
            std::cout << "--------------------------------------" << std::endl;
        }
        else
        {
            std::cerr << "Error: Incorrect 2FA key." << std::endl;
            return;
        }
    }
    else
    {
        std::cerr << "No information found for platform '" << platform << "'." << std::endl;
        return;
    }
}

void PasswordManager::printRemove()
{
    int index = 1;
    for (const auto &entry : maps)
    {
        const std::string &platform = entry.first;
        std::cout << index++ << "." << " " << platform << std::endl;
    }
}

void PasswordManager::loadFromFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "The file does not exist / Error opening the file: " << filename << std::endl;
        return;
    }

    std::string line;
    EasyAES aes;
    while (std::getline(file, line))
    {
        try
        {
            std::string decryptedLine = aes.decrypt(line, userKey);

            std::istringstream iss(decryptedLine);
            std::string platform, username, password, a2f;
            if (iss >> platform >> username >> password >> a2f)
            {
                maps[platform] = std::make_tuple(username, password, a2f);
                passwordUsage[password]++;
            }
            else
            {
                std::cerr << "Format error on line: " << decryptedLine << std::endl;
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error during decryption: " << e.what() << std::endl;
        }
    }
}

PasswordManager::~PasswordManager()
{
}

void PasswordManager::remove(const std::string &platform)
{
    auto it = maps.find(platform);
    if (it != maps.end())
    {
        const auto &info = it->second;
        const std::string password = std::get<1>(info);

        maps.erase(platform);

        if (passwordUsage[password] > 0)
        {
            passwordUsage[password]--;
        }

        if (passwordUsage[password] == 0)
        {
            passwordUsage.erase(password);
        }

        std::ofstream file("password.txt");
        if (file)
        {
            for (const auto &entry : maps)
            {
                const std::string &platform = entry.first;
                const auto &info = entry.second;

                const std::string &username = std::get<0>(info);
                const std::string &password = std::get<1>(info);
                const std::string &a2f = std::get<2>(info);

                file << platform << " " << username << " " << password << " " << a2f << std::endl;
            }
            file.close();
        }
        else
        {
            std::cerr << "Error opening the file for update." << std::endl;
        }
        std::cout << "\n";
        std::cout << "Platform '" << platform << "' has been deleted." << std::endl;
    }
    else
    {
        std::cout << "Platform '" << platform << "' does not exist." << std::endl;
    }
}

void PasswordManager::deleteAll()
{
    std::string sure;
    std::cout << "Are you sure you want to delete everything? [Y/N] ";
    std::getline(std::cin, sure);

    if (sure == "Y" || sure == "Yes")
    {
        maps.clear();
        passwordUsage.clear();

        std::ofstream file("password.txt", std::ios::trunc);
        if (file)
        {
            file.close();
            std::cout << "All data has been deleted." << std::endl;
        }
        else
        {
            std::cerr << "Error opening the file." << std::endl;
        }
    }
    else
    {
        std::cout << "No data has been deleted." << std::endl;
    }
}

void PasswordManager::saveUserPassword(const std::string &password)
{
    EasyAES aes;
    std::string encryptedPassword = aes.encrypt(password, userKey);
    std::ofstream keyFile("key.txt");
    if (keyFile)
    {
        keyFile << encryptedPassword;
        keyFile.close();
    }
    else
    {
        std::cerr << "Error opening key.txt" << std::endl;
    }
}

bool PasswordManager::verifyUserPassword(const std::string &enteredPassword)
{
    EasyAES aes;
    std::ifstream keyFile("key.txt");
    std::string encryptedPassword;

    if (keyFile)
    {
        std::getline(keyFile, encryptedPassword);
        keyFile.close();

        std::string decryptedPassword = aes.decrypt(encryptedPassword, userKey);
        return enteredPassword == decryptedPassword;
    }
    return false;
}

bool PasswordManager::exists(const std::string &platform)
{
    return maps.find(platform) != maps.end();
}

std::string PasswordManager::generatePassword(int length)
{
    static const char alphanum[] =
        "0123456789"
        "!@#$%^&*"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    int size = sizeof(alphanum) - 1;
    std::string password;

    for (int i = 0; i < length; i++)
    {
        password += alphanum[rand() % size];
    }

    return password;
}

bool PasswordManager::isPasswordValid(const std::string &password)
{
    if (password.length() < 8)
    {
        std::cerr << "Error: The password must contain at least 8 characters." << std::endl;
        return false;
    }

    bool hasSpecialChar = false;
    bool hasDigit = false;
    bool hasLowercase = false;
    bool hasUppercase = false;

    for (char ch : password)
    {
        if (std::isdigit(ch))
        {
            hasDigit = true;
        }
        else if (std::islower(ch))
        {
            hasLowercase = true;
        }
        else if (std::isupper(ch))
        {
            hasUppercase = true;
        }
        else if (std::ispunct(ch))
        {
            hasSpecialChar = true;
        }
    }

    if (!hasSpecialChar)
    {
        std::cerr << "Error: The password must contain at least one special character." << std::endl;
        return false;
    }
    if (!hasDigit)
    {
        std::cerr << "Error: The password must contain at least one digit." << std::endl;
        return false;
    }
    if (!hasLowercase)
    {
        std::cerr << "Error: The password must contain at least one lowercase letter." << std::endl;
        return false;
    }
    if (!hasUppercase)
    {
        std::cerr << "Error: The password must contain at least one uppercase letter." << std::endl;
        return false;
    }

    return true;
}
void PasswordManager::printFlemme()
{
    for (const auto &entry : maps)
    {
        const std::string &plateform = entry.first;
        const auto info = entry.second;


        std::cout << "Plateforme: " << plateform << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        std::cout << "\n";
    }
}

void PasswordManager::passwordGenerate()
{
    srand(static_cast<unsigned int>(time(0))); 
    int length = 8;                            
    std::string password;

    do
    {
        password = generatePassword(length); 
        std::cout << "Password generate : " << password << std::endl;
    } while (!isPasswordValid(password)); 

    std::cout << "Password not valid : " << password << std::endl;
}

