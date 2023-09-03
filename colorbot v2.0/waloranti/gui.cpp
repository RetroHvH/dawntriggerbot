#include <windows.h>
#include <iostream>
#include <fstream>
#include "gui.hpp"
#include <string>
#include <map>
#include <sstream>
#include <conio.h>
#include <thread>
#include "keyAuth.hpp"

using namespace std;

#include <map> // Include the map library

std::map<std::string, int> Gui::config()
{
    // std::ifstream is RAII, i.e. no need to call close
    std::ifstream cFile("config.cfg");
    if (cFile.is_open())
    {
        std::map<std::string, int> configMap; // Create a map to store the configuration values

        std::string line;
        while (getline(cFile, line))
        {
            line.erase(std::remove_if(line.begin(), line.end(), isspace),
                       line.end());
            if (line.empty() || line[0] == '#')
            {
                continue;
            }
            auto delimiterPos = line.find("=");
            auto name = line.substr(0, delimiterPos);
            auto value = line.substr(delimiterPos + 1);


            // Convert value to an integer using std::stoi
            int intValue = std::stoi(value);

            // Store the integer value in the map using the name as the key
            configMap[name] = intValue;
        }

        // Now you can access the values by their names (keys) with the 'configMap' map.

        return configMap;
    }
    else
    {
        std::cerr << "Couldn't open config file for reading. Try reinstalling the program.\n";
        exit(1);
    }
}


int Gui::keybind()
{
    std::ifstream configFile("config.cfg");
    std::string line1, line2, line3, line4, line5, line6;

    // Read the first seven lines from the config file
    std::getline(configFile, line1);
    std::getline(configFile, line2);
    std::getline(configFile, line3);
    std::getline(configFile, line4);
    std::getline(configFile, line5);
    std::getline(configFile, line6);


    // Close the file as we don't need it anymore for reading
    configFile.close();

    // Get the virtual key code from the user
    std::cout << " Press any key or mouse button: ";
    int virtualKeyCode = 0;
    bool isMouseButton = false;
    Sleep(2000);
    while (true)
    {
        // Check for keyboard key press (non-zero return value)
        for (int key = 1; key <= 255; ++key)
        {
            if (GetAsyncKeyState(key) & 0x8000)
            {
                virtualKeyCode = key;
                break;
            }
        }

        // Check for mouse button press (non-zero return value)
        for (int button = 1; button <= 5; ++button)
        {
            if (GetAsyncKeyState(0x01 << (button - 1)) & 0x8000)
            {
                virtualKeyCode = button;
                isMouseButton = true;
                break;
            }
        }

        if (virtualKeyCode != 0)
            break;
    }

    // Open the config file in write mode
    std::ofstream outFile("config.cfg");

    // Write the first 7 lines back to the file
    outFile << line1 << '\n';
    outFile << line2 << '\n';
    outFile << line3 << '\n';
    outFile << line4 << '\n';
    outFile << line5 << '\n';


    // Write the 8th line with the pressed key/mouse button in decimal
    if (isMouseButton)
        outFile << "shoot_key=" << virtualKeyCode << '\n';
    else
        outFile << "shoot_key=" << virtualKeyCode << '\n';


    // Close the file
    outFile.close();

    std::cout << "\n Config file updated with the pressed key/mouse button code.\n";
    return 0;
}
