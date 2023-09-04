#ifndef ENEMY_SCANNER_HPP
#define ENEMY_SCANNER_HPP

#include <vector>
#include <iostream>
#include <windows.h>
#include "../capturescreen.hpp"
#include <algorithm>

class enemy_scanner
{
private:
    struct vec2
    {
        double x{0.0};
        double y{0.0};
    };

public:
    enemy_scanner()
    {
    };


    bool is_enemy_outline(const std::vector<int>& pixel) const;
    static bool is_enemy_outline_old(const std::vector<int>& pixel);
    bool is_crosshair_on_enemy(const Image& screencapture) const;
    int find_enemy_outline_bottom(const Image& image) const;
    int find_enemy_outline_top(const Image& image) const;
    std::vector<int> find_enemy_outline(const Image& image) const;
    static void initColor();
};


#endif // !ENEMY_SCANNER_HPP
