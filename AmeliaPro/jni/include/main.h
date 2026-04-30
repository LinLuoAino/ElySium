#pragma once

#define CREATE_COLOR(r, g, b, a) new float[4] {(float)r, (float)g, (float)b, (float)a};

struct sColorsESP {
float *PVisible;
float *PIVisible;
float *BVisible;
float *BIVisible;
float *Color;
float *VBackgroud;
float *TColor;
float *IBackgroud;
float *IIBackgroud;
float *AimFov;
float *AimLine;
};
inline sColorsESP ColorsESP{0};

namespace font
{
    inline ImFont* inter_ch_bold = nullptr;
    inline ImFont* inter_ch_default = nullptr;    
    inline ImFont* inter_bold = nullptr;
    inline ImFont* inter_bold2 = nullptr;
    inline ImFont* inter_default = nullptr;
    inline ImFont* inter_ch_bold2 = nullptr;
    inline ImFont* pubg = nullptr;
    inline ImFont* opoo = nullptr;
    inline ImFont* oppo = nullptr;
    inline ImFont* LOGOFont = nullptr;
}

enum EEspTypebox {
D3 = 0,
Square = 1,
Corner = 2
};

enum EEspTypeline {
Top = 0,
Bottom = 1
};

enum EAimtype {
Aimbot = 0,
LegitBot = 1
};

enum EAimTrigger {
Scoping = 0,
Shooting = 1
};

struct sConfig {
bool MenuMove;
bool Exp;

struct sVisuals {
float LineT;
float BoxT;
float SkeletonT;
EEspTypebox BoxType;  
EEspTypeline LineType;  
bool Line;
bool Info;
bool SkipAi;
bool Skeleton;
bool Hp;
bool Weapon;
bool Box;
bool Screen;
bool Alert;
bool Lootbox;
};
sVisuals Visuals{0}; 


struct sRagebot {
float Recoil;
float Speed;
float Distance;
float Cross;
bool Line;
EAimTrigger Trigger; 
EAimtype Aimtype; 
bool Enable;
bool IgnoreAi;
bool IgnoreKnock;
bool Visible;
bool Switch;
};
sRagebot Ragebot{0}; 


struct sVehicle {
bool Motorbike;
bool Sidecart;
bool Dacia;
bool Uaz;
bool Buggy;
bool Mirado;
bool PickUp;
bool Rony;
bool Amphibious;
bool PG117;
bool Aquarail;
bool Enable;
bool text;
bool images;
bool backgroud;
float Distance;
};
sVehicle Vehicle{0};


struct sItems {
bool Akm;
bool SCAR;
bool M416;
bool Groza;
bool AUG;
bool M762;
bool UZI;
bool UMP45;
bool Kar98K;
bool M24;
bool AWM;
bool Mk14;
bool Mk12;
bool Flare;
bool Throwables;
bool text;
bool images;
bool backgroud;
float Distance;
};
sItems Items{0};

struct sAmmunition {
bool text;
bool images;
bool backgroud;
float Distance;
bool mm9;
bool mm7;
bool mm5;
bool mm45;
bool mm300;
bool mm12;
bool Energy;
bool Adrenaline;
bool Painkiller;
bool First;
bool Med;
bool Helmet3;
bool Backpack3;
bool Vest3;
};
sAmmunition Ammunition{0};


struct sSdk {
bool Fps;
bool Cross;
bool Hit;
bool Camera;
bool AutoLoad;
bool Check;
bool Info;
float CameraT;  
};
sSdk Sdk{0};
};
inline sConfig Config{0};

inline bool info_bar = true;
inline const char* cheat_name = "AMELIA";

inline void TouchScroll(const bool& InertiaFlag = false)
{
    ImGuiWindow *WinPtr = ImGui::GetCurrentWindow();
    float ScrollMaxY = ImGui::GetScrollMaxY();
    if (WinPtr->SkipItems || ScrollMaxY <= 0.0f)
        return;

    static float ScrollVel = 0.0f;
    static float ScrollAccel = 0.0f;
    bool Hover = ImGui::IsWindowHovered();
    ImGuiIO &GuiIO = ImGui::GetIO();

    static ImVec2 StartTouchPos = ImVec2(0, 0);
    static bool Scroll = false;
    static float LastScrollY = 0.0f;
    static double LastTime = 0.0;

    auto UpdateScroll = [&](float deltaY, float deltaTime) {
        float CurScrollY = ImGui::GetScrollY();
        float NewScrollY = CurScrollY + deltaY;
        NewScrollY = ImClamp(NewScrollY, 0.0f, ScrollMaxY);
        ImGui::SetScrollY(NewScrollY);

        ScrollVel = deltaY / (deltaTime + 0.0001f);
        ScrollAccel = ScrollVel / (deltaTime + 0.0001f); // 计算加速度
        LastScrollY = NewScrollY;
    };

    auto GetDeltaTime = [&]() {
        double CurTime = ImGui::GetTime();
        float DeltaTime = (float)(CurTime - LastTime);
        LastTime = CurTime;
        return DeltaTime;
    };

    auto ToggleParentWindowMove = [WinPtr](bool noMove) {
        if (ImGuiWindow *ParWinPtr = WinPtr->ParentWindow)
        {
            if (noMove)
            {
                ParWinPtr->Flags |= ImGuiWindowFlags_NoMove;
            }
            else
            {
                ParWinPtr->Flags &= ~ImGuiWindowFlags_NoMove;
            }
        }
    };

    if (Hover && GuiIO.MouseDown[0] && !Scroll)
    {
        StartTouchPos = GuiIO.MousePos;
        Scroll = true;
        LastScrollY = ImGui::GetScrollY();
        LastTime = ImGui::GetTime();
        ScrollVel = 0.0f; // 重置速度
        ScrollAccel = 0.0f; // 重置加速度
        ToggleParentWindowMove(true);
    }

    if (Scroll)
    {
        float DeltaTime = GetDeltaTime();

        if (GuiIO.MouseDown[0])
        {
            float DeltaY = StartTouchPos.y - GuiIO.MousePos.y;
            UpdateScroll(DeltaY, DeltaTime);
            StartTouchPos = GuiIO.MousePos;
        }
        else
        {
            Scroll = false;
            ToggleParentWindowMove(false);
        }
    }

    // 惯性滚动部分
    if (!Scroll && fabsf(ScrollVel) > 10.0f && InertiaFlag) // 增加速度阈值
    {
        float DeltaTime = GetDeltaTime();
        float CurScrollY = ImGui::GetScrollY();
        
        // 应用惯性效果
        float NewScrollY = CurScrollY + ScrollVel * DeltaTime;
        
        // 应用阻尼效果
        float damping = 0.95f; // 阻尼系数
        ScrollVel *= powf(damping, DeltaTime * 60.0f);
        
        // 边界检查
        if (NewScrollY <= 0.0f)
        {
            NewScrollY = 0.0f;
            ScrollVel = 0.0f;
        }
        else if (NewScrollY >= ScrollMaxY)
        {
            NewScrollY = ScrollMaxY;
            ScrollVel = 0.0f;
        }
        
        ImGui::SetScrollY(NewScrollY);
        
        // 当速度很小时停止
        if (fabsf(ScrollVel) < 1.0f)
        {
            ScrollVel = 0.0f;
        }
    }
    else if (!Scroll)
    {
        ScrollVel = 0.0f;
    }
}