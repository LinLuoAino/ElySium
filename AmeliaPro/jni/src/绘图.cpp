#include "图片调用.h"
#include "物资ID.h"
#include "辅助类.h"
#include <list>
#include <chrono>
#include <vector>
#include <cmath>
#include <algorithm>
#include <imgui.h>
#include <map>
#include "main.h"
#include "custom_settings.h"//定义

extern 绘制 绘制;

std::map<int, MyTextureData> 手持图片;

static float easeOutCubic(float t) {
    return 1.0f - pow(1.0f - t, 3.0f);
}

static float easeInOutCubic(float t) {
    return t < 0.5f ? 4.0f * t * t * t : 1.0f - pow(-2.0f * t + 2.0f, 3.0f) / 2.0f;
}

static float easeOutBack(float t) {
    const float c1 = 1.70158f;
    const float c3 = c1 + 1.0f;
    return 1.0f + c3 * pow(t - 1.0f, 3.0f) + c1 * pow(t - 1.0f, 2.0f);
}

static int getLength(const char *str)
{
    int len = 0;
    while (*str)
    {
        if ((*str & 0xC0) != 0x80)
        {
            len++;
        }
        ++str;
    }
    return len;
}

void 绘图::初始化绘图(int X, int Y)
{
    if (Y < X)
    {
        this->PX = X / 2;
        this->PY = Y / 2;
    }
    else
    {
        this->PX = Y / 2;
        this->PY = X / 2;
    }
}

void 绘图::绘制瞄准信息()
{
    ImGui::PushFont(YuanerSet->inter_semibold_font);
    static std::unordered_map<int, float> animationProgress;
    static std::unordered_map<int, float> animationTime;
    const float animationDuration = 0.5f;
    float deltaTime = ImGui::GetIO().DeltaTime;

    auto* drawList = ImGui::GetBackgroundDrawList();

    for (int i = 0; i < 绘制.被瞄准对象数量; i++)
    {
        if (animationProgress.find(i) == animationProgress.end()) {
            animationProgress[i] = 0.0f;
            animationTime[i] = 0.0f;
        } else if (animationProgress[i] < 1.0f) {
            animationTime[i] += deltaTime;
            float t = std::min(animationTime[i] / animationDuration, 1.0f);
            animationProgress[i] = easeOutBack(t);
        }

        float animFactor = animationProgress[i];
        float alphaMult = animFactor * animFactor;
        int alphaValue = static_cast<int>(255 * alphaMult);

        string 距离显示 = to_string(static_cast<int>(绘制.被瞄信息[i].距离)) + "M";
        
        ImVec2 szN = ImGui::CalcTextSize(绘制.被瞄信息[i].名字.c_str(), 0, 32);
        ImVec2 szW = ImGui::CalcTextSize(绘制.被瞄信息[i].瞄准武器.c_str(), 0, 32);
        ImVec2 szD = ImGui::CalcTextSize(距离显示.c_str(), 0, 32);
        ImVec2 szFix = ImGui::CalcTextSize(" 正在凝视您 ", 0, 28);

        float spacing = 15.0f * animFactor;
        float totalWidth = szN.x + szW.x + szD.x + szFix.x + (spacing * 4);
        float startY = 200.0f + (60.0f * i);
        float centerX = PX;

        float bgWidth = (totalWidth + 40.0f) * animFactor;
        ImVec2 pMin = { centerX - bgWidth / 2, startY - 5 };
        ImVec2 pMax = { centerX + bgWidth / 2, startY + 45 };

        drawList->AddRectFilled(pMin, pMax, ImColor(5, 5, 10, (int)(180 * alphaMult)), 5.0f);
        
        float pulse = (sinf(ImGui::GetTime() * 4.0f) * 0.5f + 0.5f);
        drawList->AddRect(pMin, pMax, ImColor(218, 165, 32, (int)((100 + 100 * pulse) * alphaMult)), 5.0f, 0, 1.2f);

        float curX = pMin.x + 20.0f;
        float textY = startY + 5;

        drawList->AddText(NULL, 32, ImVec2(curX, textY), ImColor(255, 250, 240, alphaValue), 绘制.被瞄信息[i].名字.c_str());
        curX += szN.x + spacing;

        drawList->AddText(NULL, 28, ImVec2(curX, textY + 2), ImColor(180, 180, 180, (int)(150 * alphaMult)), " 正在凝视您 ");
        curX += szFix.x + spacing;

        drawList->AddText(NULL, 32, ImVec2(curX, textY), ImColor(200, 0, 0, alphaValue), 绘制.被瞄信息[i].瞄准武器.c_str());
        curX += szW.x + spacing;

        drawList->AddText(NULL, 32, ImVec2(curX, textY), ImColor(255, 215, 0, alphaValue), 距离显示.c_str());

        if (animFactor > 0.8f) {
            float lineY = pMax.y - 2;
            float lineGlow = (sinf(ImGui::GetTime() * 8.0f + i) * 0.5f + 0.5f);
            drawList->AddLine({pMin.x + 10, lineY}, {pMax.x - 10, lineY}, ImColor(218, 165, 32, (int)(150 * lineGlow * alphaMult)), 1.5f);
        }
    }

    for (auto it = animationProgress.begin(); it != animationProgress.end(); ) {
        if (it->first >= 绘制.被瞄准对象数量) {
            animationTime.erase(it->first);
            it = animationProgress.erase(it);
        } else {
            ++it;
        }
    }
    ImGui::PopFont();
}

