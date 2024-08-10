#include "imgui.h"

namespace c {

	inline ImVec4 accent = ImColor(255, 61, 129, 255);

	namespace bg
	{
		inline ImVec4 background = ImColor(18, 18, 18, 255);
		inline ImVec2 size = ImVec2(775, 570);
		inline float rounding = 16.f;
	}

	namespace child
	{
		inline ImVec4 background = ImColor(18, 20, 21, 255);
		inline ImVec4 outline = ImColor(61, 66, 81, 100);
		inline ImVec4 border = ImColor(36, 43, 54, 120);
		inline ImVec4 text = ImColor(255, 255, 255, 255);

		inline float rounding = 8.f;
	}

	namespace checkbox
	{
		inline ImVec4 background = ImColor(61, 66, 81, 100);
		inline ImVec4 outline = ImColor(72, 86, 121, 100);
		inline ImVec4 circle = ImColor(49, 47, 61, 255);

		inline float rounding = 30.f;
	}

	namespace slider
	{
		inline ImVec4 background = ImColor(25, 26, 31, 255);
		inline ImVec4 circle = ImColor(49, 47, 61, 255);
	}

	namespace input
	{
		inline ImVec4 background = ImColor(25, 26, 31, 255);
		inline ImVec4 icon = ImColor(49, 47, 61, 255);
		inline float rounding = 4.f;
	}

	namespace button
	{
		inline ImVec4 background_active = ImColor(45, 46, 51, 255);
		inline ImVec4 background_inactive = ImColor(25, 26, 31, 255);
		inline float rounding = 4.f;
	}

	namespace picker
	{
		inline ImVec4 background_inactive = ImColor(41, 46, 51, 255);
		inline ImVec4 outline = ImColor(72, 86, 121, 100);
		inline float rounding = 4.f;
	}

	namespace combo
	{
		inline ImVec4 background = ImColor(25, 26, 31, 255);
		inline ImVec4 icon = ImColor(49, 47, 61, 255);
		inline float rounding = 4.f;
	}

	namespace keybind
	{
		inline ImVec4 background_inactive = ImColor(25, 26, 31, 255);
		inline ImVec4 icon = ImColor(49, 47, 61, 255);
		inline float rounding = 4.f;
	}

	namespace tabs
	{
		inline ImVec4 background_active = ImColor(25, 26, 31, 255);
		inline ImVec4 background_inactive = ImColor(25, 26, 31, 0);
		inline float rounding = 4.f;
	}

	namespace text
	{
		inline ImVec4 text_active = ImColor(255, 255, 255, 255);
		inline ImVec4 text_hov = ImColor(120, 122, 137, 255);
		inline ImVec4 text = ImColor(100, 102, 117, 255);

		inline float glow_size = 7.f;
	}

}