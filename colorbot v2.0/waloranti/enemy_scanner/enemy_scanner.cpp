#include "enemy_scanner.hpp"
#include "../utilities/config.hpp"
#include <Windows.h>
#include <chrono>
#include <iostream>
#include "../gui.hpp"
#include <thread>
#include <future>


Gui gui_instance;
std::map<std::string, int> configMap = gui_instance.config();
int Loffset = configMap["Trigger_Offset_Left"];
int Roffset = configMap["Trigger_Offset_Right"];
int delay = configMap["shoot_delay_in_ms"];
int i = 1;
int RED_RANGE[2];
int GREEN_RANGE[2];
int BLUE_RANGE[2];
float RED_DOMINANCE_THRES[2];
float GREEN_DOMINANCE_THRES[2];
float BLUE_DOMINANCE_THRES[2];
const std::vector<int> fail = { 6969,4200 };
std::vector<int> top;
std::vector<int> bottom;

void enemy_scanner::initColor() {
    if (configMap["color"] == 1) {
        RED_RANGE[0] = 200;
        RED_RANGE[1] = 255;
        GREEN_RANGE[0] = 0;
        GREEN_RANGE[1] = 200;
        BLUE_RANGE[0] = 200;
        BLUE_RANGE[1] = 255;

        RED_DOMINANCE_THRES[0] = 0.8f;
        RED_DOMINANCE_THRES[1] = 1.0f;
        GREEN_DOMINANCE_THRES[0] = 0.0f;
        GREEN_DOMINANCE_THRES[1] = 0.80f;
        BLUE_DOMINANCE_THRES[0] = 0.5f;
        BLUE_DOMINANCE_THRES[1] = 1.0f;
    }
    else {
        RED_RANGE[0] = 150;
        RED_RANGE[1] = 255;
        GREEN_RANGE[0] = 125;
        GREEN_RANGE[1] = 255;
        BLUE_RANGE[0] = 30;
        BLUE_RANGE[1] = 30;

        RED_DOMINANCE_THRES[0] = 0.6f;
        RED_DOMINANCE_THRES[1] = 1.0f;
        GREEN_DOMINANCE_THRES[0] = 0.5f;
        GREEN_DOMINANCE_THRES[1] = 1.0f;
        BLUE_DOMINANCE_THRES[0] = 0.1f;
        BLUE_DOMINANCE_THRES[1] = 0.2f;
    }
}

bool enemy_scanner::is_enemy_outline(const std::vector<int>& pixel) const
{
    float max = static_cast<float>(max(pixel[0], max(pixel[1], pixel[2])));
    float dominance[3] = { pixel[0] / max, pixel[1] / max, pixel[2] / max };
    if (pixel[0] >= RED_RANGE[0] && pixel[0] <= RED_RANGE[1] &&
        pixel[1] >= GREEN_RANGE[0] && pixel[1] <= GREEN_RANGE[1] &&
        pixel[2] >= BLUE_RANGE[0] && pixel[2] <= BLUE_RANGE[1] &&
        dominance[0] >= RED_DOMINANCE_THRES[0] && dominance[0] <= RED_DOMINANCE_THRES[1] &&
        dominance[1] >= GREEN_DOMINANCE_THRES[0] && dominance[1] <= GREEN_DOMINANCE_THRES[1] &&
        dominance[2] >= BLUE_DOMINANCE_THRES[0] && dominance[2] <= BLUE_DOMINANCE_THRES[1])
        return true;

    return false;
}

bool enemy_scanner::is_enemy_outline_old(const std::vector< int >& pixel)
{
    if (pixel[1] >= 170) {
        return false;
    }

    if (pixel[1] >= 120) {
        return abs(pixel[0] - pixel[2]) <= 8 &&
            pixel[0] - pixel[1] >= 50 &&
            pixel[2] - pixel[1] >= 50 &&
            pixel[0] >= 105 &&
            pixel[2] >= 105;
    }

    return abs(pixel[0] - pixel[2]) <= 13 &&
        pixel[0] - pixel[1] >= 60 &&
        pixel[2] - pixel[1] >= 60 &&
        pixel[0] >= 110 &&
        pixel[2] >= 100;
}