void 绘图::初始化坐标(D4DVector &屏幕坐标, 骨骼数据 &骨骼)
{
    left = 骨骼.Head.X - 屏幕坐标.Z / 2;
    right = 骨骼.Head.X + 屏幕坐标.Z / 2;
    top = 骨骼.Pelvis.Y - ((骨骼.Head.X != 0) ? (骨骼.Pelvis.Y - 骨骼.Head.Y) : (骨骼.Pelvis.Y - 骨骼.Chest.Y)) - 屏幕坐标.Z / 5;
    bottom = (骨骼.Left_Ankle.Y < 骨骼.Right_Ankle.Y) ? (骨骼.Right_Ankle.Y + 屏幕坐标.Z / 10) : (骨骼.Left_Ankle.Y + 屏幕坐标.Z / 10);
    MIDDLE = 屏幕坐标.X + 屏幕坐标.Z / 2;
    BOTTOM = 屏幕坐标.Y + 屏幕坐标.Z;
    TOP = 屏幕坐标.Y - 屏幕坐标.Z;
    屏幕坐标.X += 屏幕坐标.Z / 2;
}

void 绘图::绘制方框(bool isboot)
{
    int colorIndex = 绘制.对象信息.敌人信息.isboot ? 0 : 1;
    ImColor 方框color = 绘制.Colorset[colorIndex].方框颜色;

    ImGui::GetForegroundDrawList()->AddRect({left, top}, {right, bottom}, 方框color, 0.0f, 0, 绘制.按钮.方框粗细);
}

void 绘图::绘制漏手(bool isboot)
{
    ImColor color = 绘制.Colorset[绘制.对象信息.敌人信息.isboot ? 0 : 1].方框颜色;

    ImVec2 center = {(left + right) / 2, (top + bottom) / 2};
    float radius = 绘制.按钮.圆点大小;

    ImGui::GetForegroundDrawList()->AddCircleFilled(center, radius, color);
}

