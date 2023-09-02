#ifndef GUIHPP
#define GUIHPP
#include <windows.h>
#include <iostream>
#include <fstream>
#include "gui.hpp"
#include <string>
#include <map>

class Gui
{
public:
    static std::map<std::string, int> config();
    int keybind();
};
#endif