bool enemy_scanner::is_crosshair_on_enemy(const Image& screencapture) const {
    auto start_time = std::chrono::high_resolution_clock::now();
    bool topCheck = false;
    bool alreadyCheckedTop = false;
    bool alreadyCheckedBottom = false;
    bool bottomCheck = false;
    std::vector<RGB> abovePixelsR = getPixelsAboveCenter(screencapture, Roffset);
    std::vector<RGB> belowPixelsR = getPixelsBelowCenter(screencapture, Roffset);
    std::vector<RGB> abovePixelsL = getPixelsAboveCenter(screencapture, Loffset);
    std::vector<RGB> belowPixelsL = getPixelsBelowCenter(screencapture, Loffset);

    for (size_t y = 0; y < abovePixelsR.size(); y++) {
        if (alreadyCheckedTop && alreadyCheckedBottom) {
            return true; // No need to continue the loop, we already found both top and bottom checks
        }

        RGB tempR = abovePixelsR[y];
        RGB tempL = abovePixelsL[y];
        std::vector<int> finalR = { tempR.r, tempR.g, tempR.b };
        std::vector<int> finalL = { tempL.r, tempL.g, tempL.b };

        if (is_enemy_outline(finalR) || is_enemy_outline(finalL) && !alreadyCheckedTop) {
            topCheck = true;
            alreadyCheckedTop = true;
        }

        RGB temp1R = belowPixelsR[y];
        RGB temp1L = belowPixelsL[y];
        std::vector<int> final1R = { temp1R.r, temp1R.g, temp1R.b };
        std::vector<int> final1L = { temp1L.r, temp1L.g, temp1L.b };

        if (is_enemy_outline(final1R) || is_enemy_outline(final1L) && !alreadyCheckedBottom) {
            bottomCheck = true;
            alreadyCheckedBottom = true;
        }
        if (alreadyCheckedBottom && alreadyCheckedTop) {
            break;
        }
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    int duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    if (delay - duration_ms > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(delay - duration_ms - 16));
    }
    return (bottomCheck && topCheck);

}

int enemy_scanner::find_enemy_outline_bottom(const Image& image) const {
    std::vector<int> result;

    int width = image.width;
    int height = image.height;
    int z = image.height/2;
    int x = 0;
    int pixel_count = 0;
    int count2 = 0;
    bool alreadySet =false;
    // Iterate through the pixels from left to right and top to bottom
    while (z < height && z > 0 ) {
        for (int x = 0; x < width; ++x) {
            // Get the RGB value of the current pixel
            std::vector<int> rgb;
            for (int f = 0; f < 3; ++f) {
                rgb.push_back(image.bytes[z * width * 4 + x * 4 + f]);
            }

            // Check if the RGB value represents an enemy outline
            if (is_enemy_outline(rgb)) {
                rgb.clear();
                result.push_back(x); // x-coordinate
                result.push_back(z); 
                std::cout<<"Target:" << "\n";
                std::cout << x << "\n";
                std::cout << z << "\n";
                std::cout << "\n";// y-coordinate
                top = result;
                return 0;// Found the enemy outline, return immediately
            }
            else
            {
                rgb.clear();

            }
        }
       /* if (z == image.height - 1  && alreadySet == false)
        {
            z = image.height/2;
            alreadySet = true;
        }
        if (alreadySet == false)
        {
            z++;
        }
        if (alreadySet == true)
        {
            z--;
        }
        count2++;
        if (count2 >= 1439)
        {
            break;
        }*/
        z++;
    }

    // Return an empty vector if no enemy outline is found
    top = fail;
    return 0;
}
int enemy_scanner::find_enemy_outline_top(const Image& image) const {
    std::vector<int> result;

    int width = image.width;
    int height = image.height;
    int z = image.height/2;
    int x = 0;
    int pixel_count = 0;
    int count2 = 0;
    bool alreadySet =false;
    // Iterate through the pixels from left to right and top to bottom
    while (z <= height/2 && z > 0 ) {
        for (int x = 0; x < width; ++x) {
            // Get the RGB value of the current pixel
            std::vector<int> rgb;
            for (int f = 0; f < 3; ++f) {
                rgb.push_back(image.bytes[z * width * 4 + x * 4 + f]);
            }

            // Check if the RGB value represents an enemy outline
            if (is_enemy_outline(rgb)) {
                rgb.clear();
                result.push_back(x); // x-coordinate
                result.push_back(z); 
                std::cout<<"Target:" << "\n";
                std::cout << x << "\n";
                std::cout << z << "\n";
                std::cout << "\n";// y-coordinate
                bottom = result;
                return 0;// Found the enemy outline, return immediately
            }
            else
            {
                rgb.clear();

            }
        }
        /* if (z == image.height - 1  && alreadySet == false)
         {
             z = image.height/2;
             alreadySet = true;
         }
         if (alreadySet == false)
         {
             z++;
         }
         if (alreadySet == true)
         {
             z--;
         }
         count2++;
         if (count2 >= 1439)
         {
             break;
         }*/
        z--;
        //return 1;
    }

    // Return an empty vector if no enemy outline is found
    bottom = fail;
    return 0;
}

std::vector<int> enemy_scanner::find_enemy_outline(const Image& image) const{
    std::thread thread1([this, &image] { find_enemy_outline_top(image); });
    std::thread thread2([this, &image] { find_enemy_outline_bottom(image); });

    thread1.join();
    thread2.join();
    // Wait for and retrieve the return values

    if (bottom != fail)
    {
        return bottom;
    }
    if (top != fail)
    {
        return top;
    }
    if (top == fail && bottom == fail)
    {
        return fail;
    }
    return fail;
    
}