void 绘图::绘制人数(int 人机, int 真人) {
    if (真人 == 0 && 人机 == 0) {
        ImGui::PushFont(font::LOGOFont);
        ImVec2 屏幕尺寸 = ImGui::GetIO().DisplaySize;    
        const char* 文本 = "ElySium";
        ImVec2 文本尺寸 = ImGui::CalcTextSize(文本);
    
        float 水平位置 = (屏幕尺寸.x - 文本尺寸.x) / 2 - 46.0f;
        float 垂直位置 = 85.0f;
    
        ImVec2 阴影偏移 = ImVec2(2.0f, 2.0f);
        ImColor 阴影颜色 = ImColor(0, 0, 0, 128);
        绘制加粗文本(45, 水平位置 + 阴影偏移.x, 垂直位置 + 阴影偏移.y, 文本尺寸.y, 阴影颜色, 文本);
    
        ImColor 黄金色 = ImColor(255, 215, 0);
        绘制加粗文本(45, 水平位置, 垂直位置, 文本尺寸.y, 黄金色, 文本);
    
        ImGui::PopFont();
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(0, 0));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::Begin("##HiddenPlayerCount", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground |ImGuiWindowFlags_NoBringToFrontOnFocus);

    char 人机文本[32], 真人文本[32];
    snprintf(人机文本, sizeof(人机文本), "%d", 人机);
    snprintf(真人文本, sizeof(真人文本), "%d", 真人);

    ImGui::PushFont(font::LOGOFont);
    ImGui::SetWindowFontScale(45.0f / 25.0f);    
    ImVec2 人机尺寸 = ImGui::CalcTextSize(人机文本);
    ImVec2 真人尺寸 = ImGui::CalcTextSize(真人文本);    
    const float 偏移量 = 45.0f;
    float 人机中心X = PX + 偏移量;
    float 真人中心X = PX - 偏移量;
    float 人机位置X = 人机中心X - 人机尺寸.x / 2;
    float 真人位置X = 真人中心X - 真人尺寸.x / 2;
    ImVec4 真人颜色 = ImVec4(1.0f, 0.f, 0.f, 1.0f);
    ImVec4 人机颜色 = ImVec4(0.9f, 0.95f, 1.0f, 1.0f);    
    绘制字体描边(45, 人机位置X, 85, ImColor(人机颜色), 人机文本);
    绘制字体描边(45, 真人位置X, 85, ImColor(真人颜色), 真人文本);
    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopFont();
    
    ImGui::End();
}

void 绘图::绘制射线(骨骼数据 &骨骼, bool LineOfSightToTab[15])
{
    int 可见部位数量 = 0;
    for (int i = 0; i < 15; i++) {
        if (LineOfSightToTab[i]) {
            可见部位数量++;
        }
    }
    
    ImColor 射线颜色;
    if (可见部位数量 == 15) {
        射线颜色 = ImColor(255, 0, 0, 255);
    } else if (可见部位数量 > 0) {
        float 可见比例 = 可见部位数量 / 15.0f;
        射线颜色 = ImColor(255, (int)(255 * (1 - 可见比例)), (int)(100 * (1 - 可见比例)), 255);
    } else {
        射线颜色 = ImColor(0, 255, 0, 255);
    }
    
    ImGui::GetForegroundDrawList()->AddLine({PX, 0}, {骨骼.Head.X, top - 65}, 射线颜色, 绘制.按钮.射线粗细);
}

