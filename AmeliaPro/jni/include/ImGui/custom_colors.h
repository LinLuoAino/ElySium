#pragma once
#include "imgui.h"
extern int 菜单透明度;
class c_colors
{
public:
    // 强调色，用于突出显示某些元素
    ImVec4 accent = ImColor(119, 112, 255);  // 改为新主题的accent颜色

    //文本颜色
    ImVec4 Text = ImColor(255, 255, 255);

    //提示文本前面的小图标
    ImVec4 TextColored = ImColor(81, 255, 81);

    //提示文本颜色
    ImVec4 Texticon = ImColor(81, 255, 81);

    // 窗口背景颜色
    ImVec4 window_background = ImColor(17, 17, 20);  // 改为新主题的window.layout
    // 窗口标题栏、菜单栏等条带的颜色
    ImVec4 window_bars = ImColor(22, 22, 27);  // 改为新主题的window.header
    // 窗口名称的文本颜色
    ImVec4 window_name = ImColor(255, 255, 255);
    // 窗口游戏名称的文本颜色
    ImVec4 window_game_name = ImColor(137, 137, 140);

    // 选中项的背景颜色
    ImVec4 selection_background = ImColor(22, 22, 27);  // 改为新主题的selection.layout
    // 选中项图标的颜色
    ImVec4 selection_icon = ImColor(63, 63, 80);  // 改为新主题的text.text_inactive
    // 选中项标签的文本颜色
    ImVec4 selection_label = ImColor(63, 63, 80);  // 改为新主题的text.text_inactive

    // 子窗口顶部栏的颜色
    ImVec4 child_topbar = ImColor(22, 22, 27);  // 改为新主题的window.header
    // 子窗口的背景颜色
    ImVec4 child_background = ImColor(21, 21, 26);  // 改为新主题的child.layout
    // 子窗口标签的文本颜色
    ImVec4 child_label = ImColor(255, 255, 255);

    // 滚动条的背景颜色
    ImVec4 scrollbar_background = ImColor(20, 20, 25);

    // 复选框的背景颜色
    ImVec4 checkbox_background = ImColor(27, 27, 34);  // 改为新主题的checkbox.layout
    // 复选框未true时的颜色
    ImVec4 checkbox_label = ImColor(136, 136, 138);
    //复选框标签的文本颜色
    ImVec4 checkbox_label_active = ImColor(255, 255, 255);
    // 复选框选中标记的颜色
    ImVec4 checkbox_checkmark = ImColor(20, 20, 25);

    // 滑块的背景颜色
    ImVec4 slider_background = ImColor(27, 27, 34);  // 改为新主题的slider.layout
    // 滑块标签的文本颜色
    ImVec4 slider_label = ImColor(255, 255, 255);
    // 滑块圆形手柄的颜色
    ImVec4 slider_circle = ImColor(255, 255, 255);
    // 滑块值的文本颜色
    ImVec4 slider_value = ImColor(136, 136, 138);

    // 组合框标签的文本颜色
    ImVec4 combo_label = ImColor(255, 255, 255);
    // 组合框的背景颜色
    ImVec4 combo_background = ImColor(27, 27, 34);  // 改为新主题的dropdown.layout
    // 组合框选中值的文本颜色
    ImVec4 combo_value = ImColor(136, 136, 138);

    // 可选项目未激活时的颜色
    ImVec4 selectable_inactive = ImColor(136, 136, 138);
    // 可选项目激活时的颜色
    ImVec4 selectable_active = ImColor(255, 255, 255);

    // 输入框标签的文本颜色
    ImVec4 input_label = ImColor(255, 255, 255);
    // 输入框的背景颜色
    ImVec4 input_background = ImColor(27, 27, 34);
    // 输入框内文本的颜色
    ImVec4 input_value = ImColor(136, 136, 138);

    // 颜色选择器标签的文本颜色
    ImVec4 color_label = ImColor(255, 255, 255);
    // 颜色选择器的背景颜色
    ImVec4 color_background = ImColor(27, 27, 34);

    // 按钮的背景颜色
    ImVec4 button_background = ImColor(27, 27, 34);  // 改为新主题的button.layout
    // 按钮未激活时标签的文本颜色
    ImVec4 button_label = ImColor(136, 136, 138);
    // 按钮激活时标签的文本颜色
    ImVec4 button_label_active = ImColor(255, 255, 255);

    // 弹出窗口的背景颜色
    ImVec4 popup_background = ImColor(17, 17, 20);  // 改为新主题的window.layout
    // 弹出窗口中选中项的背景颜色
    ImVec4 popup_selection_background = ImColor(22, 22, 27);
    // 弹出窗口中选中项标签的文本颜色
    ImVec4 popup_selection_label = ImColor(255, 255, 255);
    // 弹出窗口中按键的背景颜色
    ImVec4 popup_key_background = ImColor(22, 22, 27);

    // 水印的背景颜色
    ImVec4 watermark_background = ImColor(17, 17, 20, 255);  // 改为新主题的watermark.layout
    //水印的文本颜色
    ImVec4 watermark_text = ImColor(255, 255, 255);

    // 通知框的背景颜色
    ImVec4 notify_background = ImColor(17, 17, 20);  // 改为新主题的notify.layout
    // 通知框内文本的颜色
    ImVec4 notify_text = ImColor(255, 255, 255);

    void updateColors() {
        window_background = ImColor(17, 17, 20, 菜单透明度);
        window_bars = ImColor(22, 22, 27, 菜单透明度);
        selection_background = ImColor(22, 22, 27, 20);
        child_topbar = ImColor(22, 22, 27, 菜单透明度);
        child_background = ImColor(21, 21, 26, 菜单透明度);
        scrollbar_background = ImColor(20, 20, 25, 20);

        checkbox_background = ImColor(27, 27, 34, 20);
        //checkbox_checkmark = ImColor(20, 20, 25, 30);

        slider_background = ImColor(27, 27, 34, 20);
        combo_background = ImColor(27, 27, 34, 20);
        input_background = ImColor(27, 27, 34, 20);
        color_background = ImColor(27, 27, 34, 20);

        button_background = ImColor(27, 27, 34, 20);

        // 弹出窗口的背景颜色
        popup_background = ImColor(17, 17, 20, 20);
        // 弹出窗口中选中项的背景颜色
        popup_selection_background = ImColor(22, 22, 27, 20);
        // 弹出窗口中按键的背景颜色
        popup_key_background = ImColor(22, 22, 27, 20);

        watermark_background = ImColor(17, 17, 20, 20);
        notify_background = ImColor(17, 17, 20, 20);
    }

};

inline c_colors* clr = new c_colors();