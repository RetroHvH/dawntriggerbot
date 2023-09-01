
#include "../keyAuth.hpp"
#include "gui.hpp"

#include <conio.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include "skStr.h"






std::string tm_to_readable_time(tm ctx);
static std::time_t string_to_timet(std::string timestamp);
static std::tm timet_to_tm(time_t timestamp);
const std::string compilation_date = (std::string)skCrypt(__DATE__);
const std::string compilation_time = (std::string)skCrypt(__TIME__);
Gui gui_instance1;

using namespace KeyAuth;

std::string name = "Dawn Triggerbot"; // application name. right above the blurred text aka the secret on the licenses tab among other tabs
std::string ownerid = "Vo8fR22isC"; // ownerid, found in account settings. click your profile picture on top right of dashboard and then account settings.
std::string secret = "f1c45f503a60028f53857843fe6f3b5713270a05b07a1bccf7ce33e4acd09655"; // app secret, the blurred text on licenses tab and other tabs
std::string version = "1.0"; // leave alone unless you've changed version on website
std::string url = skCrypt("https://keyauth.win/api/1.2/").decrypt(); // change if you're self-hosting
int temp;
bool done = false;

/*
    KeyAuth.cc C++ example

    Setup tutorial https://www.youtube.com/watch?v=uJ0Umy_C6Fg go to https://keyauth.cc/app/, select C++, and copy into here

    Video on how to add KeyAuth to your own application https://www.youtube.com/watch?v=GB4XW_TsHqA

    Video to use Web Loader (control loader from customer panel) https://www.youtube.com/watch?v=9-qgmsUUCK4
*/

api KeyAuthApp(name, ownerid, secret, version, url);

int auth()
{
    std::string consoleTitle = (std::string)skCrypt("Loader - Built at:  ") + compilation_date + " " + compilation_time;
    SetConsoleTitleA(consoleTitle.c_str());
    std::cout << skCrypt("\n\n Connecting..");
    KeyAuthApp.init();
    if (!KeyAuthApp.data.success)
    {
        std::cout << skCrypt("\n Status: ") << KeyAuthApp.data.message;
        Sleep(1500);

        exit(0);
        
    }


    else
    {
        
        while (done == false){
        std::cout << skCrypt("\n\n [1] Login\n [2] Register\n [3] Upgrade\n [4] License key only\n [5] KeyBind \n Choose option: ");
        int option;
        std::string username;
        std::string password;
        std::string key;
            std::cin >> option;
            
                switch (option)
                {
                case 1:
                    std::cout << skCrypt("\n\n Enter username: ");
                    std::cin >> username;
                    std::cout << skCrypt("\n Enter password: ");
                    std::cin >> password;
                    KeyAuthApp.login(username, password);
                    done = true;

                    break;
                case 2:
                    std::cout << skCrypt("\n\n Enter username: ");
                    std::cin >> username;
                    std::cout << skCrypt("\n Enter password: ");
                    std::cin >> password;
                    std::cout << skCrypt("\n Enter license: ");
                    std::cin >> key;
                    KeyAuthApp.regstr(username, password, key);
                    done = true;

                    break;
                case 3:
                    std::cout << skCrypt("\n\n Enter username: ");
                    std::cin >> username;
                    std::cout << skCrypt("\n Enter license: ");
                    std::cin >> key;
                    KeyAuthApp.upgrade(username, key);
                    done = true;

                    break;
                case 4:
                    std::cout << skCrypt("\n Enter license: ");
                    std::cin >> key;
                    KeyAuthApp.license(key);
                    done = true;

                    break;
                case 5:

                    temp = gui_instance1.keybind();                    
                    Sleep(3000);
                    system("cls");
                    break;

                default:
                    std::cout << skCrypt("\n\n Status: Failure: Invalid Selection");

                    Sleep(3000);
                    exit(0);


                }
            }

        system("cls");
        if (!KeyAuthApp.data.success)
        {
            std::cout << skCrypt("\n Status: ") << KeyAuthApp.data.message;

            Sleep(1500);
            exit(0);

        }



    }
    std::cout << skCrypt("\n Welcome to Dawn Private");
    std::cout << skCrypt("\n");
    std::cout << skCrypt("\n Username: ") << KeyAuthApp.data.username;
    std::cout << skCrypt("\n Create date: ") << tm_to_readable_time(timet_to_tm(string_to_timet(KeyAuthApp.data.createdate)));
    std::cout << skCrypt("\n Last login: ") << tm_to_readable_time(timet_to_tm(string_to_timet(KeyAuthApp.data.lastlogin)));
    std::cout << skCrypt("\n Subscription(s): ");

    for (int i = 0; i < KeyAuthApp.data.subscriptions.size(); i++) {
        auto sub = KeyAuthApp.data.subscriptions.at(i);
        std::cout << skCrypt("\n name: ") << sub.name;
        std::cout << skCrypt(" : expiry: ") << tm_to_readable_time(timet_to_tm(string_to_timet(sub.expiry)));
    }
    return(0);


}

std::string tm_to_readable_time(tm ctx) {
    char buffer[80];

    strftime(buffer, sizeof(buffer), "%a %m/%d/%y %H:%M:%S %Z", &ctx);

    return std::string(buffer);
}

static std::time_t string_to_timet(std::string timestamp) {
    auto cv = strtol(timestamp.c_str(), NULL, 10); // long

    return (time_t)cv;
}

static std::tm timet_to_tm(time_t timestamp) {
    std::tm context;

    localtime_s(&context, &timestamp);

    return context;
}