void 绘图::绘制血量(float 最大血量, float 当前血量, float 敌人距离)
{
    float 血量 = 当前血量 / 最大血量 * 100;
    if (绘制.按钮.血条样式 == 0)
    {
        auto *drawList = ImGui::GetForegroundDrawList();
        const float 基础宽度 = 125.0f;
        float 宽度缩放 = std::clamp(1.0f - (敌人距离 / 300.0f), 0.3f, 1.0f);
        float healthBarWidth = 基础宽度 * 宽度缩放;
        const float heightReduction = 12.0f;
        const float yOffset = 13.0f;
        const float healthBarHeight = 5.0f;
        float healthBarLeft = MIDDLE - healthBarWidth / 2;
        
        const float 圆角值 = 50.0f;
        float healthPercentage = std::clamp(当前血量 / 最大血量, 0.0f, 1.0f);
        
        ImVec2 背景左上 = ImVec2(healthBarLeft, top - 30 + yOffset);
        ImVec2 背景右下 = ImVec2(healthBarLeft + healthBarWidth, top - 30 + yOffset + healthBarHeight);
        drawList->AddRectFilled(背景左上, 背景右下, ImColor(0, 0, 0, 200), 圆角值);
        
        if (healthPercentage > 0) {
            ImVec2 血量左上 = 背景左上;
            ImVec2 血量右下 = ImVec2(healthBarLeft + (healthBarWidth * healthPercentage), 背景右下.y);
            drawList->AddRectFilled(血量左上, 血量右下, 绘制.Colorset[1].血量颜色, 圆角值);
        }
    }
    else if (绘制.按钮.血条样式 == 1)
    {
        std::string duo = std::to_string(static_cast<int>(血量));
        auto textSize = ImGui::CalcTextSize(duo.c_str());
        ImVec2 circleCenter = {MIDDLE, top - 75 - 20};
        float radius = 20.0f;
        ImGui::GetForegroundDrawList()->AddCircle(circleCenter, radius, ImColor(100, 100, 100, 210), 0, 3.0f);
        float healthRatio = 血量 / 100.0f;
        if (healthRatio > 1.0f) healthRatio = 1.0f;
        if (healthRatio < 0.0f) healthRatio = 0.0f;
        float start_angle = -IM_PI / 2.0f;
        float end_angle = start_angle + 2.0f * IM_PI * healthRatio;
        ImGui::GetForegroundDrawList()->PathArcTo(circleCenter, radius - 2.0f, start_angle, end_angle, 32);
        ImGui::GetForegroundDrawList()->PathStroke(绘制.Colorset[1].血量颜色, false, 4.0f);
        ImVec2 textPos = {circleCenter.x - (textSize.x * 0.5f), circleCenter.y - (textSize.y * 0.5f)};
        ImGui::GetForegroundDrawList()->AddText(nullptr, 20, textPos, ImColor(255, 255, 255), duo.c_str());
    }
    else if (绘制.按钮.血条样式 == 2)
    {
        float hps = bottom-top;
        if (血量>=35)
        {
            ImGui::GetBackgroundDrawList()->AddRectFilled({right, bottom-hps}, {right+7, bottom},绘制.Colorset[1].血量颜色);
            ImGui::GetBackgroundDrawList()->AddRect({right, top}, {right+7, bottom},ImColor(0,255,0,200), {0}, 0, {1.5});
        } else {
            ImGui::GetBackgroundDrawList()->AddRectFilled({right, bottom-hps}, {right+7, bottom},绘制.Colorset[1].血量颜色);
            ImGui::GetBackgroundDrawList()->AddRect({right, top}, {right+7, bottom},ImColor(0,255,0,200), {0}, 0, {1.5});
        }
    }
}

void 绘图::绘制手持(int 手持, int 状态, int 手雷)
{
    手持 = heldconversion(手持);
    手雷 = 手雷投掷物(手雷);
    if (手持图片.find(手持) != 手持图片.end())
    {
        if (绘制.按钮.血条样式 == 1)
        {
            ImGui::GetForegroundDrawList()->AddImage(手持图片[手持].DS, ImVec2(MIDDLE - 75 * 0.7f, top - 155 * 0.7f - 38), ImVec2(MIDDLE + 75 * 0.7f, top - 120 * 0.7f - 38), ImVec2(0, 0), ImVec2(1, 1), ImColor(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)));
        } else {
            ImGui::GetForegroundDrawList()->AddImage(手持图片[手持].DS, ImVec2(MIDDLE - 75 * 0.7f, top - 155 * 0.7f), ImVec2(MIDDLE + 75 * 0.7f, top - 120 * 0.7f), ImVec2(0, 0), ImVec2(1, 1), ImColor(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)));
        }
    }
    if (手持图片.find(手雷) != 手持图片.end())
    {
        if (绘制.按钮.血条样式 == 1)
        {
            ImGui::GetForegroundDrawList()->AddImage(手持图片[手雷].DS, ImVec2(MIDDLE - 75 * 0.7f, top - 155 * 0.7f - 38), ImVec2(MIDDLE + 75 * 0.7f, top - 120 * 0.7f - 38), ImVec2(0, 0), ImVec2(1, 1), ImColor(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)));
        } else {
            ImGui::GetForegroundDrawList()->AddImage(手持图片[手雷].DS, ImVec2(MIDDLE - 75 * 0.7f, top - 155 * 0.7f), ImVec2(MIDDLE + 75 * 0.7f, top - 120 * 0.7f), ImVec2(0, 0), ImVec2(1, 1), ImColor(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)));
        }
    }
}

