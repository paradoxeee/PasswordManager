#include <iostream>
#include <string>
#include <cassert>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "passwordManager.hpp"
#include "AES.hpp"

void ui()
{
    PasswordManager passwordManager;

    std::ifstream keyFile("key.txt");
    if (keyFile)
    {
        std::string userKey;
        std::cout << "Enter your password to access the manager: ";
        std::getline(std::cin, userKey);
        system("cls");
        system("clear");

        if (!passwordManager.verifyUserPassword(userKey))
        {
            std::cerr << "Incorrect password. Access denied." << std::endl;
            std::cout << R"(
                          oooo$$$$$$$$$$$$oooo
                      oo$$$$$$$$$$$$$$$$$$$$$$$$o
                   oo$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$o         o$   $$ o$
   o $ oo        o$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$o       $$ $$ $$o$
oo $ $ "$      o$$$$$$$$$    $$$$$$$$$$$$$    $$$$$$$$$o       $$$o$$o$
"$$$$$$o$     o$$$$$$$$$      $$$$$$$$$$$      $$$$$$$$$$o    $$$$$$$$
  $$$$$$$    $$$$$$$$$$$      $$$$$$$$$$$      $$$$$$$$$$$$$$$$$$$$$$$
  $$$$$$$$$$$$$$$$$$$$$$$    $$$$$$$$$$$$$    $$$$$$$$$$$$$$  """$$$
   "$$$""""$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$     "$$$
    $$$   o$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$     "$$$o
   o$$"   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$       $$$o
   $$$    $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" "$$$$$$ooooo$$$$o
  o$$$oooo$$$$$  $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$   o$$$$$$$$$$$$$$$$$
  $$$$$$$$"$$$$   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$     $$$$""""""""
 """"       $$$$    "$$$$$$$$$$$$$$$$$$$$$$$$$$$$"      o$$$
            "$$$o     """$$$$$$$$$$$$$$$$$$"$$"         $$$
              $$$o          "$$""$$$$$$""""           o$$$
               $$$$o                 oo             o$$$"
                "$$$$o      o$$$$$$o"$$$$o        o$$$$
                  "$$$$$oo     ""$$$$o$$$$$o   o$$$$""  
                     ""$$$$$oooo  "$$$o$$$$$$$$$"""
                        ""$$$$$$$oo $$$$$$$$$$       
                                """"$$$$$$$$$$$        
                                    $$$$$$$$$$$$       
                                     $$$$$$$$$$"      
                                      "$$$""""

)" << '\n';
            return;
        }

        passwordManager.loadFromFile("password.txt");
    }
    else
    {
        std::remove("password.txt");
        std::cerr << "Security: Do not try to bypass. All passwords have been deleted." << std::endl;

        std::string newPassword;
        std::cout << "No password found. Create a new password: ";
        std::getline(std::cin, newPassword);
        passwordManager.saveUserPassword(newPassword);
        std::cout << "Password saved successfully." << std::endl;
    }

    std::string command;
    std::cout << R"(
    ____                 _       __               ____  ___                                 
   / __ \____ __________| |     / ____  _________/ /  |/  ____ _____  ____ _____ ____  _____
  / /_/ / __ `/ ___/ ___| | /| / / __ \/ ___/ __  / /|_/ / __ `/ __ \/ __ `/ __ `/ _ \/ ___/
 / ____/ /_/ (__  (__  )| |/ |/ / /_/ / /  / /_/ / /  / / /_/ / / / / /_/ / /_/ /  __/ /    
/_/    \__,_/____/____/ |__/|__/\____/_/   \__,_/_/  /_/\__,_/_/ /_/\__,_/\__, /\___/_/     
                                                                         /____/             
_______________________________________________________________________________________________       
)" << '\n';

    while (true)
    {
        std::cout << R"(
+------------------------------------------------------------------------------------+
|                                Available Options                                   |
+------------------------------------------------------------------------------------+
| a : Add                                                                            |
| d : Delete all your data                                                           |
| l : Display your data. [Warning: Your platform key will be required]               |
| r : Remove.                                                                        |
| q : Quit. [That’s a shame]                                                         |
| c : Clear terminal content                                                         |
| g : Generate a password.                                                           |
+------------------------------------------------------------------------------------+

)" << '\n';
        std::cout << "> ";
        std::getline(std::cin, command);

        if (command == "a")
        {
            std::string platform, username, password, a2f;
            std::cout << "Please enter the platform name (Google, Discord...): ";
            std::getline(std::cin, platform);
            std::cout << "Please enter your username: ";
            std::getline(std::cin, username);
            std::cout << "Please enter your password: ";
            std::getline(std::cin, password);
            std::cout << "Please enter your 2FA key: ";
            std::getline(std::cin, a2f);
            passwordManager.add(platform, username, password, a2f);
        }
        else if (command == "l")
        {
            std::cout << "\n";
            passwordManager.printFlemme();
            passwordManager.print();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        else if (command == "q")
        {
            system("clear");
            break;
        }
        else if (command == "r")
        {
            std::string platform;
            std::cout << "Available platforms: " << std::endl;
            std::cout << "\n";
            passwordManager.printRemove();
            std::cout << "\n";
            std::cout << "Please enter the name you wish to delete: ";
            std::getline(std::cin, platform);
            passwordManager.remove(platform);
        }
        else if (command == "d")
        {
            if (keyFile)
            {
                std::string userKey;
                std::cout << "Enter your password to delete the data: ";
                std::getline(std::cin, userKey);
                passwordManager.deleteAll();
                system("cls");
                system("clear");

                if (!passwordManager.verifyUserPassword(userKey))
                {
                    std::cerr << "Incorrect password. YOU WILL DELETE NOTHING." << std::endl;
                    std::cout << R"(
                          oooo$$$$$$$$$$$$oooo
                      oo$$$$$$$$$$$$$$$$$$$$$$$$o
                   oo$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$o         o$   $$ o$
   o $ oo        o$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$o       $$ $$ $$o$
oo $ $ "$      o$$$$$$$$$    $$$$$$$$$$$$$    $$$$$$$$$o       $$$o$$o$
"$$$$$$o$     o$$$$$$$$$      $$$$$$$$$$$      $$$$$$$$$$o    $$$$$$$$
  $$$$$$$    $$$$$$$$$$$      $$$$$$$$$$$      $$$$$$$$$$$$$$$$$$$$$$$
  $$$$$$$$$$$$$$$$$$$$$$$    $$$$$$$$$$$$$    $$$$$$$$$$$$$$  """$$$
   "$$$""""$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$     "$$$
    $$$   o$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$     "$$$o
   o$$"   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$       $$$o
   $$$    $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" "$$$$$$ooooo$$$$o
  o$$$oooo$$$$$  $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$   o$$$$$$$$$$$$$$$$$
  $$$$$$$$"$$$$   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$     $$$$""""""""
 """"       $$$$    "$$$$$$$$$$$$$$$$$$$$$$$$$$$$"      o$$$
            "$$$o     """$$$$$$$$$$$$$$$$$$"$$"         $$$
              $$$o          "$$""$$$$$$""""           o$$$
               $$$$o                 oo             o$$$"
                "$$$$o      o$$$$$$o"$$$$o        o$$$$
                  "$$$$$oo     ""$$$$o$$$$$o   o$$$$""  
                     ""$$$$$oooo  "$$$o$$$$$$$$$"""
                        ""$$$$$$$oo $$$$$$$$$$       
                                """"$$$$$$$$$$$        
                                    $$$$$$$$$$$$       
                                     $$$$$$$$$$"      
                                      "$$$""""

)" << '\n';
                    return;
                }

                passwordManager.loadFromFile("password.txt");
            }
        }
        else if (command == "c")
        {
            system("cls");
            system("clear");
        }
        else if (command == "g")
        {
            passwordManager.passwordGenerate();
        }
        else
        {
            std::cerr << "Unknown command" << std::endl;
        }
    }
}

int main()
{
    system("cls");
    system("clear");
    ui();
    return 0;
}
