#include "enemy_scanner/enemy_scanner.hpp"
#include "utilities/utilities.hpp"
#include "utilities/config.hpp"
#include "utilities/skcrypt.hpp"
#include "gui.hpp"
#include "capturescreen.hpp"
#include "keyAuth.hpp"
#include "utilities/skcrypt.hpp"
#include "DD.h"

#include <vector>
#include <windows.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <array>
#include <math.h>



using namespace std;

Gui gui_instance2;
std::map<std::string, int> configMap2 = gui_instance2.config();

enemy_scanner scanner;


void click_mouse()
{
    INPUT input = {0};
    // Set up a generic mouse event
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &input, sizeof(INPUT));

    ZeroMemory(&input, sizeof(INPUT));
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &input, sizeof(INPUT));
}

void printCursorPosition()
{
    POINT cursorPos;
    if (GetCursorPos(&cursorPos))
    {
        std::cout << "Cursor Position - X: " << cursorPos.x << ", Y: " << cursorPos.y << std::endl;
    }
    else
    {
        std::cerr << "Failed to get cursor position." << std::endl;
    }
}


bool CheckDebuggerPresent()
{
    return IsDebuggerPresent() != 0;
}

int main()
{
    if (IsDebuggerPresent())
    {
        // Debugger is Detected; Take Action Here
        exit(1);
    }

    int shoot_delay_in_ms = configMap2["shoot_delay_in_ms"];
    int shoot_key = configMap2["shoot_key"];
    int shot_delay = configMap2["delay_in_between_shoots_inMS"];
    //int fov = configMap2["magnet_fov"];
    //int smooth = configMap2["magnet_smooth"];
    const std::vector<int> fail = {6969, 4200};

    auth();

    scanner.initColor();

    while (true)
    {
        if (utilities::is_pressed(shoot_key))
        {
            Image currentScreen = captureDesktop();
            std::vector<int> Target = scanner.find_enemy_outline(currentScreen);
            if (Target != fail && Target[0] < 1440 && Target[1] < 2560 && Target[0] > 0 && Target[1] > 0)
            {
               
                printCursorPosition();
                
            }

            if (scanner.is_crosshair_on_enemy(currentScreen))
            {
                click_mouse();
                Sleep(shot_delay);
            }
        }
    }
    return 0;
}