void 绘图::绘制骨骼(骨骼数据 &骨骼, D4DVector &屏幕坐标, bool LineOfSightToTab[15], int 距离)
{
    if (距离 <= (int)绘制.骨骼距离限制)
    {
        auto &颜色 = 绘制.Colorset[绘制.对象信息.敌人信息.isboot ? 0 : 1].骨骼颜色;
        ImColor 默认颜色 = 绘制.Colorset[绘制.对象信息.敌人信息.isboot ? 0 : 1].骨骼颜色;
        ImColor 可见颜色 = ImColor(0, 255, 0, 100);

        auto *背景绘制 = ImGui::GetBackgroundDrawList();
        auto *前景绘制 = ImGui::GetForegroundDrawList();

        if (骨骼.Head.X != 0 && 骨骼.Head.Y != 0) {
            背景绘制->AddCircle(ImVec2(骨骼.Head.X, 骨骼.Head.Y), 屏幕坐标.W / 16, LineOfSightToTab[0] ? 可见颜色 : 默认颜色, 0, 绘制.按钮.骨骼粗细);
        }

        auto 绘制线段 = [&](int 连线索引, D2DVector &点1, D2DVector &点2)
        {
            if (点1.X == 0 && 点1.Y == 0) return;
            if (点2.X == 0 && 点2.Y == 0) return;
            
            float dx = 点2.X - 点1.X;
            float dy = 点2.Y - 点1.Y;
            if ((dx * dx + dy * dy) < 10000)
            {
                ImColor color = LineOfSightToTab[连线索引] ? 可见颜色 : 默认颜色;
                前景绘制->AddLine(ImVec2(点1.X, 点1.Y), ImVec2(点2.X, 点2.Y), color, 绘制.按钮.骨骼粗细);
            }
        };

        绘制线段(1, 骨骼.Chest, 骨骼.Pelvis);
        绘制线段(3, 骨骼.Chest, 骨骼.Left_Shoulder);
        绘制线段(4, 骨骼.Left_Shoulder, 骨骼.Left_Elbow);
        绘制线段(5, 骨骼.Left_Elbow, 骨骼.Left_Wrist);
        绘制线段(6, 骨骼.Chest, 骨骼.Right_Shoulder);
        绘制线段(7, 骨骼.Right_Shoulder, 骨骼.Right_Elbow);
        绘制线段(8, 骨骼.Right_Elbow, 骨骼.Right_Wrist);
        绘制线段(9, 骨骼.Pelvis, 骨骼.Left_Thigh);
        绘制线段(10, 骨骼.Left_Thigh, 骨骼.Left_Knee);
        绘制线段(11, 骨骼.Left_Knee, 骨骼.Left_Ankle);
        绘制线段(12, 骨骼.Pelvis, 骨骼.Right_Thigh);
        绘制线段(13, 骨骼.Right_Thigh, 骨骼.Right_Knee);
        绘制线段(14, 骨骼.Right_Knee, 骨骼.Right_Ankle);
    }
}

void 绘图::头甲包显示(骨骼数据& t_骨骼数据, int 敌人头, int 敌人甲, int 敌人包, D4DVector& t_屏幕坐标) 
{
    if (!绘制.按钮.头甲包显示)
        return;

    float top = t_骨骼数据.Pelvis.Y - ((t_骨骼数据.Head.X != 0) ? (t_骨骼数据.Pelvis.Y - t_骨骼数据.Head.Y) : (t_骨骼数据.Pelvis.Y - t_骨骼数据.Chest.Y)) - t_屏幕坐标.Z / 5;

    ImVec2 screenSize = ImGui::GetIO().DisplaySize;
    auto* drawList = ImGui::GetForegroundDrawList();

    const float 血条顶部偏移 = 30.0f;
    float 名字标签顶部 = top - 血条顶部偏移 - 2.0f - 22.0f + 5.0f;

    std::string 穿戴 = "头:" + tou(敌人头) + "  ";
    穿戴 += "甲:" + jia(敌人甲) + "  ";
    穿戴 += "包:" + bao(敌人包);

    float 穿戴信息顶部 = 名字标签顶部 - ImGui::CalcTextSize(穿戴.c_str()).y - 2.0f;
    float 穿戴信息水平位置 = MIDDLE - ImGui::CalcTextSize(穿戴.c_str()).x / 2;

    if (穿戴信息水平位置 + ImGui::CalcTextSize(穿戴.c_str()).x < 0 || 穿戴信息水平位置 > screenSize.x || 穿戴信息顶部 + ImGui::CalcTextSize(穿戴.c_str()).y < 0 || 穿戴信息顶部 > screenSize.y) 
    {
        return;
    }

    drawList->AddText(nullptr, 16.0f, {穿戴信息水平位置, 穿戴信息顶部}, IM_COL32(255, 255, 255, 255), 穿戴.c_str());
}

