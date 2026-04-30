#include "imgui.h"

namespace c {

	inline ImVec4 accent = ImColor(75, 0, 130, 255); // 暗紫色 (Dark Purple)

	namespace bg
	{

		inline ImVec4 background = ImColor(0, 0, 0, 200);
		inline ImVec2 size = ImVec2(960, 610);
		inline float rounding = 18.f;

	}

	namespace child
	{

		inline ImVec4 background = ImColor(80, 80, 80, 50);
		inline ImVec4 outline = ImColor(255, 255, 255, 15);
		inline float rounding = 10.f;

	}

	namespace checkbox
	{
		inline ImVec4 background = ImColor(255, 255, 255, 15);
		inline ImVec4 circle_inactive = ImColor(80, 80, 80, 255);
		inline float rounding = 2.5f;
	}

	namespace slider
	{
		inline ImVec4 background = ImColor(255, 255, 255, 15);
		inline ImVec4 circle_inactive = ImColor(80, 80, 80, 255);
		inline float rounding = 2.5f;
	}

	namespace tabs
	{
		inline ImVec4 background = ImColor(16, 17, 19, 255);

		inline ImVec4 outline = ImColor(22, 24, 26, 255);
		inline float rounding = 5.f;
	}

	namespace combo
	{
		inline ImVec4 background = ImColor(16, 17, 19, 255);
		inline ImVec4 outline = ImColor(22, 24, 26, 255);
		inline ImVec4 icon = ImColor(26, 27, 31, 255);

		inline float rounding = 5.f;
	}

	namespace picker
	{
		inline ImVec4 background = ImColor(16, 17, 19, 255);
		inline ImVec4 outline = ImColor(22, 24, 26, 255);

		inline float rounding = 2.5f;
	}

	namespace button
	{
		inline ImVec4 background_active = ImColor(255, 255, 255, 30);
		inline ImVec4 background_inactive = ImColor(255, 255, 255, 15);

		inline ImVec4 outline = ImColor(255, 255, 255, 15);

		inline float rounding = 5.f;
	}

	namespace keybind
	{
		inline ImVec4 background_active = ImColor(26, 27, 29, 255);
		inline ImVec4 background_inactive = ImColor(16, 17, 19, 255);

		inline ImVec4 outline = ImColor(22, 24, 26, 255);

		inline float rounding = 2.f;
	}

	namespace input
	{
		inline ImVec4 background_active = ImColor(20, 20, 20, 255);
		inline ImVec4 background_inactive = ImColor(20, 20, 20, 255);

		inline ImVec4 outline = ImColor(255, 255, 255, 15);
		inline ImVec4 icon = ImColor(26, 27, 31, 255);

		inline float rounding = 5.f;
	}

	namespace scroll
	{
		inline float rounding = 30.f;
	}

	namespace text
	{
		inline ImVec4 text_active = ImColor(255, 255, 255, 255);
		inline ImVec4 text_hov = ImColor(86, 88, 95, 255);
		inline ImVec4 text = ImColor(100, 100, 100, 255);

		inline float glow_size = 15.f;
	}
}
