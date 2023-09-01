#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <Windows.h>
#include "../gui.hpp"


namespace cfg
{
	constexpr int aimbot_key{ VK_LBUTTON };
	constexpr int magnet_key{ VK_XBUTTON2 };
	constexpr int flick_key{ VK_XBUTTON1 };
	constexpr int head_offset_x{ 2 };
	constexpr int head_offset_y{ 4 };
	inline int magnet_fov{ 9 };
	inline int magnet_smooth{ 8 };
	inline int magnet_delay_between_shots{ 200 };
	inline int aimbot_fov{ 9 };
	inline int aimbot_smooth{ 14 };
	inline int recoil_length{ 44 };
	inline double recoil_offset{ 0 };
}





#endif // !CONFIG_HPP