void 绘图::绘制自瞄触摸范围(float 触摸范围, float 触摸范围X, float 触摸范围Y)
{
    auto textSize = ImGui::CalcTextSize("触摸位置,长按拖动", 0, 30);
    ImGui::GetForegroundDrawList()->AddRectFilled({触摸范围X - 触摸范围 / 2, PY * 2 - 触摸范围Y + 触摸范围 / 2}, {触摸范围X + 触摸范围 / 2, PY * 2 - 触摸范围Y - 触摸范围 / 2}, ImColor(255, 0, 0, 120));
    ImGui::GetForegroundDrawList()->AddText(NULL, 30, {触摸范围X - (textSize.x / 2), PY * 2 - 触摸范围Y}, ImColor(255, 255, 255), "触摸位置,长按拖动");
}

void 绘图::绘制自救(float 自救倒计时)
{
    string 计时文本 = "";
    char buffer[50];
    std::sprintf(buffer, "%.1f", 自救倒计时);
    计时文本 = "正在自救-[" + std::string(buffer) + "]";
    auto textSize = ImGui::CalcTextSize(计时文本.c_str());
    ImVec2 计时位置 = ImVec2(MIDDLE - (textSize.x / 4)-20, top - 70);
    绘制字体描边(30, 计时位置.x, 计时位置.y, ImColor(255, 0, 0, 255), 计时文本.c_str());
}

void 绘图::绘制连点触摸圆点(float 连点X, float 连点Y, float 连点次数, float 连点间隔)
{
    const float 圆点半径 = 绘制.念宝.连点范围; 
    const char* 提示文字 = "连点位置";
    auto textSize = ImGui::CalcTextSize(提示文字);
    ImGui::GetForegroundDrawList()->AddCircleFilled({连点X, 连点Y}, 圆点半径, ImColor(0, 0, 255, 120), 38);
    ImGui::GetForegroundDrawList()->AddText(NULL, 30, {连点X - (textSize.x / 2), 连点Y + 圆点半径 + 5}, ImColor(255, 255, 255), 提示文字);
}

void 绘图::绘制字体描边(float size, int x, int y, ImVec4 color, const char *str)
{
    ImGui::GetBackgroundDrawList()->AddText(nullptr, size, ImVec2(x + 1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), str);
    ImGui::GetBackgroundDrawList()->AddText(nullptr, size, ImVec2(x - 0.1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), str);
    ImGui::GetBackgroundDrawList()->AddText(nullptr, size, ImVec2(x, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), str);
    ImGui::GetBackgroundDrawList()->AddText(nullptr, size, ImVec2(x, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), str);
    ImGui::GetBackgroundDrawList()->AddText(nullptr, size, ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(color), str);
}

void 绘图::绘制加粗文本(float size, float x, float y, ImColor color, ImColor color1, const char *str)
{
    ImGui::GetBackgroundDrawList()->AddText(nullptr, size, ImVec2(x - 0.1, y - 0.1), color1, str);
    ImGui::GetBackgroundDrawList()->AddText(nullptr, size, ImVec2(x + 0.1, y + 0.1), color1, str);
    ImGui::GetBackgroundDrawList()->AddText(nullptr, size, ImVec2(x, y), color, str);
}

bool 绘图::WorldTurnScreen(VecTor2 &Screen, VecTor3 World, float Matrix[])
{
    float Camera = Matrix[3] * World.x + Matrix[7] * World.y + Matrix[11] * World.z + Matrix[15];
    if (Camera < 0.03)
    {
        return false;
    }
    Screen.x = PX + (Matrix[0] * World.x + Matrix[4] * World.y + Matrix[8] * World.z + Matrix[12]) / Camera * PX;
    Screen.y = PY - (Matrix[1] * World.x + Matrix[5] * World.y + Matrix[9] * World.z + Matrix[13]) / Camera * PY;
    return true;
}

void 绘图::ExplosionRange(D3DVector Obj, ImColor color, float Range, float thickn, float Matrix[])
{
    VecTor2 centerScreen;
    if (!WorldTurnScreen(centerScreen, VecTor3(Obj.X, Obj.Y, Obj.Z), Matrix))
        return;

    const int segments = 36;
    std::vector<VecTor2> screenPoints;
    
    for (int i = 0; i < segments; i++)
    {
        float angle = (float)i / segments * 2.0f * 3.1415926f;
        VecTor3 worldPoint = VecTor3(Obj.X + Range * cosf(angle), Obj.Y + Range * sinf(angle), Obj.Z);
        VecTor2 screenPoint;
        if (WorldTurnScreen(screenPoint, worldPoint, Matrix))
        {
            screenPoints.push_back(screenPoint);
        }
    }
    
    if (screenPoints.size() < 2) return;
    
    for (size_t i = 0; i < screenPoints.size(); i++)
    {
        size_t next_i = (i + 1) % screenPoints.size();
        ImGui::GetForegroundDrawList()->AddLine(ImVec2(screenPoints[i].x, screenPoints[i].y), ImVec2(screenPoints[next_i].x, screenPoints[next_i].y), color, thickn);
    }
}

void 绘图::Parabola(VecTor3 obj, float Matrix[])
{
    static std::vector<VecTor3> grenadePath;
    VecTor3 newGrenadePosition;
    newGrenadePosition = obj;
    grenadePath.push_back(newGrenadePosition);

    if (grenadePath.size() > 500)
    {
        grenadePath.erase(grenadePath.begin(), grenadePath.begin() + grenadePath.size() - 500);
    }
    
    VecTor2 screenPos;
    std::vector<VecTor2> screenPath;
    
    for (const VecTor3 &pos : grenadePath)
    {
        float w = Matrix[3] * pos.x + Matrix[7] * pos.y + Matrix[11] * pos.z + Matrix[15];
        if (w > 0.001f)
        {
            screenPos.x = (Matrix[0] * pos.x + Matrix[4] * pos.y + Matrix[8] * pos.z + Matrix[12]) / w;
            screenPos.y = (Matrix[1] * pos.x + Matrix[5] * pos.y + Matrix[9] * pos.z + Matrix[13]) / w;
            screenPos.x = PX + screenPos.x * PX;
            screenPos.y = PY - screenPos.y * PY;           
            screenPath.push_back(screenPos);
        }
    }

    if (screenPath.size() > 1)
    {
        const float MAX_DISTANCE_THRESHOLD = 100.0f;
        for (size_t i = 1; i < screenPath.size(); ++i)
        {
            if (screenPath[i].x >= 0 && screenPath[i].x <= 2*PX && screenPath[i].y >= 0 && screenPath[i].y <= 2*PY)
            {
                size_t closestIndex = i - 1;
                float minDistance = std::numeric_limits<float>::max();
                bool foundClosePoint = false;
                
                for (size_t j = 0; j < i; ++j)
                {
                    if (screenPath[j].x >= 0 && screenPath[j].x <= 2*PX && screenPath[j].y >= 0 && screenPath[j].y <= 2*PY)
                    {
                        float distance = std::sqrt(std::pow(screenPath[i].x - screenPath[j].x, 2) + std::pow(screenPath[i].y - screenPath[j].y, 2));
                        if (distance < minDistance && distance <= MAX_DISTANCE_THRESHOLD)
                        {
                            minDistance = distance;
                            closestIndex = j;
                            foundClosePoint = true;
                        }
                    }
                }
                
                if (foundClosePoint)
                {
                    ImGui::GetForegroundDrawList()->AddLine({screenPath[i].x, screenPath[i].y}, {screenPath[closestIndex].x, screenPath[closestIndex].y}, ImColor(255, 255, 255, 255), 1.4f);
                }
            }
        }
    }
}