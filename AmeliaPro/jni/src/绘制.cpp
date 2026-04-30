#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <map>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <thread>
#include <atomic>
#include <chrono>
#include <thread>
#include "图片调用.h"
#include "物资ID.h"
#include "辅助类.h"
#include "武器.h"
#include "Gryo/Gryo.h"
#include <memory>
#include <mutex>
#include <random>
#include "main.h"
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <stdexcept>
#include <cmath>
#include <set>
#include <android/log.h>
#include "自救.h"
#include "custom_settings.h"

#include <curl/curl.h>
#include <string>
#include <cstdint>
#include <iostream>
#include <sys/ptrace.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <linux/elf.h>
#include <map>
#include "PhysX.h"

#include <elf.h>
#include <unordered_map>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <map>
#include <sys/user.h>
#include <stdint.h>
#include <dlfcn.h>
#include <arm_acle.h>

#include <iostream>
#include <vector>
#include <string>
#include <sys/ptrace.h>
#include <sys/uio.h>
#include <elf.h>

#include <errno.h>
#include <assert.h>

int SmokeCount;
D3DVector SmokePos[20];

extern 绘制 绘制;
extern 念宝 念宝;

std::map<std::string, std::chrono::steady_clock::time_point> 自救Timers;
void 更新自救倒计时() {
    auto it = 自救Timers.begin();
    while (it != 自救Timers.end()) {
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - it->second
        ).count();
        if (elapsed >= 10) it = 自救Timers.erase(it);
        else ++it;
    }
}
std::chrono::steady_clock::time_point startTime;
std::string 持续锁定名字 = "";

int ShelterJudgment(bool LineOfSightToTab[15])
{
    for (int i = 0; i < 15; i++)
    {
        if (!LineOfSightToTab[i])
        {
            return i;
        }
    }
    return 999;
}

float 散弹枪(int WeaponId)
{
    switch (WeaponId)
    {
    case 104005:
        return 1.0;
        break;
    case 104001:
        return 1.0;
        break;
    case 104002:
        return 1.0;
        break;
    case 104003:
        return 1.0;
        break;
    case 104004:
        return 1.0;
        break;
    case 104100:
        return 1.0;
        break;
    case 104101:
        return 1.0;
        break;
    case 104102:
        return 1.0;
        break;
    //适配地铁武器
    case 9810001:
        return 1.0;
        break;
    case 9810002:
        return 1.0;
        break;
    case 9810003:
        return 1.0;
        break;
    case 9810008:
        return 1.0;
        break;
    case 9810009:
        return 1.0;
        break;
    case 9810010:
        return 1.0;
        break;
    case 9810015:
        return 1.0;
        break;
    case 9810016:
        return 1.0;
        break;
    case 9810017:
        return 1.0;
        break;
    case 9810018:
        return 1.0;
        break;
    case 9810022:
        return 1.0;
        break;
    case 9810023:
        return 1.0;
        break;
    case 9810024:
        return 1.0;
        break;
    case 9810025:
        return 1.0;
        break;
    case 9810029:
        return 1.0;
        break;
    case 9810030:
        return 1.0;
        break;
    case 9810031:
        return 1.0;
        break;
    case 9810032:
        return 1.0;
        break;
    case 9810036:
        return 1.0;
        break;
    case 9810037:
        return 1.0;
        break;
    case 9810038:
        return 1.0;
        break;
    case 9810041:
        return 1.0;
        break;
    }
    return 0.0; // 不是喷子
}

inline bool 是弓(int WeaponId)
{
    switch (WeaponId)
    {
    case 107001: 
    case 107007: 
        return true;
        break;
    default:
        return false;
        break;
    }
}
inline bool 是冲锋枪(int WeaponId)
{
    switch (WeaponId)
    {
    case 102001: 
    case 102002: 
    case 102003: 
    case 102004: 
    case 102005: 
    case 102007: 
    case 102008: 
    case 102105: 
        return true;
        break;
    default:
        return false;
        break;
    }
}
inline bool 是连狙(int WeaponId)
{
    switch (WeaponId)
    {
    case 103004: 
    case 103005: 
    case 103006: 
    case 103007: 
    case 103009: 
    case 103010: 
    case 103013: 
    case 103014: 
    case 103100: 
        return true;
        break;
    default:
        return false;
        break;
    }
}
inline bool 是单发狙(int WeaponId)
{
    switch (WeaponId)
    {
    case 103001: 
    case 103002: 
    case 103003: 
    case 103008: 
    case 103011: 
    case 103012: 
    case 103015: 
    case 103018: 
        return true;
        break;
    default:
        return false;
        break;
    }
}
std::random_device rand_device;
std::mt19937 engine(rand_device());
std::uniform_int_distribution<int> dist(0, 100);
uintptr_t 弹道追踪函数;
uintptr_t 弹道追踪函数1;
int sizet = 0;
uintptr_t lockBase;
float 追踪距离限制;
float 追踪概率限制;
float 追踪范围限制;
bool 倒地不追;
bool 人机不追;

struct Rotator1 
{
    float Y; // P
    float X; // Y
    float Roll;
};

struct vector31
{
    float X;
    float Y;
    float Z;
};

FRotator toRotator(vector31 local, D3DVector target) 
{
    D3DVector rotation;
    rotation.X = local.X - target.X;
    rotation.Y = local.Y - target.Y;
    rotation.Z = local.Z - target.Z;
    
    FRotator newViewAngle;
    float hyp = sqrt(rotation.X * rotation.X + rotation.Y * rotation.Y);
    
    newViewAngle.Pitch = -atan(rotation.Z / hyp) * (180.f / (float)3.14159265358979323846);
    newViewAngle.Yaw = atan(rotation.Y / rotation.X) * (180.f / (float)3.14159265358979323846);
    newViewAngle.Roll = (float)0.f;
    
    if (rotation.X >= 0.f)
    {
        newViewAngle.Yaw += 180.0f;
    }
    
    return newViewAngle;
}
float 人物距离;

static uint shellcode[19] = 
{
    0xD100C3FF, 0xF9000FE0, 0xF9400FE0, 0xBD508000, 
    0xBD0023E0, 0xF9400FE0, 0xBD508400, 0xBD0027E0, 
    0xF9400FE0, 0xBD508800, 0xBD002BE0, 0xBD4023E2, 
    0xBD4027E1, 0xBD402BE0, 0x1E204043, 0x1E204002, 
    0x1E204060, 0x9100C3FF, 0xD65F03C0
};

void getFunctionHex(uintptr_t function)
{
    for (int i = 0; i < 999;)
    {
        int 机械码 = 0;
        机械码 = *(int*)(function + i * 4);
        shellcode[i] = 机械码;
        
        if (机械码 == 0xD65F03C0)
        {
            sizet = i + 1;
            break;
        }
        else
        {
            i++;
        }
    }
}
void InjectFunction(uintptr_t address, uintptr_t function) {
    getFunctionHex(function);
    char filename[64] = {0};
    sprintf(filename,"/proc/%d/mem", 绘制.地址.pid1);
    int fd = open(filename, O_RDWR | O_SYNC);
    iovec iov = {shellcode, static_cast<size_t>(sizet) * 4};
    syscall(SYS_pwritev, fd, &iov, 1, address, 0);
}
Rotator1 hook3(void* a1)
{
    uintptr_t WeaponComponent = (uintptr_t)a1;
    float x = *(float*)(WeaponComponent + 0x50);
    float y = *(float*)(WeaponComponent + 0x54);
    float z = *(float*)(WeaponComponent + 0x58);
    
    return { x, y, 0 };
}

std::map<std::string, int> Antitankgrenade;

void 绘制::ReadsSaveData(ifstream& file) {
    file.read(reinterpret_cast<char*>(&按钮), sizeof(按钮));
    file.read(reinterpret_cast<char*>(&自瞄), sizeof(自瞄));
    file.read(reinterpret_cast<char*>(&念宝), sizeof(念宝));
    file.read(reinterpret_cast<char*>(&防录屏), sizeof(防录屏));
    file.read(reinterpret_cast<char*>(&无后台开关), sizeof(无后台开关));
    file.read(reinterpret_cast<char*>(&压枪力), sizeof(压枪力));
    file.read(reinterpret_cast<char*>(&预判度), sizeof(预判度));
    file.read(reinterpret_cast<char*>(&Colorset[1].血量颜色), sizeof(Colorset[1].血量颜色));
    file.read(reinterpret_cast<char*>(&Colorset[1].射线颜色), sizeof(Colorset[1].射线颜色));
    file.read(reinterpret_cast<char*>(&Colorset[1].骨骼颜色), sizeof(Colorset[1].骨骼颜色));
    file.read(reinterpret_cast<char*>(&Colorset[1].方框颜色), sizeof(Colorset[1].方框颜色));
    file.read(reinterpret_cast<char*>(&Colorset[0].血量颜色), sizeof(Colorset[0].血量颜色));
    file.read(reinterpret_cast<char*>(&Colorset[0].射线颜色), sizeof(Colorset[0].射线颜色));
    file.read(reinterpret_cast<char*>(&Colorset[0].骨骼颜色), sizeof(Colorset[0].骨骼颜色));
    file.read(reinterpret_cast<char*>(&Colorset[0].方框颜色), sizeof(Colorset[0].方框颜色));
    file.read(reinterpret_cast<char*>(&物资颜色), sizeof(物资颜色));
}

void 绘制::WriteSaveData(ofstream& file) {
    file.write(reinterpret_cast<char*>(&按钮), sizeof(按钮));
    file.write(reinterpret_cast<char*>(&自瞄), sizeof(自瞄));
    file.write(reinterpret_cast<char*>(&念宝), sizeof(念宝));
    file.write(reinterpret_cast<char*>(&防录屏), sizeof(防录屏));
    file.write(reinterpret_cast<char*>(&无后台开关), sizeof(无后台开关));
    file.write(reinterpret_cast<char*>(&压枪力), sizeof(压枪力));
    file.write(reinterpret_cast<char*>(&预判度), sizeof(预判度));
    file.write(reinterpret_cast<char*>(&Colorset[1].血量颜色), sizeof(Colorset[1].血量颜色));
    file.write(reinterpret_cast<char*>(&Colorset[1].射线颜色), sizeof(Colorset[1].射线颜色));
    file.write(reinterpret_cast<char*>(&Colorset[1].骨骼颜色), sizeof(Colorset[1].骨骼颜色));
    file.write(reinterpret_cast<char*>(&Colorset[1].方框颜色), sizeof(Colorset[1].方框颜色));
    file.write(reinterpret_cast<char*>(&Colorset[0].血量颜色), sizeof(Colorset[0].血量颜色));
    file.write(reinterpret_cast<char*>(&Colorset[0].射线颜色), sizeof(Colorset[0].射线颜色));
    file.write(reinterpret_cast<char*>(&Colorset[0].骨骼颜色), sizeof(Colorset[0].骨骼颜色));
    file.write(reinterpret_cast<char*>(&Colorset[0].方框颜色), sizeof(Colorset[0].方框颜色));
    file.write(reinterpret_cast<char*>(&物资颜色), sizeof(物资颜色));
}

void 绘制::保存配置(const char* name)
{
    ofstream file(GetFilePath(name), ios::binary | ios::out);
    if (file.is_open()) {
        WriteSaveData(file);
    }
}

void 绘制::读取配置(const char* name)
{
    ifstream file(GetFilePath(name), ios::binary | ios::in);
    if (file.is_open()) {
        ReadsSaveData(file);
    }
}

void 绘制::OffScreen(ImDrawList *ImDraw, D4DVector Obj, float camear, ImColor color, float Radius, float 距离)
{
    ImRect screen_rect = {0.0f, 0.0f, 真实PX, 真实PY};
    if (Obj.Z > 0 && screen_rect.Contains({Obj.X, Obj.Y}))
        return;
    auto screen_center = screen_rect.GetCenter();
    auto angle = atan2(screen_center.y - Obj.Y, screen_center.x - Obj.X);
    angle += camear > 0 ? M_PI : 0.0f;
    D2DVector arrow_center{screen_center.x + Radius * cosf(angle), screen_center.y + Radius * sinf(angle)};
    
    float angle_rad = 120.0f * M_PI / 180.0f;
    float half_angle = angle_rad / 2.0f;
    float length = 35.0f;
    
    std::array<ImVec2, 3> points{ImVec2(0.0f, 0.0f), ImVec2(-length * cosf(half_angle), -length * sinf(half_angle)), ImVec2(-length * cosf(half_angle), length * sinf(half_angle))};
    
    ImVec2 text_pos(-40.0f, 0.0f);
    
    float scale = 1.155f;
    D2DVector tpoint;
    
    for (auto &point : points)
    {
        auto x = point.x * scale;
        auto y = point.y * scale;
        point.x = arrow_center.X + x * cosf(angle) - y * sinf(angle);
        point.y = arrow_center.Y + x * sinf(angle) + y * cosf(angle);
    }
    
    auto text_x = text_pos.x * scale;
    auto text_y = text_pos.y * scale;
    tpoint.X = arrow_center.X + text_x * cosf(angle) - text_y * sinf(angle);
    tpoint.Y = arrow_center.Y + text_x * sinf(angle) + text_y * cosf(angle);

    float alpha = 1.0f;
    if (camear > 0)
    {
        constexpr float nearThreshold = 200 * 200;
        ImVec2 screen_outer_diff = {
            Obj.X < 0 ? abs(Obj.X) : (Obj.X > screen_rect.Max.x ? Obj.X - screen_rect.Max.x : 0.0f),
            Obj.Y < 0 ? abs(Obj.Y) : (Obj.Y > screen_rect.Max.y ? Obj.Y - screen_rect.Max.y : 0.0f),
        };
        float distance = static_cast<float>(pow(screen_outer_diff.x, 2) + pow(screen_outer_diff.y, 2));
        alpha = camear < 0 ? 1.0f : (distance / nearThreshold);
    }
    
    ImColor arrowColor;
    if (距离 < 50.0f) {
        arrowColor = ImColor(1.0f, 0.3f, 0.3f, alpha);
    } else if (距离 < 150.0f) {
        arrowColor = ImColor(1.0f, 0.7f, 0.3f, alpha);
    } else {
        arrowColor = ImColor(0.4f, 0.9f, 0.4f, alpha);
    }

    for (int i = 0; i < 5; i++) {
        float progress = (float)i / 4.0f;
        float startX = points[0].x * (1.0f - progress) + points[1].x * progress;
        float startY = points[0].y * (1.0f - progress) + points[1].y * progress;
        float endX = points[0].x * (1.0f - (progress + 0.25f)) + points[1].x * (progress + 0.25f);
        float endY = points[0].y * (1.0f - (progress + 0.25f)) + points[1].y * (progress + 0.25f);
        
        if (endX > points[1].x) {
            endX = points[1].x;
            endY = points[1].y;
        }
        
        float thickness = 4.0f * (1.0f - progress);
        if (thickness < 0.5f) thickness = 0.5f;
        
        ImDraw->AddLine(ImVec2(startX, startY), ImVec2(endX, endY), arrowColor, thickness);
    }
    
    for (int i = 0; i < 5; i++) {
        float progress = (float)i / 4.0f;
        float startX = points[0].x * (1.0f - progress) + points[2].x * progress;
        float startY = points[0].y * (1.0f - progress) + points[2].y * progress;
        float endX = points[0].x * (1.0f - (progress + 0.25f)) + points[2].x * (progress + 0.25f);
        float endY = points[0].y * (1.0f - (progress + 0.25f)) + points[2].y * (progress + 0.25f);
        
        if (endX > points[2].x) {
            endX = points[2].x;
            endY = points[2].y;
        }
        
        float thickness = 4.0f * (1.0f - progress);
        if (thickness < 0.5f) thickness = 0.5f;
        
        ImDraw->AddLine(ImVec2(startX, startY), ImVec2(endX, endY), arrowColor, thickness);
    }

    string tmp = to_string((int)距离) + "m";
    auto textSize = ImGui::CalcTextSize(tmp.c_str(), 0, 20);
    ImDraw->AddText(NULL, 20, {tpoint.X - textSize.x/2, tpoint.Y - textSize.y/2}, ImColor(1.0f, 1.0f, 1.0f, alpha), tmp.c_str());
}

void 绘制::初始化坐标(int 真实X, int 真实Y) {
    this->真实PX = 真实X;
    this->真实PY = 真实Y;
    
    if (真实Y < 真实X)
    {
        this->PX = 真实X / 2;
        this->PY = 真实Y / 2;
    }
    else
    {
        this->PX = 真实Y / 2;
        this->PY = 真实X / 2;
    }
}

#define UpdateVolley 0x870C07C
#define ShootBulletInner 0x86B0E20

void 绘制::初始化绘制(char* 包名) {
    地址.pid1 = 读写.getPID(包名);
    this->pid = 地址.pid1;
    读写.初始化读写(地址.pid1);

    地址.libue4 = 读写.get_module_base("libUE4.so");
    
    弹道追踪函数 = 地址.libue4 + UpdateVolley;        
    弹道追踪函数1 = 地址.libue4 + ShootBulletInner;
}

bool 绘制::AnianSmoke(D3DVector Pos)
{
    for (size_t i = 0; i < SmokeCount; i++)
    {
        float Smoke距离 = 计算.计算距离(SmokePos[i],Pos);
        if (Smoke距离 < 10)
        {
            return true;
        }
    }
    return false;
}

int 绘制::findminat()
{
    float DistanceMin = 450.0f;
    float min = 自瞄.自瞄范围;
    int minAt = 999;
    
    for (int i = 0; i < 自瞄.瞄准总数量; i++)
    {
        if (AnianSmoke(自瞄函数[i].瞄准坐标) && 自瞄.烟雾不描)
        {
            continue;
        }
    
        if (自瞄.三倍压枪) {
            if (自瞄函数[i].名字 == 持续锁定名字 && 自身数据.开火 == 1) {
                minAt = i;
                break;
            }
        }
        
        switch ((int)自瞄.瞄准优先)
        {
        case 0:
            if (自瞄函数[i].准心距离 < min && 自瞄函数[i].准心距离 != 0)
            {
                min = 自瞄函数[i].准心距离;
                minAt = i;
            }
            break;
        case 1:
            if (自瞄函数[i].准心距离 < 自瞄.自瞄范围)
            {
                if (自瞄函数[i].距离 < DistanceMin)
                {
                    DistanceMin = 自瞄函数[i].距离;
                    minAt = i;
                }
            }
            break;
        }
    }
    
    if (minAt == 999)
    {
        自瞄.瞄准目标 = -1;
        return -1;
    }
    
    自瞄.瞄准目标 = minAt;
    if (自瞄函数[自瞄.瞄准目标].掩体部位 == 999)
    {
        自瞄.瞄准目标 = -1;
        return -1;
    }
    return minAt;
}

void 绘制::GetTouch()
{
    std::thread *触摸位置线程 = new std::thread([&]
    {
        for(;;)
        {
            usleep(1000000 / 120);
            ImGuiIO& iooi = ImGui::GetIO();
            if (自瞄.触摸位置 && iooi.MouseDown[0] && iooi.MousePos.x <= 自瞄.触摸范围X + 自瞄.触摸范围 && iooi.MousePos.y <= displayInfo.height - 自瞄.触摸范围Y + 自瞄.触摸范围 && iooi.MousePos.x >= 自瞄.触摸范围X - 自瞄.触摸范围 && iooi.MousePos.y >= displayInfo.height - 自瞄.触摸范围Y - 自瞄.触摸范围)
            {
                usleep(30000);
                if (自瞄.触摸位置 && iooi.MouseDown[0] && iooi.MousePos.x <= 自瞄.触摸范围X + 自瞄.触摸范围 && iooi.MousePos.y <= displayInfo.height - 自瞄.触摸范围Y + 自瞄.触摸范围 && iooi.MousePos.x >= 自瞄.触摸范围X - 自瞄.触摸范围 && iooi.MousePos.y >= displayInfo.height - 自瞄.触摸范围Y - 自瞄.触摸范围)
                {
                    while (自瞄.触摸位置 && iooi.MouseDown[0] && iooi.MousePos.x <= 自瞄.触摸范围X + 自瞄.触摸范围 && iooi.MousePos.y <= displayInfo.height - 自瞄.触摸范围Y + 自瞄.触摸范围 && iooi.MousePos.x >= 自瞄.触摸范围X - 自瞄.触摸范围 && iooi.MousePos.y >= displayInfo.height - 自瞄.触摸范围Y - 自瞄.触摸范围)
                    {
                        自瞄.触摸范围X = iooi.MousePos.x;
                        自瞄.触摸范围Y = displayInfo.height - iooi.MousePos.y;
                        usleep(500);
                    }
                }
            }
        }
    });
    触摸位置线程->detach();
}

std::map<uintptr_t, std::pair<D3DVector, float>> activeSmokes;

bool 绘制::自瞄触发(float 距离)
{
    if (自瞄.龙息弹) {
        if (自身数据.手持 == 106001 or 自身数据.手持 == 106002 or 自身数据.手持 == 106003 or 自身数据.手持 == 106004 or 自身数据.手持 == 106005 or 自身数据.手持 == 106010 or 自身数据.手持 == 106008 or 自身数据.手持 == 106011 or 自身数据.手持 == 106007 or 自身数据.手持 == 106009) {
            if (距离 < 自瞄.龙息弹距离限制) {
                if (自瞄.龙息弹自瞄条件 == 0) {
                    return true;
                }
            }
        }
    }
    
    if (自瞄.近战限制开关 && 自身数据.开镜 != 0 && 距离 <= 自瞄.近战限制距离) {
        return false;
    }
    
    if (自瞄.喷子自瞄)
    {
        if (识别喷子(自身数据.手持) == 1.0f)
        {
            if (距离 < 自瞄.喷子距离限制)
            {
                if (自瞄.喷子自瞄条件 == 0 or (自瞄.喷子自瞄条件 == 1 && 自身数据.开镜 != 0) or (自瞄.喷子自瞄条件 == 2 && 自身数据.喷子开火 != 0))
                {
                    if (自瞄.喷子连点)
                    {
                        clickRegion(true,自瞄.连点位置);
                    }
                    return true;
                }
            }
        }
        if (自身数据.手持 == 103009 or 自身数据.手持 == 103005 or 自身数据.手持 == 103006 or 自身数据.手持 == 103010 or 自身数据.手持 == 103004 or 自身数据.手持 == 103013 or 自身数据.手持 == 103017 or 自身数据.手持 == 103100 or 自身数据.手持 == 103014)    
        {
            if (自瞄.喷子连点)
            {
                clickRegion(true,自瞄.连点位置);
            }
            return true;
        }
    }

    if (自瞄函数[自瞄.瞄准目标].距离 > 自瞄.自瞄距离限制)
    {
        return false;
    }
    if (自瞄函数[自瞄.瞄准目标].距离 > 自瞄.腰射距离限制 && 自身数据.开火 != 0 && 自身数据.Fov > 75)
    {
        return false;
    }
    if (自瞄.狙击自瞄)
    {
        if (识别狙击(自身数据.手持) == 1.0f)
        {
            if (自身数据.开镜 != 0)
            {
                return true;
            }
        }
    }
    if (念宝.射手自瞄)
    {
        if (识别射手(自身数据.手持) == 1.0f)
        {
            if (自身数据.开镜 != 0)
            {
                return true;
            }
        }
    }
    if (念宝.弓弩自瞄)
    {
        if (识别弓弩(自身数据.手持) == 1.0f)
        {
            if (自身数据.开镜 != 0)
            {
                return true;
            }
        }
    }

    switch ((int)自瞄.自瞄条件)
    {
    case 0:
        if (自身数据.开火 != 1)
        {
            return false;
        }
        break;
    case 1:
        if (自身数据.开镜 != 1)
        {
            return false;
        }
        break;
    case 2:
        if (自身数据.开火 == 0 && 自身数据.开镜 == 0)
        {
            return false;
        }
        break;
    }
    return true;
}

static std::thread* 自瞄线程 = nullptr;
static volatile bool 自瞄运行 = false;

void 停止触摸自瞄()
{
    if (!自瞄运行) 
        return;    
    自瞄运行 = false;    
    if (自瞄线程 && 自瞄线程->joinable()) 
    {
        自瞄线程->join();
    }    
    delete 自瞄线程;
    自瞄线程 = nullptr;
}

void 绘制::自瞄主线程()
{
    if (自瞄运行) 
        return;
    
    自瞄运行 = true;

    自瞄线程 = new std::thread([&] 
    {
        bool isDown = false;
        float halfSize = 自瞄.触摸范围 / 2;
        double RandomnumberX = 自瞄.触摸范围Y, RandomnumberY = 自瞄.触摸范围X;
        double tx = 自瞄.触摸范围Y, ty = 自瞄.触摸范围X;

        if (自瞄.随机触摸点) 
        {
            RandomnumberY = 自瞄.触摸范围X - halfSize + (rand() % (int)自瞄.触摸范围);
            RandomnumberX = 自瞄.触摸范围Y - halfSize + (rand() % (int)自瞄.触摸范围);
            tx = RandomnumberX, ty = RandomnumberY;
        }

        double ScreenX, ScreenY;
        if (displayInfo.orientation == 1 || displayInfo.orientation == 3)
        {
            ScreenX = displayInfo.height;
            ScreenY = displayInfo.width;
        }
        else 
        {
            ScreenX = displayInfo.width;
            ScreenY = displayInfo.height;
        }

        double ScrXH = ScreenX / 2.0f;
        double ScrYH = ScreenY / 2.0f;
        static float TargetX = 0;
        static float TargetY = 0;
        D3DVector obj;
        float NowCoor[3];
        float zm_x, zm_y;

        int 目标血量 = 100;
        string 目标名字;
        bool 自瞄测试 = false;
        int 数率 = 0;

        timer AimFPS;
        AimFPS.SetFps(按钮.当前帧率);
        AimFPS.AotuFPS_init();
        AimFPS.setAffinity();

        for (;;)
        {
            mtx.lock();
            usleep(自瞄.自瞄速度 * 1000);
                        
            if (!自瞄.初始化)
            {
                if (isDown == true)
                {
                    tx = 自瞄.触摸范围Y, ty = 自瞄.触摸范围X;
                    
                    if (自瞄.随机触摸点) 
                    {
                        RandomnumberY = 自瞄.触摸范围X - halfSize + (rand() % (int)自瞄.触摸范围);
                        RandomnumberX = 自瞄.触摸范围Y - halfSize + (rand() % (int)自瞄.触摸范围);
                        tx = RandomnumberX, ty = RandomnumberY;
                    }

                    // 恢复变量
                    Touch_Up(8);
                    // 抬起
                    isDown = false;
                }
                
                usleep(自瞄.自瞄速度 * 1000);
                continue;
            }

            findminat();

            if (自瞄.瞄准目标 == -1)
            {
                if (isDown == true)
                {
                    tx = 自瞄.触摸范围Y, ty = 自瞄.触摸范围X;
                    
                    if (自瞄.随机触摸点) 
                    {
                        RandomnumberY = 自瞄.触摸范围X - halfSize + (rand() % (int)自瞄.触摸范围);
                        RandomnumberX = 自瞄.触摸范围Y - halfSize + (rand() % (int)自瞄.触摸范围);
                        tx = RandomnumberX, ty = RandomnumberY;
                    }

                    // 恢复变量
                    Touch_Up(8);

                    // 抬起
                    isDown = false;
                }
                
                usleep(自瞄.自瞄速度 * 1000);
                continue;
            }

            if (自瞄函数[自瞄.瞄准目标].掩体部位 == 999)
            {
                if (isDown == true)
                {
                    tx = 自瞄.触摸范围Y, ty = 自瞄.触摸范围X;
                    
                    if (自瞄.随机触摸点) 
                    {
                        RandomnumberY = 自瞄.触摸范围X - halfSize + (rand() % (int)自瞄.触摸范围);
                        RandomnumberX = 自瞄.触摸范围Y - halfSize + (rand() % (int)自瞄.触摸范围);
                        tx = RandomnumberX, ty = RandomnumberY;
                    }

                    // 恢复变量
                    Touch_Up(8);

                    // 抬起
                    isDown = false;
                }
                
                usleep(自瞄.自瞄速度 * 1000);
                continue;
            }

            float ToReticleDistance = 自瞄函数[自瞄.瞄准目标].准心距离;
            float BulletFlightTime = 自瞄函数[自瞄.瞄准目标].距离 / 自身数据.子弹速度;
            float FlyTime;
            float 预判力度a = 自瞄.预判力度;

            if (对象信息.敌人信息.乘坐载具) 
            {
                预判力度a = 预判度.扫车;
            }
            else
            {
                // 5.56
                if (自身数据.手持id == 101004 && 预判度.m416 != 0)
                    预判力度a = 预判度.m416;
                else if (自身数据.手持id == 101003 && 预判度.scar_l != 0)
                    预判力度a = 预判度.scar_l;
                else if (自身数据.手持id == 101006 && 预判度.aug != 0)
                    预判力度a = 预判度.aug;
                else if (自身数据.手持id == 101013 && 预判度.famas != 0)
                    预判力度a = 预判度.famas;
                else if (自身数据.手持id == 101010 && 预判度.g36c != 0)
                    预判力度a = 预判度.g36c;
                else if (自身数据.手持id == 105001 && 预判度.m249 != 0)
                    预判力度a = 预判度.m249;
                // 7.62
                else if (自身数据.手持id == 101001 && 预判度.akm != 0)
                    预判力度a = 预判度.akm;
                else if (自身数据.手持id == 101008 && 预判度.m762 != 0)
                    预判力度a = 预判度.m762;
                else if (自身数据.手持id == 101012 && 预判度.蜜獾 != 0)
                    预判力度a = 预判度.蜜獾;
                else if (自身数据.手持id == 105012 && 预判度.pkm != 0)
                    预判力度a = 预判度.pkm;
                else if (自身数据.手持id == 105010 && 预判度.mg3 != 0)
                    预判力度a = 预判度.mg3;
                else if (自身数据.手持id == 105013 && 预判度.mg_36 != 0)
                    预判力度a = 预判度.mg_36;
                //冲锋枪
                else if (自身数据.手持id == 102105 && 预判度.p90 != 0)
                    预判力度a = 预判度.p90;
                else if (自身数据.手持id == 102001 && 预判度.uzi != 0)
                    预判力度a = 预判度.uzi;
                else if (自身数据.手持id == 102002 && 预判度.ump45 != 0)
                    预判力度a = 预判度.ump45;
                else if (自身数据.手持id == 102003 && 预判度.vector != 0)
                    预判力度a = 预判度.vector;
                else if (自身数据.手持id == 102004 && 预判度.汤姆逊 != 0)
                    预判力度a = 预判度.汤姆逊;
                else if (自身数据.手持id == 102005 && 预判度.野牛 != 0)
                    预判力度a = 预判度.野牛;
                //射手步枪
                else if (自身数据.手持id == 103006 && 预判度.mini14 != 0)
                    预判力度a = 预判度.mini14;
                else if (自身数据.手持id == 103004 && 预判度.sks != 0)
                    预判力度a = 预判度.sks;
                else if (自身数据.手持id == 103013 && 预判度.m417 != 0)
                    预判力度a = 预判度.m417;
                else if (自身数据.手持id == 103014 && 预判度.mk20_h != 0)
                    预判力度a = 预判度.mk20_h;
                else if (自身数据.手持id == 103100 && 预判度.mk12 != 0)
                    预判力度a = 预判度.mk12;
                else if (自身数据.手持id == 103005 && 预判度.vss != 0)
                    预判力度a = 预判度.vss;
            }

            if (自瞄函数[自瞄.瞄准目标].距离 >= 40) 
            {
                FlyTime = 自瞄函数[自瞄.瞄准目标].距离 / (自身数据.子弹速度 * 0.01f) * 预判力度a;
            }
            else 
            {
                FlyTime = 自瞄函数[自瞄.瞄准目标].距离 / (自身数据.子弹速度 * 0.0055f) * 预判力度a;
            }

            float DropM = 130.0f * BulletFlightTime * BulletFlightTime;
            float 压枪力度 = 自瞄.压枪力度;
            float 蹲下压枪力度 = 念宝.蹲下压枪力度;
            float 趴下压枪力度 = 念宝.趴下压枪力度;
            float 喷子压枪力度 = 念宝.喷子压枪力度;

            // 5.56
            if (自身数据.手持id == 101004 && 压枪力.m416 != 0)
                压枪力度 = 压枪力度 * 压枪力.m416;
            else if (自身数据.手持id == 101003 && 压枪力.scar_l != 0)
                压枪力度 = 压枪力度 * 压枪力.scar_l;
            else if (自身数据.手持id == 101006 && 压枪力.aug != 0)
                压枪力度 = 压枪力度 * 压枪力.aug;
            else if (自身数据.手持id == 101013 && 压枪力.famas != 0)
                压枪力度 = 压枪力度 * 压枪力.famas;
            else if (自身数据.手持id == 101010 && 压枪力.g36c != 0)
                压枪力度 = 压枪力度 * 压枪力.g36c;
            else if (自身数据.手持id == 105001 && 压枪力.m249 != 0)
                压枪力度 = 压枪力度 * 压枪力.m249;
            else if (自身数据.手持id == 101002 && 压枪力.m416 != 0)
                压枪力度 = 压枪力度 * 压枪力.m416;
            // 7.62
            else if (自身数据.手持id == 101001 && 压枪力.akm != 0)
                压枪力度 = 压枪力度 * 压枪力.akm;
            else if (自身数据.手持id == 101008 && 压枪力.m762 != 0)
                压枪力度 = 压枪力度 * 压枪力.m762;
            else if (自身数据.手持id == 101012 && 压枪力.蜜獾 != 0)
                压枪力度 = 压枪力度 * 压枪力.蜜獾;
            else if (自身数据.手持id == 105012 && 压枪力.pkm != 0)
                压枪力度 = 压枪力度 * 压枪力.pkm;
            else if (自身数据.手持id == 105010 && 压枪力.mg3 != 0)
                压枪力度 = 压枪力度 * 压枪力.mg3;
            else if (自身数据.手持id == 105013 && 压枪力.mg_36 != 0)
                压枪力度 = 压枪力度 * 压枪力.mg_36;
            //冲锋枪
            else if (自身数据.手持id == 102105 && 压枪力.p90 != 0)
                压枪力度 = 压枪力度 * 压枪力.p90;
            else if (自身数据.手持id == 102001 && 压枪力.uzi != 0)
                压枪力度 = 压枪力度 * 压枪力.uzi;
            else if (自身数据.手持id == 102002 && 压枪力.ump45 != 0)
                压枪力度 = 压枪力度 * 压枪力.ump45;
            else if (自身数据.手持id == 102003 && 压枪力.vector != 0)
                压枪力度 = 压枪力度 * 压枪力.vector;
            else if (自身数据.手持id == 102004 && 压枪力.汤姆逊 != 0)
                压枪力度 = 压枪力度 * 压枪力.汤姆逊;
            else if (自身数据.手持id == 102005 && 压枪力.野牛 != 0)
                压枪力度 = 压枪力度 * 压枪力.野牛;
            //射手步枪
            else if (自身数据.手持id == 103006 && 压枪力.mini14 != 0)
                压枪力度 = 压枪力度 * 压枪力.mini14;
            else if (自身数据.手持id == 103004 && 压枪力.sks != 0)
                压枪力度 = 压枪力度 * 压枪力.sks;
            else if (自身数据.手持id == 103013 && 压枪力.m417 != 0)
                压枪力度 = 压枪力度 * 压枪力.m417;
            else if (自身数据.手持id == 103014 && 压枪力.mk20_h != 0)
                压枪力度 = 压枪力度 * 压枪力.mk20_h;
            else if (自身数据.手持id == 103100 && 压枪力.mk12 != 0)
                压枪力度 = 压枪力度 * 压枪力.mk12;
            else if (自身数据.手持id == 103005 && 压枪力.vss != 0)
                压枪力度 = 压枪力度 * 压枪力.vss;

            NowCoor[0] = 自瞄函数[自瞄.瞄准目标].瞄准坐标.X;
            NowCoor[1] = 自瞄函数[自瞄.瞄准目标].瞄准坐标.Y;
            NowCoor[2] = 自瞄函数[自瞄.瞄准目标].瞄准坐标.Z;

            obj.X = NowCoor[0] + (自瞄函数[自瞄.瞄准目标].人物向量.X * FlyTime);
            float y_pred = (自瞄函数[自瞄.瞄准目标].人物向量.Y * FlyTime);
            float y_gravity = (自瞄函数[自瞄.瞄准目标].人物向量.Z * 0.25f * FlyTime);
            obj.Y = NowCoor[1] + y_pred + y_gravity;
            obj.Z = NowCoor[2] + (自瞄函数[自瞄.瞄准目标].人物向量.Z * FlyTime) + DropM;

            if (自身数据.开火 == 1) 
            {
                if (自身数据.自身状态 == 16 || 自身数据.自身状态 == 17 || 自身数据.自身状态 == 19 || 
                    自身数据.自身状态 == 272 || 自身数据.自身状态 == 1040 || 自身数据.自身状态 == 1296 || 
                    自身数据.自身状态 == 4112 || 自身数据.自身状态 == 4368 || 自身数据.自身状态 == 5136 || 
                    自身数据.自身状态 == 5392 || 自身数据.自身状态 == 273 || 自身数据.自身状态 == 1041 || 
                    自身数据.自身状态 == 1297 || 自身数据.自身状态 == 4113 || 自身数据.自身状态 == 4369 || 
                    自身数据.自身状态 == 5137 || 自身数据.自身状态 == 5393 || 自身数据.自身状态 == 272 || 
                    自身数据.自身状态 == 1296 || 自身数据.自身状态 == 1297 || 自身数据.自身状态 == 273 || 
                    自身数据.自身状态 == 4368 || 自身数据.自身状态 == 4369 || 自身数据.自身状态 == 5392 || 
                    自身数据.自身状态 == 5393 || 自身数据.自身状态 == 400 || 自身数据.自身状态 == 403 || 
                    自身数据.自身状态 == 8339 || 自身数据.自身状态 == 8337 || 自身数据.自身状态 == 8336 || 
                    自身数据.自身状态 == 657 || 自身数据.自身状态 == 656 || 自身数据.自身状态 == 401 || 
                    自身数据.自身状态 == 32915 || 自身数据.自身状态 == 32912 || 自身数据.自身状态 == 147 || 
                    自身数据.自身状态 == 145 || 自身数据.自身状态 == 144)
                {
                    obj.Z -= 自瞄函数[自瞄.瞄准目标].距离 * 压枪力度 * GetWeaponId(自身数据.手持);
                }
                else if (自身数据.自身状态 == 32 || 自身数据.自身状态 == 33 || 自身数据.自身状态 == 35 || 
                         自身数据.自身状态 == 288 || 自身数据.自身状态 == 1056 || 自身数据.自身状态 == 1312 || 
                         自身数据.自身状态 == 4128 || 自身数据.自身状态 == 5152 || 自身数据.自身状态 == 5408 || 
                         自身数据.自身状态 == 289 || 自身数据.自身状态 == 1057 || 自身数据.自身状态 == 4129 || 
                         自身数据.自身状态 == 4385 || 自身数据.自身状态 == 5153 || 自身数据.自身状态 == 288 || 
                         自身数据.自身状态 == 289 || 自身数据.自身状态 == 4384 || 自身数据.自身状态 == 4385 || 
                         自身数据.自身状态 == 5408 || 自身数据.自身状态 == 5409 || 自身数据.自身状态 == 1312 || 
                         自身数据.自身状态 == 1313)
                {
                    obj.Z -= 自瞄函数[自瞄.瞄准目标].距离 * 蹲下压枪力度 * GetWeaponId(自身数据.手持);
                }
                else if (自身数据.自身状态 == 64 || 自身数据.自身状态 == 65 || 自身数据.自身状态 == 67 || 
                         自身数据.自身状态 == 1088 || 自身数据.自身状态 == 320 || 自身数据.自身状态 == 1344)
                {
                    obj.Z -= 自瞄函数[自瞄.瞄准目标].距离 * 趴下压枪力度 * GetWeaponId(自身数据.手持);
                }
            }

            if (识别喷子(自身数据.手持) == 1.0f)
            {
                obj.Z -= 自瞄函数[自瞄.瞄准目标].距离 * 喷子压枪力度 * GetWeaponId(自身数据.手持);
            }

            D2DVector vpvp = 计算.计算屏幕坐标2(自身数据.矩阵, obj, PX, PY);
            float AimDs = sqrt(pow(PX - vpvp.X, 2) + pow(PY - vpvp.Y, 2));

            if (自瞄.动态自瞄 && (自身数据.开火 == 1 || 自身数据.开镜 == 1))
            {
                自瞄.动态范围 = AimDs;
            }

            zm_y = vpvp.X;
            zm_x = ScreenX - vpvp.Y;

            if (zm_x <= 0 || zm_x >= ScreenX || zm_y <= 0 || zm_y >= ScreenY)
            {
                if (isDown == true)
                {
                    tx = 自瞄.触摸范围Y, ty = 自瞄.触摸范围X;
                    
                    if (自瞄.随机触摸点) 
                    {
                        RandomnumberY = 自瞄.触摸范围X - halfSize + (rand() % (int)自瞄.触摸范围);
                        RandomnumberX = 自瞄.触摸范围Y - halfSize + (rand() % (int)自瞄.触摸范围);
                        tx = RandomnumberX, ty = RandomnumberY;
                    }

                    // 恢复变量
                    Touch_Up(8);

                    // 抬起
                    isDown = false;
                }
                
                usleep(自瞄.自瞄速度 * 1000);
                continue;
            }

            if (ToReticleDistance <= 自瞄.自瞄范围)
            {
                if (!自瞄触发(自瞄函数[自瞄.瞄准目标].距离))
                {
                    if (isDown == true)
                    {
                        tx = 自瞄.触摸范围Y, ty = 自瞄.触摸范围X;
                        
                        if (自瞄.随机触摸点) 
                        {
                            RandomnumberY = 自瞄.触摸范围X - halfSize + (rand() % (int)自瞄.触摸范围);
                            RandomnumberX = 自瞄.触摸范围Y - halfSize + (rand() % (int)自瞄.触摸范围);
                            tx = RandomnumberX, ty = RandomnumberY;
                        }

                        Touch_Up(8);
                        
                        isDown = false;
                    }
                    
                    usleep(自瞄.自瞄速度 * 1000);
                    continue;
                }

                if (自瞄.框内自瞄) 
                {
                    if ((自瞄函数[自瞄.瞄准目标].人物向量.X == 0 && AimDs > 35) || 
                        (自瞄函数[自瞄.瞄准目标].人物向量.X != 0 && AimDs > 65))
                    {
                        if (isDown == true)
                        {
                            tx = 自瞄.触摸范围Y, ty = 自瞄.触摸范围X;
                            
                            if (自瞄.随机触摸点) 
                            {
                                RandomnumberY = 自瞄.触摸范围X - halfSize + (rand() % (int)自瞄.触摸范围);
                                RandomnumberX = 自瞄.触摸范围Y - halfSize + (rand() % (int)自瞄.触摸范围);
                                tx = RandomnumberX, ty = RandomnumberY;
                            }

                            Touch_Up(8);
                            
                            isDown = false;
                        }
                        
                        usleep(自瞄.自瞄速度 * 1000);
                        continue;
                    }
                }

                if (自瞄.掉血自瞄) 
                {
                    if (数率 > 0) 
                    {
                        数率++;
                        
                        if (数率 == (int)自瞄.掉血自瞄数率) 
                        {
                            数率 = 0;
                        }
                    }
                    else 
                    {
                        if (自瞄函数[自瞄.瞄准目标].名字 == 目标名字) 
                        {
                            if (自瞄函数[自瞄.瞄准目标].血量 >= 目标血量) 
                            {
                                目标血量 = 自瞄函数[自瞄.瞄准目标].血量;
                                continue;
                            }
                            else 
                            {
                                数率++;
                                目标血量 = 自瞄函数[自瞄.瞄准目标].血量;
                            }
                        }
                        else 
                        {
                            目标名字 = 自瞄函数[自瞄.瞄准目标].名字;
                            目标血量 = 自瞄函数[自瞄.瞄准目标].血量;
                            continue;
                        }
                    }
                }

                if (isDown == false)
                {
                    if (自瞄.充电口方向 == 0)
                    {
                        Touch_Down(8, (int)tx, (int)ty);
                    }
                    else
                    {
                        Touch_Down(8, displayInfo.height - (int)tx, displayInfo.width - (int)ty);
                    }
                    
                    isDown = true;
                }

                float Acc = getScopeAcc((int)(90 / 自身数据.Fov));

                if (zm_x > ScrXH) 
                {
                    TargetX = -(ScrXH - zm_x) / 自瞄.自瞄速度 * Acc;
                    
                    if (TargetX + ScrXH > ScrXH * 2) 
                    {
                        TargetX = 0;
                    }
                }
                else if (zm_x < ScrXH) 
                {
                    TargetX = (zm_x - ScrXH) / 自瞄.自瞄速度 * Acc;
                    
                    if (TargetX + ScrXH < 0) 
                    {
                        TargetX = 0;
                    }
                }

                if (zm_y > ScrYH) 
                {
                    TargetY = -(ScrYH - zm_y) / 自瞄.自瞄速度 * Acc;
                    
                    if (TargetY + ScrYH > ScrYH * 2) 
                    {
                        TargetY = 0;
                    }
                }
                else if (zm_y < ScrYH) 
                {
                    TargetY = (zm_y - ScrYH) / 自瞄.自瞄速度 * Acc;
                    
                    if (TargetY + ScrYH < 0) 
                    {
                        TargetY = 0;
                    }
                }

                if (TargetY >= 35 || TargetX >= 35 || TargetY <= -35 || TargetX <= -35)
                {
                    if (isDown == true)
                    {
                        tx = 自瞄.触摸范围Y, ty = 自瞄.触摸范围X;
                        
                        if (自瞄.随机触摸点) 
                        {
                            RandomnumberY = 自瞄.触摸范围X - halfSize + (rand() % (int)自瞄.触摸范围);
                            RandomnumberX = 自瞄.触摸范围Y - halfSize + (rand() % (int)自瞄.触摸范围);
                            tx = RandomnumberX, ty = RandomnumberY;
                        }

                        // 恢复变量
                        Touch_Up(8);
                        
                        isDown = false;
                    }
                    
                    usleep(自瞄.自瞄速度 * 1000);
                    continue;
                }

                tx += TargetX;
                ty += TargetY;

                if (tx >= RandomnumberX + 自瞄.触摸范围 || tx <= RandomnumberX - 自瞄.触摸范围 ||
                    ty >= RandomnumberY + 自瞄.触摸范围 || ty <= RandomnumberY - 自瞄.触摸范围)
                {
                    tx = 自瞄.触摸范围Y, ty = 自瞄.触摸范围X;
                    
                    if (自瞄.随机触摸点) 
                    {
                        RandomnumberY = 自瞄.触摸范围X - halfSize + (rand() % (int)自瞄.触摸范围);
                        RandomnumberX = 自瞄.触摸范围Y - halfSize + (rand() % (int)自瞄.触摸范围);
                        tx = RandomnumberX, ty = RandomnumberY;
                    }

                    // 恢复变量
                    Touch_Up(8);
                    // 抬起

                    if (自瞄.充电口方向 == 0)
                    {
                        Touch_Down(8, (int)tx, (int)ty);
                    }
                    else
                    {
                        Touch_Down(8, displayInfo.height - (int)tx, displayInfo.width - (int)ty);
                    }

                    isDown = true;
                }

                if (自瞄.充电口方向 == 0)
                {
                    Touch_Move(8, (int)tx, (int)ty);
                }
                else
                {
                    Touch_Move(8, displayInfo.height - (int)tx, displayInfo.width - (int)ty);
                }
                
                isDown = true;
            }
            else
            {
                if (isDown == true)
                {
                    tx = 自瞄.触摸范围Y, ty = 自瞄.触摸范围X;
                    
                    if (自瞄.随机触摸点) 
                    {
                        RandomnumberY = 自瞄.触摸范围X - halfSize + (rand() % (int)自瞄.触摸范围);
                        RandomnumberX = 自瞄.触摸范围Y - halfSize + (rand() % (int)自瞄.触摸范围);
                        tx = RandomnumberX, ty = RandomnumberY;
                    }

                    // 恢复变量
                    Touch_Up(8);

                    // 抬起
                    isDown = false;
                }
            }

            usleep(自瞄.自瞄速度 * 1000);
            AimFPS.SetFps(按钮.当前帧率);
            AimFPS.AotuFPS();
            
            mtx.unlock();
        }
    });

    自瞄线程->detach();
}

static std::thread* 陀螺仪自瞄线程 = nullptr;
static volatile bool 陀螺仪自瞄运行 = false;

void 停止陀螺仪自瞄()
{
    if (!陀螺仪自瞄运行) return;
    陀螺仪自瞄运行 = false;
    if (陀螺仪自瞄线程 && 陀螺仪自瞄线程->joinable()) {
        陀螺仪自瞄线程->join();
    }
    delete 陀螺仪自瞄线程;
    陀螺仪自瞄线程 = nullptr;    
}

void 绘制::陀螺仪自瞄主线程()
{
    if (陀螺仪自瞄运行) return;
    陀螺仪自瞄运行 = true;
    陀螺仪自瞄线程 = new std::thread([&] {
        Gyro.Init();
        double ScreenX, ScreenY;
        if(displayInfo.orientation == 1 || displayInfo.orientation == 3){
            ScreenX = displayInfo.height; 
            ScreenY = displayInfo.width;
        } else {
            ScreenX = displayInfo.width; 
            ScreenY = displayInfo.height;
        }
        double ScrXH = ScreenX / 2.0f;
        double ScrYH = ScreenY / 2.0f;
        
        static float TargetX = 0;
        static float TargetY = 0;
        
        D3DVector obj;
        float NowCoor[3];
        float zm_x, zm_y;
        
        int 目标血量 = 100;
        string 目标名字;
        int 数率 = 0;
        static bool 正在自瞄 = false;
        timer AimFPS;
        AimFPS.SetFps(按钮.当前帧率);
        AimFPS.AotuFPS_init();
        AimFPS.setAffinity();

        for (;;)
        {  
            mtx.lock();
            
            usleep(自瞄.自瞄速度 * 500);
                        
            if (!自瞄.初始化)
            {
                if (自瞄.自瞄算法 == 0 && 正在自瞄 && 自瞄.初始化) {
                    Gyro.Unlock_Event();
                    正在自瞄 = false;
                }
                usleep(自瞄.自瞄速度 * 500);
                continue;
            }

            findminat();
            if (自瞄.瞄准目标 == -1)
            {
                if (自瞄.自瞄算法 == 0 && 正在自瞄 && 自瞄.初始化) {
                    Gyro.Unlock_Event();
                    正在自瞄 = false;
                }
                usleep(自瞄.自瞄速度 * 500);
                continue;
            }

            if (自瞄函数[自瞄.瞄准目标].掩体部位 == 999)
            {
                if (自瞄.自瞄算法 == 0 && 正在自瞄 && 自瞄.初始化) {
                    Gyro.Unlock_Event();
                    正在自瞄 = false;
                }
                usleep(自瞄.自瞄速度 * 500);
                continue;
            }

            float ToReticleDistance = 自瞄函数[自瞄.瞄准目标].准心距离;
            float BulletFlightTime = 自瞄函数[自瞄.瞄准目标].距离 / 自身数据.子弹速度;
            float FlyTime;
            float 预判力度a = 自瞄.预判力度;  

            if (对象信息.敌人信息.乘坐载具) {
                预判力度a = 预判度.扫车;
            } else {
                if (自身数据.手持id == 101004 && 预判度.m416 != 0)
                    预判力度a = 预判度.m416;
                else if (自身数据.手持id == 101003 && 预判度.scar_l != 0)
                    预判力度a = 预判度.scar_l;
                else if (自身数据.手持id == 101006 && 预判度.aug != 0)
                    预判力度a = 预判度.aug;
                else if (自身数据.手持id == 101013 && 预判度.famas != 0)
                    预判力度a = 预判度.famas;
                else if (自身数据.手持id == 101010 && 预判度.g36c != 0)
                    预判力度a = 预判度.g36c;
                else if (自身数据.手持id == 105001 && 预判度.m249 != 0)
                    预判力度a = 预判度.m249;
                // 7.62
                else if (自身数据.手持id == 101001 && 预判度.akm != 0)
                    预判力度a = 预判度.akm;
                else if (自身数据.手持id == 101008 && 预判度.m762 != 0)
                    预判力度a = 预判度.m762;
                else if (自身数据.手持id == 101012 && 预判度.蜜獾 != 0)
                    预判力度a = 预判度.蜜獾;
                else if (自身数据.手持id == 105012 && 预判度.pkm != 0)
                    预判力度a = 预判度.pkm;
                else if (自身数据.手持id == 105010 && 预判度.mg3!= 0)
                    预判力度a = 预判度.mg3;
                else if (自身数据.手持id == 105013 && 预判度.mg_36 != 0)
                    预判力度a = 预判度.mg_36;
                //冲锋枪
                else if (自身数据.手持id == 102105 && 预判度.p90 != 0)
                    预判力度a = 预判度.p90;
                else if (自身数据.手持id == 102001 && 预判度.uzi != 0)
                    预判力度a = 预判度.uzi;
                else if (自身数据.手持id == 102002 && 预判度.ump45 != 0)
                    预判力度a = 预判度.ump45;
                else if (自身数据.手持id == 102003 && 预判度.vector != 0)
                    预判力度a = 预判度.vector;
                else if (自身数据.手持id == 102004 && 预判度.汤姆逊 != 0)
                    预判力度a = 预判度.汤姆逊;
                else if (自身数据.手持id == 102005 && 预判度.野牛 != 0)
                    预判力度a = 预判度.野牛;
                //射手步枪
                else if (自身数据.手持id == 103006 && 预判度.mini14 != 0)
                    预判力度a = 预判度.mini14;
                else if (自身数据.手持id == 103004 && 预判度.sks != 0)
                    预判力度a = 预判度.sks;
                else if (自身数据.手持id == 103013 && 预判度.m417 != 0)
                    预判力度a = 预判度.m417;
                else if (自身数据.手持id == 103014 && 预判度.mk20_h != 0)
                    预判力度a = 预判度.mk20_h;
                else if (自身数据.手持id == 103100&& 预判度.mk12 != 0)
                    预判力度a = 预判度.mk12;
                else if (自身数据.手持id == 103005&& 预判度.vss != 0)
                    预判力度a = 预判度.vss;
            }

            if (自瞄函数[自瞄.瞄准目标].距离 >= 40) {
                FlyTime = 自瞄函数[自瞄.瞄准目标].距离 / (自身数据.子弹速度 * 0.01f) * 预判力度a;
            } else {
                FlyTime = 自瞄函数[自瞄.瞄准目标].距离 / (自身数据.子弹速度 * 0.0055f) * 预判力度a;
            }
            float DropM = 130.0f * BulletFlightTime * BulletFlightTime;
            float 压枪力度 = 自瞄.压枪力度;
            float 蹲下压枪力度 = 念宝.蹲下压枪力度;
            float 趴下压枪力度 = 念宝.趴下压枪力度;
            float 喷子压枪力度 = 念宝.喷子压枪力度;
            // 5.56
            if (自身数据.手持id == 101004 && 压枪力.m416 != 0)
                压枪力度 = 压枪力度 * 压枪力.m416;
            else if (自身数据.手持id == 101003 && 压枪力.scar_l != 0)
                 压枪力度 = 压枪力度 * 压枪力.scar_l;
            else if (自身数据.手持id == 101006 && 压枪力.aug != 0)
                 压枪力度 = 压枪力度 * 压枪力.aug;
            else if (自身数据.手持id == 101013 && 压枪力.famas != 0)
                 压枪力度 = 压枪力度 * 压枪力.famas;
            else if (自身数据.手持id == 101010 && 压枪力.g36c != 0)
                压枪力度 = 压枪力度 * 压枪力.g36c;
            else if (自身数据.手持id == 105001 && 压枪力.m249 != 0)
                压枪力度 = 压枪力度 * 压枪力.m249;
            // 7.62
            else if (自身数据.手持id == 101001 && 压枪力.akm != 0)
                压枪力度 = 压枪力度 * 压枪力.akm;
            else if (自身数据.手持id == 101008 && 压枪力.m762 != 0)
                压枪力度 = 压枪力度 * 压枪力.m762;
            else if (自身数据.手持id == 101012 && 压枪力.蜜獾 != 0)
                压枪力度 = 压枪力度 * 压枪力.蜜獾;
            else if (自身数据.手持id == 105012 && 压枪力.pkm != 0)
                压枪力度 = 压枪力度 * 压枪力.pkm;
            else if (自身数据.手持id == 105010 && 压枪力.mg3 != 0)
                压枪力度 = 压枪力度 * 压枪力.mg3;
            else if (自身数据.手持id == 105013 && 压枪力.mg_36 != 0)
                压枪力度 = 压枪力度 * 压枪力.mg_36;
            //冲锋枪
            else if (自身数据.手持id == 102105 && 压枪力.p90 != 0)
                压枪力度 = 压枪力度 * 压枪力.p90;
            else if (自身数据.手持id == 102001 && 压枪力.uzi != 0)
                压枪力度 = 压枪力度 * 压枪力.uzi;
            else if (自身数据.手持id == 102002 && 压枪力.ump45 != 0)
                压枪力度 = 压枪力度 * 压枪力.ump45;
            else if (自身数据.手持id == 102003 && 压枪力.vector != 0)
                压枪力度 = 压枪力度 * 压枪力.vector;
            else if (自身数据.手持id == 102004 && 压枪力.汤姆逊 != 0)
                压枪力度 = 压枪力度 * 压枪力.汤姆逊;
            else if (自身数据.手持id == 102005 && 压枪力.野牛 != 0)
                压枪力度 = 压枪力度 * 压枪力.野牛;
            //射手步枪
            else if (自身数据.手持id == 103006 && 压枪力.mini14 != 0)
                压枪力度 = 压枪力度 * 压枪力.mini14;
            else if (自身数据.手持id == 103004 && 压枪力.sks != 0)
                压枪力度 = 压枪力度 * 压枪力.sks;
            else if (自身数据.手持id == 103013 && 压枪力.m417 != 0)
                压枪力度 = 压枪力度 * 压枪力.m417;
            else if (自身数据.手持id == 103014 && 压枪力.mk20_h != 0)
                压枪力度 = 压枪力度 * 压枪力.mk20_h;
            else if (自身数据.手持id == 103100 && 压枪力.mk12 != 0)
                压枪力度 = 压枪力度 * 压枪力.mk12;
            else if (自身数据.手持id == 103005 && 压枪力.vss != 0)
                压枪力度 = 压枪力度 * 压枪力.vss;
            
            NowCoor[0] = 自瞄函数[自瞄.瞄准目标].瞄准坐标.X;
            NowCoor[1] = 自瞄函数[自瞄.瞄准目标].瞄准坐标.Y;
            NowCoor[2] = 自瞄函数[自瞄.瞄准目标].瞄准坐标.Z;
            obj.X = NowCoor[0] + (自瞄函数[自瞄.瞄准目标].人物向量.X * FlyTime);
            float y_pred = (自瞄函数[自瞄.瞄准目标].人物向量.Y * FlyTime);
            float y_gravity = (自瞄函数[自瞄.瞄准目标].人物向量.Z * 0.25f * FlyTime);
            obj.Y = NowCoor[1] + y_pred + y_gravity;
            obj.Z = NowCoor[2] + (自瞄函数[自瞄.瞄准目标].人物向量.Z * FlyTime) + DropM;
            if (自身数据.开火 == 1) {
                if (自身数据.自身状态==16 || 自身数据.自身状态==17 || 自身数据.自身状态==19 || 自身数据.自身状态==272 || 自身数据.自身状态==1040 || 自身数据.自身状态==1296 || 自身数据.自身状态==4112 || 自身数据.自身状态==4368 || 自身数据.自身状态==5136 || 自身数据.自身状态==5392 || 自身数据.自身状态==273 || 自身数据.自身状态==1041 || 自身数据.自身状态==1297 || 自身数据.自身状态==4113 || 自身数据.自身状态==4369 || 自身数据.自身状态==5137 || 自身数据.自身状态==5393 || 自身数据.自身状态 == 272 || 自身数据.自身状态 == 1296 || 自身数据.自身状态 == 1297 || 自身数据.自身状态 == 273 || 自身数据.自身状态 == 4368 || 自身数据.自身状态 == 4369 || 自身数据.自身状态 == 5392 || 自身数据.自身状态 == 5393 || 自身数据.自身状态 == 400 || 自身数据.自身状态 == 403 || 自身数据.自身状态 == 8339 || 自身数据.自身状态 == 8337 || 自身数据.自身状态 == 8336 || 自身数据.自身状态 == 8336 || 自身数据.自身状态 == 657 || 自身数据.自身状态 == 656 || 自身数据.自身状态 == 401 || 自身数据.自身状态 == 32915 || 自身数据.自身状态 == 32912 || 自身数据.自身状态 == 147 || 自身数据.自身状态 == 145 || 自身数据.自身状态 == 144)
                {
                    obj.Z -= 自瞄函数[自瞄.瞄准目标].距离 * 压枪力度 * GetWeaponId(自身数据.手持);
                }
                else if (自身数据.自身状态==32 || 自身数据.自身状态==33 || 自身数据.自身状态==35 || 自身数据.自身状态==288 || 自身数据.自身状态==1056 || 自身数据.自身状态==1312 || 自身数据.自身状态==4128 || 自身数据.自身状态==5152 || 自身数据.自身状态==5408 || 自身数据.自身状态==289 || 自身数据.自身状态==1057 || 自身数据.自身状态==4129 || 自身数据.自身状态==4385 || 自身数据.自身状态==5153 || 自身数据.自身状态 == 288 || 自身数据.自身状态 == 289 || 自身数据.自身状态 == 4384 || 自身数据.自身状态 == 4385 || 自身数据.自身状态 == 5408 || 自身数据.自身状态 == 5409 || 自身数据.自身状态 == 1312 || 自身数据.自身状态 == 1313)
                {
                    obj.Z -= 自瞄函数[自瞄.瞄准目标].距离 * 蹲下压枪力度 * GetWeaponId(自身数据.手持);
                }
                else if (自身数据.自身状态==64 || 自身数据.自身状态==65 || 自身数据.自身状态==67 || 自身数据.自身状态 == 1088 || 自身数据.自身状态 == 320 || 自身数据.自身状态 == 1344)
                {
                    obj.Z -= 自瞄函数[自瞄.瞄准目标].距离 * 趴下压枪力度 * GetWeaponId(自身数据.手持);
                }
            }

            if (识别喷子(自身数据.手持) == 1.0f)
            {
                obj.Z -= 自瞄函数[自瞄.瞄准目标].距离 * 喷子压枪力度 * GetWeaponId(自身数据.手持);
            }
            

            D2DVector vpvp = 计算.计算屏幕坐标2(自身数据.矩阵, obj, PX, PY);
            float AimDs = sqrt(pow(PX - vpvp.X, 2) + pow(PY - vpvp.Y, 2));
            
            if(自瞄.动态自瞄 && (自身数据.开火==1 || 自身数据.开镜 == 1))
            {
                自瞄.动态范围 = AimDs;
            }
            
            zm_y = vpvp.X;
            zm_x = ScreenX - vpvp.Y;

            if (zm_x <= 0 || zm_x >= ScreenX || zm_y <= 0 || zm_y >= ScreenY)
            {
                if (自瞄.自瞄算法 == 0 && 正在自瞄 && 自瞄.初始化) {
                    Gyro.Unlock_Event();
                    正在自瞄 = false;
                }
                usleep(自瞄.自瞄速度 * 500);
                continue;
            }

            if (ToReticleDistance <= 自瞄.自瞄范围)
            {
                if (!自瞄触发(自瞄函数[自瞄.瞄准目标].距离))
                {
                    if (自瞄.自瞄算法 == 0 && 正在自瞄 && 自瞄.初始化) {
                        Gyro.Unlock_Event();
                        正在自瞄 = false;
                    }                 
                    usleep(自瞄.自瞄速度 * 500);
                    continue;
                }

                if (自瞄.框内自瞄) {
                    if ((自瞄函数[自瞄.瞄准目标].人物向量.X == 0 and AimDs > 35) or (自瞄函数[自瞄.瞄准目标].人物向量.X != 0 and AimDs > 65))
                    {
                        if (自瞄.自瞄算法 == 0 && 正在自瞄 && 自瞄.初始化) {
                            Gyro.Unlock_Event();
                            正在自瞄 = false;
                        }
                        usleep(自瞄.自瞄速度 * 1000);
                        continue;
                    }
                }
                
                if (自瞄.掉血自瞄) {
                    if (数率 > 0) {
                        数率++;
                        if (数率 == (int)自瞄.掉血自瞄数率) {
                            数率 = 0;
                        }
                    } else {
                        if (自瞄函数[自瞄.瞄准目标].名字 == 目标名字) {
                            if (自瞄函数[自瞄.瞄准目标].血量 >= 目标血量) {
                                目标血量 = 自瞄函数[自瞄.瞄准目标].血量;
                            } else {
                                数率++;
                                目标血量 = 自瞄函数[自瞄.瞄准目标].血量;
                            }
                        } else {
                            目标名字 = 自瞄函数[自瞄.瞄准目标].名字;
                            目标血量 = 自瞄函数[自瞄.瞄准目标].血量;
                        }
                    }
                }

                正在自瞄 = true;
                if (自瞄.自瞄算法 == 0 && 自瞄.初始化) {
                    float Acc = getScopeAcc((int)(90 / 自身数据.Fov));        
                    float dx = vpvp.X - (ScreenY / 2);
                    float dy = vpvp.Y - (ScreenX / 2);
                    dx *= Acc;
                    dy *= Acc;
                    dx *= 自瞄.陀螺仪自瞄速度;
                    dy *= 自瞄.陀螺仪自瞄速度;

                    float pitch = -dy;
                    float yaw = dx;

                    if (自瞄.充电口方向)
                    {
                        pitch = -pitch;
                        yaw = -yaw;
                    }
                    
                    if (自瞄.初始化)
                    {
                        float sendX = pitch * 0.004f;
                        float sendY = yaw * 0.004f;
                        if (displayInfo.orientation == 3)
                        {
                            sendX = -sendX;
                            sendY = -sendY;
                        }
                        Gyro.Send_Event(sendX, sendY);
                    }
                } 
            } else {
                 if (自瞄.自瞄算法 == 0 && 正在自瞄 && 自瞄.初始化) {
                    Gyro.Unlock_Event();
                    正在自瞄 = false;
                }
            }
            usleep(自瞄.自瞄速度 * 1000);
            AimFPS.SetFps(按钮.当前帧率);      
            AimFPS.AotuFPS();
        }
        if (自瞄.自瞄算法 == 0 && 正在自瞄 && 自瞄.初始化) {
            Gyro.Unlock_Event();
        }
        
        mtx.unlock();
    });
    陀螺仪自瞄线程->detach();
}

void 绘制::更新地址数据()
{    
    地址.世界地址 = 读写.getPtr64(读写.getPtr64(地址.libue4 + 0x15772758) + 0xb0);

    地址.自身地址 = 读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(地址.libue4 + 0x15772758) + 0xb8) + 0x88) + 0x30) + 0x3478);
    
    地址.矩阵地址 = 读写.getPtr64(读写.getPtr64(地址.libue4 + 0x1573BEE8) + 0x20) + 0x270;
    
    if (按钮.解密)
    {
        地址.数组地址 = 读写.getPtr64(读写.getPtr64(读写.getPtr64(读写.getPtr64(地址.libue4 + 0x141BF3F8) + 0xf8) + 0x138) + 0xf0);
        世界数量 = 读写.getDword(读写.getPtr64(读写.getPtr64(读写.getPtr64(地址.libue4 + 0x141BF3F8) + 0xf8) + 0x138) + 0xf8);
    }
    else
    {
        地址.数组地址 = 读写.getPtr64(地址.世界地址 + 0xA0);
        世界数量 = 读写.getDword(地址.世界地址 + 0xA8);
    }
    地址.类地址 = 读写.getPtr64(地址.libue4 + 0x14E33C18);
    
    读写.read(读写.getPtr64(地址.自身地址 + 0x268) + 0x200, &自身数据.坐标, sizeof(自身数据.坐标));
    
    读写.read(地址.矩阵地址, &自身数据.矩阵, sizeof(自身数据.矩阵));
    
    自身数据.自身队伍 = 读写.getDword(地址.自身地址 + 0xB78);
 //   txnet_radar_set_self((unsigned long long)地址.自身地址);
    自身数据.自身状态 = 读写.getDword(读写.getPtr64(地址.自身地址 + 0x1680));
    自身数据.开镜 = 读写.getDword(地址.自身地址 + 0x1798);
    自身数据.开火 = 读写.getDword(地址.自身地址 + 0x2608);
    
    自身数据.喷子开火 = 读写.getDword(读写.getPtr64(读写.getPtr64(地址.自身地址 + 0x1108) + 0x1f60) + 0x234);
    自身数据.手持id = 读写.getDword(读写.getPtr64(地址.自身地址 + 0x1108) + 0xDA8);
    自身数据.手持 = heldconversion(自身数据.手持id);
    自身数据.Fov = 读写.getFloat(读写.getPtr64(读写.getPtr64(地址.自身地址 + 0x5C58) + 0x660) + 0x670);
    自身数据.子弹速度 = 读写.getFloat(读写.getPtr64(读写.getPtr64(地址.自身地址 + 0x1108) + 0xC68) + 0x15CC);
    读写.read(读写.getPtr64(读写.getPtr64(地址.自身地址 + 0x5C58) + 0x660) + 0x640, &地址.PovLocation, sizeof(地址.PovLocation));
    读写.read(读写.getPtr64(读写.getPtr64(地址.自身地址 + 0x5C58) + 0x660) + 0x658, &地址.PovRotation, sizeof(地址.PovRotation));

    自身数据.准星Y = 读写.getFloat(地址.自身地址 + 0x198) - 90;
    对象地址.准星Y = 读写.getFloat(对象地址.敌人地址 + 0x198) - 90;
    
    自身数据.Controller = 读写.getPtr64(地址.自身地址 + 0x5C58);
    
    static bool 断点已设置[2] = {false, false};
    int 随机数 = dist(engine);
    bool 触发概率追踪 = (随机数 < 追踪概率限制);
    
    if (自瞄.开启追踪) 
    {
        const char* breakpointType = "";
        if (lockBase && 散弹枪(自身数据.手持) != 1.0f && 触发概率追踪 && 人物距离 <= 追踪距离限制 && 倒地不追 && 人机不追 && 自身数据.开火 == 1) 
        {
            if (!断点已设置[0] && 弹道追踪函数 != 0) 
            {
                if (读写.add_breakpoint(弹道追踪函数, 0, 4) == 0)
                {
                    InjectFunction(弹道追踪函数, (uintptr_t)hook3);
                    断点已设置[0] = true;
                    breakpointType = "硬件断点 (4字节)";
                }
            }
        } 
        else 
        {
            if (断点已设置[0] && (自身数据.开火 != 1 || !触发概率追踪)) 
            {
                if (读写.remove_breakpoint(弹道追踪函数))
                {
                    读写.WriteDword(弹道追踪函数, 1840659439);
                    读写.WriteDword(弹道追踪函数 + 0x4, 1828795373);
                    读写.WriteDword(弹道追踪函数 + 0x8, 1828858859);
                    断点已设置[0] = false;
                }
            }
        }
        if (breakpointType != "" && 断点已设置[0] && 自身数据.开火 == 1 && 触发概率追踪) {
            char szMsg[512];
            sprintf(szMsg, "断点设置成功！追踪地址: 0x%llX 断点类型: %s Hook函数: 0x%llX 状态: 已激活", 弹道追踪函数, breakpointType, (uintptr_t)hook3);
            cout << szMsg << endl;
        }
    } 
    
    if (自瞄.开启追踪) 
    {
        const char* breakpointTypet = "";
        if (lockBase && 散弹枪(自身数据.手持) == 1.0f && 触发概率追踪 && 人物距离 <= 追踪距离限制 && 倒地不追 && 人机不追 && 自身数据.开火 == 1) 
        {
            if (!断点已设置[1] && 弹道追踪函数1 != 0) 
            {
                if (读写.add_breakpoint(弹道追踪函数1, 0, 4) == 0)
                {
                    InjectFunction(弹道追踪函数1, (uintptr_t)hook3);
                    断点已设置[1] = true;
                    breakpointTypet = "硬件断点 (4字节)";
                }
            }
        } 
        else 
        {
            if (断点已设置[1] && (自身数据.开火 != 1 || !触发概率追踪)) 
            {
                if (读写.remove_breakpoint(弹道追踪函数1))
                {
                    读写.WriteDword(弹道追踪函数1, -1119459328);
                    读写.WriteDword(弹道追踪函数1 + 0x4, -1119458303);
                    读写.WriteDword(弹道追踪函数1 + 0x8, -1119457278);
                    断点已设置[1] = false;
                }
            }
        }
        if (breakpointTypet != "" && 断点已设置[1] && 自身数据.开火 == 1 && 触发概率追踪) {
            char sbMsg[512];
            sprintf(sbMsg, "断点设置成功！追踪地址: 0x%llX 断点类型: %s Hook函数: 0x%llX 状态: 已激活", 弹道追踪函数1, breakpointTypet, (uintptr_t)hook3);
            cout << sbMsg << endl;
        }
    }
}

Vec3 GetForward(const Rotator rotation) {
    float SP = sinf(rotation.Pitch * 3.1415926f / 180.0f);
    float CP = cosf(rotation.Pitch * 3.1415926f / 180.0f);
    float SY = sinf(rotation.Yaw * 3.1415926f / 180.0f);
    float CY = cosf(rotation.Yaw * 3.1415926f / 180.0f);
    return {CP * CY, CP * SY, SP};
}

Vec2 绘制::WorldToScreen(Vec3 obj)
{
    Vec2 bscreen;
    float camear = 自身数据.矩阵[3] * obj.x + 自身数据.矩阵[7] * obj.y + 自身数据.矩阵[11] * obj.z + 自身数据.矩阵[15];
    bscreen.x = PX + (自身数据.矩阵[0] * obj.x + 自身数据.矩阵[4] * obj.y + 自身数据.矩阵[8] * obj.z + 自身数据.矩阵[12]) / camear * PX;
    bscreen.y = PY - (自身数据.矩阵[1] * obj.x + 自身数据.矩阵[5] * obj.y + 自身数据.矩阵[9] * obj.z + 自身数据.矩阵[13]) / camear * PY;
    return bscreen;
}

void drawMesh(TriangleMeshData* mesh, ImDrawList* Draw) {
    if (!mesh) return;

    for (auto i = 0; i < mesh->Indices.size(); i += 3) {

        const auto v0 = mesh->Vertices[mesh->Indices[i]];
        const auto v1 = mesh->Vertices[mesh->Indices[i + 1]];
        const auto v2 = mesh->Vertices[mesh->Indices[i + 2]];

        Vec2 s0 = 绘制.WorldToScreen(Vec3(v0.x, v0.y, v0.z));
        Vec2 s1 = 绘制.WorldToScreen(Vec3(v1.x, v1.y, v1.z));
        Vec2 s2 = 绘制.WorldToScreen(Vec3(v2.x, v2.y, v2.z));

        if (s0.x > 0 && s0.x < abs_ScreenX && s0.y > 0 && s0.y < abs_ScreenY || s1.x > 0 && s1.x < abs_ScreenX && s1.y > 0 && s1.y < abs_ScreenY || s2.x > 0 && s2.x < abs_ScreenX && s2.y > 0 && s2.y < abs_ScreenY) {
            Draw->AddTriangle(ImVec2(s0.x, s0.y), ImVec2(s1.x, s1.y),  ImVec2(s2.x, s2.y), ImColor(255, 255, 255, 80), 0.2f);
        }
    }
}

void DrawESP(ImDrawList *Draw) {
    if (绘制.按钮.模型绘制) {
        Vec3 Forward = GetForward(绘制.地址.PovRotation);
        Vec3 End = Vec3(绘制.地址.PovLocation.X, 绘制.地址.PovLocation.Y, 绘制.地址.PovLocation.Z) + Forward * 60000.0f;
        physx::PxVec3 origin(绘制.地址.PovLocation.X, 绘制.地址.PovLocation.Y, 绘制.地址.PovLocation.Z);
        physx::PxVec3 target(End.x, End.y, End.z);

        if (DynamicLoadScene) {
            auto rayHit = DynamicLoadScene->Raycast(origin, target);
            if (rayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
                auto HitMesh = DynamicLoadScene->GetGeomeoryData(rayHit.hit.geomID);
                drawMesh(HitMesh, Draw);
            }
        }
        if (DynamicRigidScene) {
            auto rayHit = DynamicRigidScene->Raycast(origin, target);
            if (rayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
                auto HitMesh = DynamicRigidScene->GetGeomeoryData(rayHit.hit.geomID);
                drawMesh(HitMesh, Draw);
            }
        }
    }
}

ImColor 绘制::floatArrToImColor(ImColor color) {
    return color;
}

void 绘制::更新对象数据()
{
    uintptr_t tempBase = 0;
    uintptr_t tempDis = 99999999;
    if (自瞄.触摸位置)
    {
        绘图.绘制自瞄触摸范围(自瞄.触摸范围, 自瞄.触摸范围X, 自瞄.触摸范围Y);
    }
    if (!自瞄.动态自瞄)
    {
        自瞄.动态范围 = 自瞄.自瞄范围;
    }
    if (自瞄.初始化 && !自瞄.隐藏自瞄圈)
    {
        ImDrawList* drawList = ImGui::GetForegroundDrawList();
        float radius = 自瞄.动态范围;
        ImVec2 center = {PX, PY};
        drawList->AddCircle(center, radius + 1.0f, ImColor(0, 0, 0, 255), 0, 2.25f);
        drawList->AddCircle(center, radius, ImColor(255, 255, 255, 255), 0, 2.0f);
    }
    if (自瞄.开启追踪 && 自瞄.显示范围) {
        ImGui::GetForegroundDrawList()->AddCircle({PX, PY}, 追踪范围限制, ImColor(255,0,0,255), 0, 2.0f);
    }
    if (按钮.雷达)
    {
        ImVec2 center = {按钮.雷达X, 按钮.雷达Y};
        float radius = 150.0f * 念宝.雷达大小 / 150.0f * 念宝.雷达大致;
        static float flowOffset = 0.0f;
        static float starTime = 0.0f;
        ImDrawList* drawList = ImGui::GetForegroundDrawList();
    
        ImColor col_void = ImColor(5, 5, 10, 180);
        ImColor col_holy_gold = ImColor(255, 215, 0, 255);
        ImColor col_star = ImColor(255, 250, 240, 255);
        ImColor col_flow_dark = ImColor(15, 10, 25, 220);
    
        drawList->AddCircleFilled(center, radius, col_void);
        
        starTime += 0.01f;
        for (int i = 0; i < 45; i++) {
            float sAlpha = (sinf(starTime + i * 0.5f) * 0.5f + 0.5f) * 0.7f;
            float r = (sinf(i * 123.0f) * 0.5f + 0.5f) * radius * 0.95f;
            float a = i * 2.399f;
            drawList->AddCircleFilled({center.x + cosf(a) * r, center.y + sinf(a) * r}, (i % 5 == 0) ? 1.3f : 0.8f, ImColor(255, 250, 240, (int)(200 * sAlpha)));
        }
    
        flowOffset += 0.025f;
        for (int i = 0; i < 3; i++) {
            float start_a = flowOffset + (i * IM_PI * 0.66f);
            drawList->PathArcTo(center, radius + 2.5f, start_a, start_a + 1.5f, 40);
            drawList->PathStroke(col_flow_dark, false, 4.0f);
            drawList->PathArcTo(center, radius + 2.5f, start_a, start_a + 1.5f, 40);
            drawList->PathStroke(ImColor(255, 215, 0, 80), false, 1.0f);
        }
    
        float cSz = 8.0f;
        drawList->AddLine({center.x - cSz, center.y}, {center.x + cSz, center.y}, col_holy_gold, 1.5f);
        drawList->AddLine({center.x, center.y - cSz}, {center.x, center.y + cSz}, col_holy_gold, 1.5f);
        drawList->AddCircle(center, 4.5f, col_star, 0, 1.0f);
    
        float vAng = IM_PI * 自身数据.准星Y / 180.0f; 
        const char* dirs[] = {"S","SE","E","NE","N","NW","W","SW"};
        ImFont* font = ImGui::GetFont();
        float fSize = 19.0f;
    
        float maxTextWidth = 0.0f;
        for (int i = 0; i < 8; i += 2) {
            float w = font->CalcTextSizeA(fSize, FLT_MAX, 0.0f, dirs[i]).x;
            if (w > maxTextWidth) maxTextWidth = w;
        }
        float uniformRingRadius = maxTextWidth * 0.85f;
    
        for (int i = 0; i < 8; i++) {
            float a = (IM_PI * 2.0f * i / 8.0f) - (IM_PI / 2.0f) + vAng;
            bool isMainDir = (i % 2 == 0);
            ImColor cTheme = isMainDir ? col_holy_gold : ImColor(160, 160, 160, 180);
            
            ImVec2 tP = { center.x + cosf(a) * (radius + 28.0f), center.y + sinf(a) * (radius + 28.0f) };
            ImVec2 tS = font->CalcTextSizeA(fSize, FLT_MAX, 0.0f, dirs[i]);
            
            drawList->AddLine({center.x + cosf(a) * radius, center.y + sinf(a) * radius}, 
                              {center.x + cosf(a) * (radius + (isMainDir ? 10.0f : 5.0f)), center.y + sinf(a) * (radius + (isMainDir ? 10.0f : 5.0f))}, 
                              cTheme, isMainDir ? 2.5f : 1.2f);
    
            if (isMainDir) {
                float tFlow = flowOffset * 1.8f;
                drawList->AddCircle(tP, uniformRingRadius, ImColor(10, 5, 20, 200), 0, 2.5f);
                drawList->PathArcTo(tP, uniformRingRadius, tFlow, tFlow + 1.2f, 15);
                drawList->PathStroke(col_holy_gold, false, 1.2f);
            }
    
            drawList->AddText(font, fSize, {tP.x - tS.x * 0.5f, tP.y - tS.y * 0.5f}, cTheme, dirs[i]);
        }
    }
   
    int 绘制人机 = 0, 绘制真人 = 0, SmokeNum = 0;
    被瞄准对象数量 = 0;
    自瞄.瞄准对象数量 = 0;
    
    char mytemp[648];
    读写.getUTF8(mytemp, 读写.getPtr64(地址.自身地址 + 0xae8));
    自身数据.名字 = mytemp;

  /*  txnet_radar_set_game((unsigned long long)地址.世界地址, 1, -1);
    if (地址.自身地址 != 0) {
        txnet_radar_item_pubgmhd self_item = {};
        self_item.type = "player";
        self_item.address = (unsigned long long)地址.自身地址;
        self_item.name = 自身数据.名字.c_str();
        self_item.team = 自身数据.自身队伍;
        self_item.pos_x = 自身数据.坐标.X;
        self_item.pos_y = 自身数据.坐标.Y;
        self_item.pos_z = 自身数据.坐标.Z;
        txnet_radar_add_item(&self_item);
    }*/

    for (int a = 0; a < 世界数量; a++)
    {
        if (按钮.解密)
        {
            对象地址.敌人地址 = 读写.getPtr64(读写.getPtr64(地址.数组地址 + a * 0x10) + 0x20);
        }
        else
        {
            对象地址.敌人地址 = 读写.getPtr64(地址.数组地址 + a * 8);
        }
        
        // uintptr_t 相机T = 读写.getPtr64(读写.getPtr64(对象地址.敌人地址 + 0x5C58) + 0x660);
        // 读写.read(读写.getPtr64(相机T + 0x680), &对象信息.敌人信息.坐标, sizeof(对象信息.敌人信息.坐标)); // 更新相机坐标
        读写.read(读写.getPtr64(对象地址.敌人地址 + 0x268) + 0x200, &对象信息.敌人信息.坐标, sizeof(对象信息.敌人信息.坐标)); // 更新坐标
        对象信息.敌人信息.距离 = 计算.计算距离(自身数据.坐标, 对象信息.敌人信息.坐标);

        float camear_r = 自身数据.矩阵[3] * 对象信息.敌人信息.坐标.X + 自身数据.矩阵[7] * 对象信息.敌人信息.坐标.Y + 自身数据.矩阵[11] * 对象信息.敌人信息.坐标.Z + 自身数据.矩阵[15];
        float r_x = PX + (自身数据.矩阵[0] * 对象信息.敌人信息.坐标.X + 自身数据.矩阵[4] * 对象信息.敌人信息.坐标.Y + 自身数据.矩阵[8] * 对象信息.敌人信息.坐标.Z + 自身数据.矩阵[12]) / camear_r * PX;
        float r_y = PY - (自身数据.矩阵[1] * 对象信息.敌人信息.坐标.X + 自身数据.矩阵[5] * 对象信息.敌人信息.坐标.Y + 自身数据.矩阵[9] * (对象信息.敌人信息.坐标.Z - 5) + 自身数据.矩阵[13]) / camear_r * PY;
        float r_z = PY - (自身数据.矩阵[1] * 对象信息.敌人信息.坐标.X + 自身数据.矩阵[5] * 对象信息.敌人信息.坐标.Y + 自身数据.矩阵[9] * (对象信息.敌人信息.坐标.Z + 205) + 自身数据.矩阵[13]) / camear_r * PY;
        D4DVector t_屏幕坐标 = {r_x - (r_y - r_z) / 4, r_y, (r_y - r_z) / 2, r_y - r_z};
        float X = r_x - (r_y - r_z) / 4;
        float Y = r_y;
        float W = (r_y - r_z) / 2;
        float MIDDLE = X + W / 2;
        float BOTTOM = Y + W;
        float TOP = Y - W;
        char 计算地址[656] = "阿念大王WoW";
        sprintf(计算地址, "%lx", 对象地址.敌人地址);

        if (按钮.手雷预警)
        {
            int 手雷ID = 读写.getDword(对象地址.敌人地址 + 0x794);
            const char *投掷物信息 = Getagrenade(手雷ID);
            自身数据.投掷 = 手雷投掷物(手雷ID);
            if (手雷ID == 602004 or 手雷ID == 9825004)
            {
                if (!计时器.hasTimer(计算地址))
                {
                    手雷类.add(计算地址, 对象信息.敌人信息.坐标.X, 对象信息.敌人信息.坐标.Y);
                }
                else
                {
                    手雷类.remove(计算地址);
                }
            }
            if (投掷物信息 != nullptr)
            {
                std::string name = 投掷物信息;
                if (t_屏幕坐标.W > 0)
                {
                    float 计算_max = 7 - (计时器.getTimerSeconds(计算地址) / 7 * (60 / ImGui::GetIO().Framerate * 0.115));
                    绘图::VecTor3 坐标 = {对象信息.敌人信息.坐标.X, 对象信息.敌人信息.坐标.Y, 对象信息.敌人信息.坐标.Z};
                    if (计算_max >= 0)
                    {
                        name += "[" + std::to_string((int)对象信息.敌人信息.距离) + "米]";
                        auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 35);
                        if (手雷ID != 602004 && 手雷ID != 9825004)
                        {
                            ImGui::GetForegroundDrawList()->AddText(NULL, 22, {r_x - (textSize.x / 2), r_y + 30}, ImColor(255, 0, 0, 255), name.c_str());
                        }
                        if (手雷ID == 602004 or 手雷ID == 9825004)
                        {
                            float 计时 = 计算_max / 7 * 100;
                            float aa = 计时 * 3.6;

                            string 计算 = std::to_string((int)计算_max);
                            计算 += "s";
                            textSize = ImGui::CalcTextSize(计算.c_str(), 0, 22);
                            ImGui::GetForegroundDrawList()->AddText(NULL, 22, {r_x - (textSize.x / 2), r_y - 22}, ImColor(255, 0, 0, 255), 计算.c_str());
                            
                            if (念宝.手雷圆圈)
                            {
                                绘图.ExplosionRange(对象信息.敌人信息.坐标, ImColor(255, 0, 0), 210, 1.5f, 自身数据.矩阵);
                            }
    
                            static std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now();
                            auto currentTime = std::chrono::steady_clock::now();
                            float elapsedTime = std::chrono::duration<float>(currentTime - lastTime).count();
    
                            float blinkFrequency = 5.0f + (7.0f - 计算_max) * 3.0f;
                            bool shouldDraw = true;
    
                            if (计算_max < 3.0f)
                            {
                                float blinkPhase = std::sin(elapsedTime * blinkFrequency * 2 * 3.14159f);
                                shouldDraw = blinkPhase > 0;
                            }
    
                            ImColor rangeColor;
                            if (计算_max > 5.0f)
                            {
                                rangeColor = ImColor(0, 255, 0, 255);
                            }
                            else if (计算_max > 3.0f)
                            {
                                float ratio = (计算_max - 3.0f) / 2.0f;
                                int red = static_cast<int>(255 * (1 - ratio));
                                rangeColor = ImColor(red, 255, 0, 255);
                            }
                            else if (计算_max > 1.0f)
                            {
                                float ratio = (计算_max - 1.0f) / 2.0f;
                                int green = static_cast<int>(255 * ratio);
                                rangeColor = ImColor(255, green, 0, 255);
                            }
                            else
                            {
                                int alpha = shouldDraw ? 255 : 100;
                                rangeColor = ImColor(255, 0, 0, alpha);
                            }
    
                            if (shouldDraw || 计算_max >= 3.0f)
                            {
                                if (念宝.手雷圆圈)
                                {
                                    绘图.ExplosionRange(对象信息.敌人信息.坐标, rangeColor, 400, 2.5f, 自身数据.矩阵);
                                }
                            }
        
                            if (elapsedTime > 1.0f / 60.0f)
                            {
                                lastTime = currentTime;
                            }
                            
                            if (念宝.手雷轨迹)
                            {
                                绘图.Parabola(坐标, 自身数据.矩阵);
                            }
                        }
                        if (手雷ID == 602001 or 手雷ID == 9825001)
                        {
                            if (念宝.手雷圆圈)
                            {
                                绘图.ExplosionRange(对象信息.敌人信息.坐标, ImColor(255, 255, 255), 180, 1.5f, 自身数据.矩阵);
                            }
                            if (念宝.手雷轨迹)
                            {
                                绘图.Parabola(坐标, 自身数据.矩阵);
                            }
                        }
                        if (手雷ID == 602003 or 手雷ID == 9825003)
                        {
                            if (念宝.手雷圆圈)
                            {
                                绘图.ExplosionRange(对象信息.敌人信息.坐标, ImColor(255, 0, 255), 485, 1.5f, 自身数据.矩阵);
                            }
                            if (念宝.手雷轨迹)
                            {
                                绘图.Parabola(坐标, 自身数据.矩阵);
                            }
                        }                        
                        if (手雷ID == 602075)
                        {
                            if (念宝.手雷圆圈)
                            {
                                绘图.ExplosionRange(对象信息.敌人信息.坐标, ImColor(128, 0, 128), 80, 1.5f, 自身数据.矩阵);
                            }
                            if (念宝.手雷轨迹)
                            {
                                绘图.Parabola(坐标, 自身数据.矩阵);
                            }
                        }
                    }
                }
            }

            if (按钮.自救倒计时)
            {
                string a = GetHol(对象信息.敌人信息.状态);
                char 计算地址[256];
                sprintf(计算地址, "%lx", 对象信息.敌人信息.状态);
                if (对象信息.敌人信息.状态 == 131104 || 对象信息.敌人信息.状态 == 655360 || 对象信息.敌人信息.状态 == 131105)
                {
                    auto it = 自救Timers.find(计算地址);
                    if (it != 自救Timers.end())
                    {
                        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - it->second).count();
                        if (elapsed > 6)
                        {
                            自救Timers[计算地址] = std::chrono::steady_clock::now();
                        }
                    }
                    else
                    {
                        自救Timers[计算地址] = std::chrono::steady_clock::now();
                    }

                    auto &timer = 自救Timers[计算地址];
                    int 剩余时间 = 10 - std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - timer).count();

                    if (剩余时间 >= 0 && t_屏幕坐标.W >= 0)
                    {
                        绘图.绘制自救(剩余时间);
                    }
                }
                else
                {
                    auto it = 自救Timers.find(计算地址);
                    if (it != 自救Timers.end())
                    {
                        自救Timers.erase(it);
                    }
                }
            }
            更新自救倒计时();
        }

        char ClassName[64] = "";
        char 对象信息_max[200] = "";
        int ClassID = 读写.getPtr64(对象地址.敌人地址 + 24);
        long int FNameEntry;

        if (t_屏幕坐标.W > 0)
        {
            FNameEntry = 读写.getPtr64(读写.getPtr64(地址.类地址 + (ClassID / 0x4000) * 0x8) + (ClassID % 0x4000) * 0x8);
            读写.read(FNameEntry + 0xC, ClassName, 64);
            if (按钮.Debug)
            {
                if (自瞄.瞄准目标 != -1)
                {
                    char classInfo[256];
                    snprintf(classInfo, sizeof(classInfo), "类名: %s\nID: %d", ClassName, ClassID);
                    auto textSize = ImGui::CalcTextSize(classInfo, 0, 20);
                    ImGui::GetForegroundDrawList()->AddText(NULL, 20, {r_x - (textSize.x / 2), r_y}, ImColor(255, 255, 255, 255), classInfo);
        
                    bool 重复啦宝宝 = false;
                    FILE* checkFile = fopen("./阿念の开发者输出.txt", "r");
                    if (checkFile != nullptr)
                    {
                        char line[256];
                        char existingClassName[128];
                        int existingID;
            
                        while (fgets(line, sizeof(line), checkFile))
                        {
                            if (sscanf(line, "%127s %d", existingClassName, &existingID) == 2)
                            {
                                if (strcmp(existingClassName, ClassName) == 0 && existingID == ClassID)
                                {
                                    重复啦宝宝 = true;
                                    break;
                                }
                            }
                        }
                        fclose(checkFile);
                    }
        
                    if (!重复啦宝宝)
                    {
                        FILE* file = fopen("./阿念の开发者输出.txt", "a");
                        if (file != nullptr)
                        {
                            fprintf(file, "%s %d\n", ClassName, ClassID);
                            fclose(file);
                        }
                    }
                }
            }
            
            if (按钮.盒内物资 && (读写.getDword(读写.getPtr64(对象地址.敌人地址 + 0xd98)) > 0 && 读写.getDword(读写.getPtr64(对象地址.敌人地址 + 0xd98)) < 1000)) {
                float Aimatdistance = sqrt(pow(PX - r_x, 2) + pow(PY - r_y, 2));
                if (Aimatdistance < 50 && 自瞄.瞄准目标 == -1) {
                    int 盒内物资数量 = 读写.getDword(对象地址.敌人地址 + 0xd98 + 0x8);
                    long int 物资数组 = 读写.getPtr64(对象地址.敌人地址 + 0xd98) + 0x4;
                    int 文本高度 = 50;
                    int 阿念控显 = min(盒内物资数量, 15);
                    for (int i = 0; i < 阿念控显; i++) {
                        int 物资地址ID = 读写.getDword(物资数组 + 0x38 * i);
                        int 物资地址数量 = 读写.getDword((物资数组 + 0x38 * i) + 0x14);
                        std::string name = GetItems(物资地址ID);
                        if (name != "NULL") {
                            name += "[" + to_string(物资地址数量) + "]";
                            auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 20);
                            文本高度 += 25;

                            ImGui::GetBackgroundDrawList()->AddText(NULL, 20, {(float)(r_x - (textSize.x / 2) - 0.1), (float)(r_y - 文本高度 - 0.1)}, ImColor(0, 0, 0, 255), name.c_str());
                            ImGui::GetBackgroundDrawList()->AddText(NULL, 20, {(float)(r_x - (textSize.x / 2) + 0.1), (float)(r_y - 文本高度 + 0.1)}, ImColor(0, 0, 0, 255), name.c_str());
                            ImGui::GetForegroundDrawList()->AddText(NULL, 20, {r_x - (textSize.x / 2), r_y - 文本高度}, 绘制::floatArrToImColor(绘制::物资颜色), name.c_str());
                        }
                    }
                    
                    if (盒内物资数量 > 15) {
                        std::string moreInfo = "还有 " + to_string(盒内物资数量 - 15) + " 个物品";
                        auto textSize = ImGui::CalcTextSize(moreInfo.c_str(), 0, 20);
                        文本高度 += 25;
            
                        ImGui::GetBackgroundDrawList()->AddText(NULL, 20, {(float)(r_x - (textSize.x / 2) - 0.1), (float)(r_y - 文本高度 - 0.1)}, ImColor(0, 0, 0, 255), moreInfo.c_str());
                        ImGui::GetBackgroundDrawList()->AddText(NULL, 20, {(float)(r_x - (textSize.x / 2) + 0.1), (float)(r_y - 文本高度 + 0.1)}, ImColor(0, 0, 0, 255), moreInfo.c_str());
                        ImGui::GetForegroundDrawList()->AddText(NULL, 20, {r_x - (textSize.x / 2), r_y - 文本高度}, 绘制::floatArrToImColor(绘制::物资颜色), moreInfo.c_str());
                    }
                }
            }

            if (按钮.车辆)
            {
                long long VehicleData = 读写.getPtr64(对象地址.敌人地址 + 0xbd8);
                float 载具血量 = 读写.getFloat(VehicleData + 0x1f0) / 读写.getFloat(VehicleData + 0x1ec) * 100;
                float 载具油量 = 读写.getFloat(VehicleData + 0x214) / 读写.getFloat(VehicleData + 0x210) * 100;
        
                if ((int)载具血量 != 0 && (int)载具油量 != 0 && 载具油量 <= 100 && 载具血量 <= 100 && 载具油量 >= 0 && 载具血量 >= 0)
                {
                    std::string Vehicle = "\uE97C";
                    std::string name = getMaterialName(ClassName);
                    if (name != "Error" && 对象信息.敌人信息.距离 > 5)
                    {
                        name += std::to_string((int)对象信息.敌人信息.距离) + "米";
                   /*     {
                            txnet_radar_item_pubgmhd veh = {};
                            veh.type = "vehicle";
                            veh.address = (unsigned long long)对象地址.敌人地址;
                            veh.uid = NULL;
                            veh.name = name.c_str();
                            veh.team = 0;
                            veh.pos_x = 对象信息.敌人信息.坐标.X;
                            veh.pos_y = 对象信息.敌人信息.坐标.Y;
                            veh.pos_z = 对象信息.敌人信息.坐标.Z;
                            veh.health = 载具血量;
                            veh.signal = TXNET_RADAR_NO_VALUE;
                            veh.fuel = 载具油量;
                            veh.rotator = TXNET_RADAR_NO_VALUE;
                            veh.pitch = 0;
                            veh.state = 0;
                            veh.is_bot = false;
                            veh.in_vehicle = false;
                            veh.distance = 对象信息.敌人信息.距离;
                            veh.weapon = 0;
                            veh.weapon_name = NULL;
                            veh.current_ammo = TXNET_RADAR_NO_VALUE;
                            veh.max_ammo = TXNET_RADAR_NO_VALUE;
                            txnet_radar_add_item(&veh);
                        }*/
                        auto nameSize = ImGui::CalcTextSize(name.c_str(), 0, 20);                      
                        auto healthSize = ImGui::CalcTextSize("血量: 100%", 0, 40);
                        auto fuelSize = ImGui::CalcTextSize("油量: 100%", 0, 40);            
                        auto iconSize = YuanerSet->icon->CalcTextSizeA(25, FLT_MAX, 0.0f, Vehicle.c_str());
                        float maxTextWidth = std::max({nameSize.x + iconSize.x + 2, healthSize.x, fuelSize.x});                       
                        float centerX = r_x - (maxTextWidth / 2);            
                        ImColor color = ImColor(255,255,255,255);

                        float iconX = centerX;
                        float iconY = r_y;
                        ImGui::GetForegroundDrawList()->AddText(YuanerSet->icon, 25, {iconX, iconY}, color, Vehicle.c_str());
            
                        float nameX = iconX + iconSize.x + 2;
                        ImGui::GetForegroundDrawList()->AddText(NULL, 20, {nameX, r_y}, color, name.c_str());
                
                        ImGuiIO &io = ImGui::GetIO();
                        ImFont *smallFont = io.Fonts->Fonts[0];
                        float smallFontSize = 16.0f * 1.20f;
    
                        if (按钮.车辆血量)
                        {
                            std::stringstream 血量文本;
                            血量文本 << "血量: " << (int)载具血量 << "%";
                            ImVec2 healthTextSize = smallFont->CalcTextSizeA(smallFontSize, FLT_MAX, 0.0f, 血量文本.str().c_str());      
                            ImGui::GetForegroundDrawList()->AddText(smallFont, smallFontSize, {centerX, r_y + 25}, ImColor(220, 6, 230), 血量文本.str().c_str());
                        }
                        if (按钮.车辆油量)
                        {
                            std::stringstream 油量文本;
                            油量文本 << "油量: " << (int)载具油量 << "%";
                            ImVec2 fuelTextSize = smallFont->CalcTextSizeA(smallFontSize, FLT_MAX, 0.0f, 油量文本.str().c_str());
                            ImGui::GetForegroundDrawList()->AddText(smallFont, smallFontSize, {centerX, r_y + 40}, ImColor(220, 6, 230), 油量文本.str().c_str());
                        }
                    }
                }
            }
            
            if (自瞄.烟雾不描) {
                if (strstr(ClassName,"ProjSmoke") != nullptr)
                {
                    SmokePos[SmokeNum] = 对象信息.敌人信息.坐标;
                    SmokeNum++;
                }
            }
            
            {
                int MaterialID = 读写.getDword(对象地址.敌人地址 + 0x714);
                std::string name = getBoxName(MaterialID);
                if (name != "Error") {
                    name += "[";
                    name += std::to_string((int)对象信息.敌人信息.距离);
                    name += "米]";
                    auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 23);
                    ImColor textColor = ImColor(255, 255, 255, 255);
                    if (MaterialID == 202004) {
                        textColor = ImColor(224, 153, 255, 255);
                    }
                    if (MaterialID == 101004) {
                        textColor = ImColor(224, 153, 255, 255);
                    }
                    if (MaterialID == 203015) {
                        textColor = ImColor(224, 153, 255, 255);
                    }
                    ImGui::GetForegroundDrawList()->AddText(NULL, 23, {r_x - (textSize.x / 2), r_y}, textColor, name.c_str());
                }
            }

            if (按钮.超级物资箱 && (strstr(ClassName, "MilitarySupplyBoxBase_Baltic_Theme_C") != 0 or strstr(ClassName, "EscopeBox_SpeEffect_C") != 0) or strstr(ClassName, "MilitarySupplyBoxBase_BaltieClassic_C") != 0) {
                std::string name = "超级物资箱[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 25);
                ImGui::GetForegroundDrawList()->AddText(NULL, 25, {r_x - (textSize.x / 2), r_y}, ImColor(230, 126, 34, 255), name.c_str());
            }
            if (按钮.密库钥匙 && (strstr(ClassName, "BP_PickupWrapper_Key_C") != 0 or strstr(ClassName, "BP_PickwpWrapper_Key_C") != 0))  {
                std::string name = "密室钥匙[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 25);
                ImGui::GetForegroundDrawList()->AddText(NULL, 25, {r_x - (textSize.x / 2), r_y}, ImColor(230, 126, 34, 255), name.c_str());
            }
            
            if (按钮.自救器 && (strstr(ClassName, "BP_PickupWrapper_Popsicle1_C") != 0 or strstr(ClassName, "BP_revivalAED_Pickup_C") != 0))  {
                std::string name = "自救器[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 25);
                ImGui::GetForegroundDrawList()->AddText(NULL, 25, {r_x - (textSize.x / 2), r_y}, ImColor(230, 126, 34, 255), name.c_str());
            }

            if (按钮.绘制信号枪 && (strstr(ClassName, "BP_Pistol_Flaregun_C") != 0 or strstr(ClassName, "BP_Pistol_Flaregun_10000_C") != 0 or strstr(ClassName, "BP_Pistol_Flaregun_Wrapper_C") != 0))  {
                std::string FlareGun = "\uE97B";
                std::string name = "信号枪[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";
    
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 25);
                auto iconSize = YuanerSet->icon->CalcTextSizeA(25, FLT_MAX, 0.0f, FlareGun.c_str());
    
                float totalWidth = iconSize.x + 2 + textSize.x;
                float centerX = r_x - (totalWidth / 2);
    
                float iconX = centerX;
                float iconY = r_y;
                ImGui::GetForegroundDrawList()->AddText(YuanerSet->icon, 25, {iconX, iconY}, ImColor(237, 152, 116, 255), FlareGun.c_str());
    
                float textX = iconX + iconSize.x + 2;
                ImGui::GetForegroundDrawList()->AddText(NULL, 25, {textX, r_y}, ImColor(237, 152, 116, 255), name.c_str());
            }

            if (按钮.绘制召回信号枪 && (strstr(ClassName, "Pistol_RevivalFlaregun") != 0 or strstr(ClassName, "Pistol_RevivalFlaregun_Wrapper") != 0))  {
                std::string FlareGun = "\uE97B";
                std::string name = "召回枪[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";
    
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 25);
                auto iconSize = YuanerSet->icon->CalcTextSizeA(25, FLT_MAX, 0.0f, FlareGun.c_str());
    
                float totalWidth = iconSize.x + 2 + textSize.x;
                float centerX = r_x - (totalWidth / 2);
    
                float iconX = centerX;
                float iconY = r_y;
                ImGui::GetForegroundDrawList()->AddText(YuanerSet->icon, 25, {iconX, iconY}, ImColor(230, 126, 34, 200), FlareGun.c_str());
    
                float textX = iconX + iconSize.x + 2;
                ImGui::GetForegroundDrawList()->AddText(NULL, 25, {textX, r_y}, ImColor(230, 126, 34, 200), name.c_str());
            }
            
            if (按钮.绘制金插 && strstr(ClassName, "PeopleSkill") != 0) {
                std::string name = "金色插件[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "米]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 25);
                ImGui::GetForegroundDrawList()->AddText(NULL, 25, {r_x - (textSize.x / 2), r_y}, ImColor(230, 126, 34, 255), name.c_str());
            }

            if (按钮.盒子 && (strstr(ClassName, "CharacterDeadInventoryBox_C") != 0 || strstr(ClassName, "EscapePlayerTombBox_C") != 0 || strstr(ClassName, "aracterDeadInventoryBox_C") != 0 || strstr(ClassName, "SuperPeopleDeadBox_C") != 0 || strstr(ClassName, "_TrainingBoxListWrapper_C") != 0 || strstr(ClassName, "PickUpListWrapperActor") != 0 || strstr(ClassName, "RollTombBox_C") != 0 || strstr(ClassName, "BigMech_RollTombBox_C") != 0)) {
                std::string Airdrop = "\uE9D7";
                std::string name = "盒子[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "M]";

                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 20);
                auto iconSize = YuanerSet->icon->CalcTextSizeA(25, FLT_MAX, 0.0f, Airdrop.c_str());
                float totalWidth = iconSize.x + 2 + textSize.x;
                float startX = t_屏幕坐标.X - (totalWidth / 2) + 50;
    
                float iconX = startX;
                float iconY = t_屏幕坐标.Y;
                ImGui::GetForegroundDrawList()->AddText(YuanerSet->icon, 25, {iconX, iconY}, ImColor(84, 150, 136, 255), Airdrop.c_str());
        
                float textX = iconX + iconSize.x + 2;
                ImGui::GetForegroundDrawList()->AddText(NULL, 20, {textX, t_屏幕坐标.Y}, ImColor(84, 150, 136, 255), name.c_str());
            }

            if (按钮.空投 && (strstr(ClassName, "AirDropBox") != 0 or strstr(ClassName, "AirDropBox") != 0)) {
            /*    {
                    txnet_radar_item_pubgmhd ad = {};
                    ad.type = "airdrop";
                    ad.address = (unsigned long long)对象地址.敌人地址;
                    ad.uid = NULL;
                    ad.name = "空投";
                    ad.team = 0;
                    ad.pos_x = 对象信息.敌人信息.坐标.X;
                    ad.pos_y = 对象信息.敌人信息.坐标.Y;
                    ad.pos_z = 对象信息.敌人信息.坐标.Z;
                    ad.health = 0;
                    ad.signal = TXNET_RADAR_NO_VALUE;
                    ad.fuel = TXNET_RADAR_NO_VALUE;
                    ad.rotator = TXNET_RADAR_NO_VALUE;
                    ad.pitch = 0;
                    ad.state = 0;
                    ad.is_bot = false;
                    ad.in_vehicle = false;
                    ad.distance = (int)对象信息.敌人信息.距离;
                    ad.weapon = 0;
                    ad.weapon_name = NULL;
                    ad.current_ammo = TXNET_RADAR_NO_VALUE;
                    ad.max_ammo = TXNET_RADAR_NO_VALUE;
                    txnet_radar_add_item(&ad);
                }*/
                std::string Airdrop = "\uE96E";
                std::string name = "空投[";
                name += std::to_string((int)对象信息.敌人信息.距离);
                name += "M]";
                auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 25);
                auto iconSize = YuanerSet->icon->CalcTextSizeA(25, FLT_MAX, 0.0f, Airdrop.c_str());
                float totalWidth = iconSize.x + 2 + textSize.x;
                float startX = t_屏幕坐标.X - (totalWidth / 2) + 50;
                float iconX = startX;
                float iconY = t_屏幕坐标.Y;
                ImGui::GetForegroundDrawList()->AddText(YuanerSet->icon, 25, {iconX, iconY}, ImColor(241, 133, 255, 200), Airdrop.c_str());
                float textX = iconX + iconSize.x + 2;
                ImGui::GetForegroundDrawList()->AddText(NULL, 25, {textX, t_屏幕坐标.Y}, ImColor(241, 133, 255, 200), name.c_str());
            }

            int 开启状态 = 读写.getDword(对象地址.敌人地址 + 0x270);
            if (按钮.绘制宝箱 && (strstr(ClassName, "EscapeBox_SupplyBox_") != 0 || strstr(ClassName, "EscapeBoxHight_SupplyBox_") != 0 || strstr(ClassName, "EscapeBoxHight_RadiationBox_") != 0 || strstr(ClassName, "CG033_ActivityCommonTreasureNew_Box") != 0 || strstr(ClassName, "CG033_ActivityCommonTreasureNew_BlueBox") != 0 || strstr(ClassName, "EscapeBoxHight_SupplyBox_Lv3-5") != 0 || strstr(ClassName, "BP_Escape_MonsterDeadInventoryBox_C") != 0 || strstr(ClassName, "BP_EscapeInnerWrapperList_C") != 0 || strstr(ClassName, "EscapeBoxPeak_RadiationBox_") != 0 || strstr(ClassName, "EscapeBoxPeak_Weapon_") != 0 || strstr(ClassName, "EscapeBoxPeak_Medical_") != 0 || strstr(ClassName, "EscapeBoxPeak_SupplyBox_") != 0)) {
                if (按钮.隐藏已开启 && 开启状态 == 1) {
                } else {
                    std::string Airdrop = "\uE96F";
                    std::string name = "物资箱 ";
                    name += Level(ClassName);   
                    if (开启状态 == 1) {
                        name += " 已开启";
                    } else {
                        name += " 未开启";
                    }
                    name += "[" + std::to_string(static_cast<int>(对象信息.敌人信息.距离)) + "M]";
                    auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 25);
                    auto iconSize = YuanerSet->icon->CalcTextSizeA(25, FLT_MAX, 0.0f, Airdrop.c_str());
                    float totalWidth = iconSize.x + 2 + textSize.x;
                    float startX = t_屏幕坐标.X - (totalWidth / 2) + 50;
                    float iconX = startX;
                    float iconY = t_屏幕坐标.Y;
                    ImGui::GetForegroundDrawList()->AddText(YuanerSet->icon, 25, {iconX, iconY}, ImColor(84, 150, 136, 255), Airdrop.c_str());
                    float textX = iconX + iconSize.x + 2;
                    ImGui::GetForegroundDrawList()->AddText(NULL, 25, {textX, t_屏幕坐标.Y}, ImColor(84, 150, 136, 255), name.c_str());
                }
            }

            if (按钮.绘制药箱 && (strstr(ClassName, "EscapeBox_Medical_") != 0 or strstr(ClassName, "EscapeBoxHight_Medical_") != 0)) {
                if (按钮.隐藏已开启 && 开启状态 == 1) {
                } else {
                    std::string Airdrop = "\uE96F";
                    std::string name = "药箱 ";
                    name += Level(ClassName);
                    if (开启状态 == 1) {
                        name += " 已开启";
                    } else {
                        name += " 未开启";
                    }
                    name += "[" + std::to_string(static_cast<int>(对象信息.敌人信息.距离)) + "M]";
                    auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 25);
                    auto iconSize = YuanerSet->icon->CalcTextSizeA(25, FLT_MAX, 0.0f, Airdrop.c_str());
                    float totalWidth = iconSize.x + 2 + textSize.x;
                    float startX = t_屏幕坐标.X - (totalWidth / 2) + 50;
                    float iconX = startX;
                    float iconY = t_屏幕坐标.Y;
                    ImGui::GetForegroundDrawList()->AddText(YuanerSet->icon, 25, {iconX, iconY}, ImColor(84, 150, 136, 255), Airdrop.c_str());
                    float textX = iconX + iconSize.x + 2;
                    ImGui::GetForegroundDrawList()->AddText(NULL, 25, {textX, t_屏幕坐标.Y}, ImColor(84, 150, 136, 255), name.c_str());
                }
            }

            if (按钮.绘制武器箱 && (strstr(ClassName, "EscapeBox_Weapon_") != 0 or strstr(ClassName, "EscapeBoxHight_Weapon_") != 0)) {
                if (按钮.隐藏已开启 && 开启状态 == 1) {
                } else {
                    std::string Airdrop = "\uE96F";
                    std::string name = "武器箱 ";
                    name += Level(ClassName);
                    if (开启状态 == 1) {
                        name += " 已开启";
                    } else {
                        name += " 未开启";
                    }
                    name += "[" + std::to_string(static_cast<int>(对象信息.敌人信息.距离)) + "M]";
        
                    auto textSize = ImGui::CalcTextSize(name.c_str(), 0, 25);
                    auto iconSize = YuanerSet->icon->CalcTextSizeA(25, FLT_MAX, 0.0f, Airdrop.c_str());
                    float totalWidth = iconSize.x + 2 + textSize.x;
                    float startX = t_屏幕坐标.X - (totalWidth / 2) + 50;        
                    float iconX = startX;
                    float iconY = t_屏幕坐标.Y;
                    ImGui::GetForegroundDrawList()->AddText(YuanerSet->icon, 25, {iconX, iconY}, ImColor(84, 150, 136, 255), Airdrop.c_str());        
                    float textX = iconX + iconSize.x + 2;
                    ImGui::GetForegroundDrawList()->AddText(NULL, 25, {textX, t_屏幕坐标.Y}, ImColor(84, 150, 136, 255), name.c_str());
                }
            }
        }

        if (strstr(ClassName, "BPPawn_Escape_Raven") != 0 or strstr(ClassName, "BPPawn_Escape_UAV_C") != 0)
        {
            continue;
        }
        if (读写.getDword(地址.自身地址) == 0 && 对象信息.敌人信息.距离 <= 5)
        {
            continue;
        }

        if (读写.getFloat(对象地址.敌人地址 + 0x38C4) == 479.5)
        {
            D4DVector 屏外预警坐标(r_x, r_y, r_y - r_z, (r_y - r_z) / 2);
            对象信息.敌人信息.队伍 = 读写.getDword(对象地址.敌人地址 + 0xB78);
            对象信息.敌人信息.isboot = (对象信息.敌人信息.队伍 == -1) ? 1 : 读写.getDword(对象地址.敌人地址 + 0xB94);
            念宝.高级人机 = (对象信息.敌人信息.队伍 == -1) ? 1 : 读写.getDword(对象地址.敌人地址 + 0xB98);
            if (按钮.忽略人机 && 对象信息.敌人信息.isboot == 1) {
                continue;
            }
            if (对象信息.敌人信息.队伍 < 0) {
                continue;
            }
            对象信息.敌人信息.状态 = 读写.getDword(读写.getPtr64(对象地址.敌人地址 + 0x1680));
            对象信息.敌人信息.雷达 = 计算.rotateCoord(自身数据.准星Y, (自身数据.坐标.X - 对象信息.敌人信息.坐标.X) / 200, (自身数据.坐标.Y - 对象信息.敌人信息.坐标.Y) / 200);
            读写.read(对象地址.敌人地址 + 0x10FC, &对象信息.敌人信息.向量, sizeof(对象信息.敌人信息.向量));
            对象信息.敌人信息.Rotator = 读写.getFloat(读写.getPtr64(对象地址.敌人地址 + 0x5C58) + 0x5fc) + 90;
            对象信息.敌人信息.当前血量 = 读写.getFloat(对象地址.敌人地址 + 0xff8);
            对象信息.敌人信息.最大血量 = 读写.getFloat(对象地址.敌人地址 + 0x1000);

            对象信息.敌人信息.乘坐载具 = 读写.getDword(对象地址.敌人地址 + 0x1288) != 0;
            对象信息.敌人信息.手持 = 读写.getDword(读写.getPtr64(对象地址.敌人地址 + 0x1108) + 0xDA8);
            对象信息.敌人信息.子弹数量 = 读写.getDword(读写.getPtr64(对象地址.敌人地址 + 0x1108) + 0x1d18);
            对象信息.敌人信息.子弹最大数量 = 读写.getDword(读写.getPtr64(对象地址.敌人地址 + 0x1108) + 0x1d1c);
            对象信息.敌人信息.敌人头 = 读写.getDword(读写.getPtr64(对象地址.敌人地址 + 0x4E60) + 0x1FC);
            对象信息.敌人信息.敌人甲 = 读写.getDword(读写.getPtr64(对象地址.敌人地址 + 0x4E60) + 0x21C);//
            对象信息.敌人信息.敌人包 = 读写.getDword(读写.getPtr64(对象地址.敌人地址 + 0x4E60) + 0x1CC);
            
            // 川上富江 - 顶尖类名骨骼写法 简洁 效率 独家
            
            auto matrixMultiply = [](D3DVector &result, const FMatrix &matrixA, const FMatrix &matrixB) {
                result.X = matrixA.M[3][0] * matrixB.M[0][0] + matrixA.M[3][1] * matrixB.M[1][0] + matrixA.M[3][2] * matrixB.M[2][0] + matrixA.M[3][3] * matrixB.M[3][0];
                result.Y = matrixA.M[3][0] * matrixB.M[0][1] + matrixA.M[3][1] * matrixB.M[1][1] + matrixA.M[3][2] * matrixB.M[2][1] + matrixA.M[3][3] * matrixB.M[3][1];
                result.Z = matrixA.M[3][0] * matrixB.M[0][2] + matrixA.M[3][1] * matrixB.M[1][2] + matrixA.M[3][2] * matrixB.M[2][2] + matrixA.M[3][3] * matrixB.M[3][2];
            };
            
            auto transformToMatrix = [](const FTransform &t) -> FMatrix {
                FMatrix m;
                m.M[3][0] = t.Translation.X; m.M[3][1] = t.Translation.Y; m.M[3][2] = t.Translation.Z;
                float x2 = t.Rotation.X + t.Rotation.X, y2 = t.Rotation.Y + t.Rotation.Y, z2 = t.Rotation.Z + t.Rotation.Z;
                float xx2 = t.Rotation.X * x2, yy2 = t.Rotation.Y * y2, zz2 = t.Rotation.Z * z2;
                m.M[0][0] = (1 - (yy2 + zz2)) * t.Scale3D.X;
                m.M[1][1] = (1 - (xx2 + zz2)) * t.Scale3D.Y;
                m.M[2][2] = (1 - (xx2 + yy2)) * t.Scale3D.Z;
                float yz2 = t.Rotation.Y * z2, wx2 = t.Rotation.W * x2;
                m.M[2][1] = (yz2 - wx2) * t.Scale3D.Z; m.M[1][2] = (yz2 + wx2) * t.Scale3D.Y;
                float xy2 = t.Rotation.X * y2, wz2 = t.Rotation.W * z2;
                m.M[1][0] = (xy2 - wz2) * t.Scale3D.Y; m.M[0][1] = (xy2 + wz2) * t.Scale3D.X;
                float xz2 = t.Rotation.X * z2, wy2 = t.Rotation.W * y2;
                m.M[2][0] = (xz2 + wy2) * t.Scale3D.Z; m.M[0][2] = (xz2 - wy2) * t.Scale3D.X;
                m.M[0][3] = 0; m.M[1][3] = 0; m.M[2][3] = 0; m.M[3][3] = 1;
                return m;
            };
            
            long int MeshOffset = 读写.getPtr64(对象地址.敌人地址 + 0x658);
            uintptr_t BoneArray = 读写.getPtr64(MeshOffset + 0x820) + 0x30;
            int boneCache = 读写.getPtr64(MeshOffset + 0x828);
            
            if (MeshOffset && BoneArray) {
                FTransform meshTransform;
                读写.read(MeshOffset + 0x1f0, &meshTransform, sizeof(FTransform));
                FMatrix c2wMatrix = transformToMatrix(meshTransform);
            
                bool isBossIndices = false;
                auto getFinalIndices = [&](uintptr_t MeshAddr, int Bonecount, const char* 类名, bool &isBoss) -> std::vector<int> {
                    if (strstr(类名, "Pawn_Escape_BOSS_Claws_C")) { isBoss = true; return {5, 4, 1, 28, 7, 29, 8, 30, 9, 57, 50, 58, 51, 59, 52}; }
                    if (strstr(类名, "Pawn_Escape_RD_") || strstr(类名, "Pawn_Escape_Boss_")) { isBoss = true; return {5, 4, 1, 28, 7, 29, 8, 30, 9, 52, 48, 53, 49, 54, 50}; }
                    
                    isBoss = false;
                    if (Bonecount == 69) return {5, 4, 1, 11, 33, 12, 34, 13, 35, 55, 59, 56, 60, 57, 61};
                    if (Bonecount == 71) return {5, 4, 1, 6, 34, 7, 35, 8, 36, 55, 59, 56, 60, 57, 61};
                    if (Bonecount == 73) return {5, 4, 1, 12, 34, 13, 35, 14, 36, 57, 61, 58, 62, 59, 63};
                    if (Bonecount == 63) return {5, 4, 0, 8, 30, 9, 31, 10, 32, 50, 54, 51, 55, 52, 56};
                    if (Bonecount == 64) return {5, 4, 0, 7, 29, 8, 30, 9, 31, 51, 57, 52, 58, 53, 59};
                    if (Bonecount == 38) return {5, 4, 0, 7, 17, 9, 19, 11, 21, 27, 34, 28, 35, 30, 36};
                    if (Bonecount == 30) return {5, 4, 0, 9, 14, 10, 15, 11, 16, 18, 22, 19, 23, 20, 24};
                    if (Bonecount == 26) return {5, 4, 0, 13, 6, 14, 7, 15, 8, 21, 18, 22, 19, 23, 20};
                    if (Bonecount == 27) return {5, 4, 0, 7, 10, 8, 11, 9, 12, 14, 18, 15, 19, 16, 20};
                    if (Bonecount == 46) return {5, 4, 0, 21, 8, 22, 9, 23, 10, 35, 40, 36, 41, 38, 44};
                    if (Bonecount == 42) return {16, 15, 0, 20, 31, 21, 32, 22, 33, 3, 8, 4, 9, 5, 10};
                    if (Bonecount == 76 || Bonecount == 61) return {5, 4, 0, 7, 27, 8, 28, 9, 29, 48, 52, 49, 53, 50, 54};
            
                    uintptr_t skeletalMesh = 读写.getPtr64(MeshAddr + 0x808);
                    if (skeletalMesh) {
                        uintptr_t nameArray = 读写.getPtr64(skeletalMesh + 0x218);
                        int nameCount = 读写.read<int>(skeletalMesh + 0x220);
                        std::unordered_map<std::string, int> tempMap;
                        for (int i = 0; i < nameCount; i++) {
                            int nameIndex = 读写.read<int>(nameArray + i * 0x10);
                            unsigned int block = (nameIndex / 0x4000) * 0x8;
                            unsigned int offset = (nameIndex % 0x4000) * 0x8;
                            uintptr_t namePoolChunk = 读写.read<uintptr_t>(地址.类地址 + block);
                            uintptr_t nameAddr = 读写.read<uintptr_t>(namePoolChunk + offset) + 0xC;
                            char buf[64] = {0};
                            读写.read(nameAddr, buf, 63);
                            std::string n = buf;
                            std::transform(n.begin(), n.end(), n.begin(), ::tolower);
                            tempMap[n] = i;
                        }
                        std::vector<std::string> targets = {
                            "spine_03", "pelvis", 
                            "upperarm_l", "upperarm_r", 
                            "lowerarm_l", "lowerarm_r", 
                            "hand_l", "hand_r", 
                            "thigh_l", "thigh_r", 
                            "calf_l", "calf_r", 
                            "foot_l", "foot_r"
                        };
                        std::vector<int> res;
                        res.push_back(5); 
                        for (const auto& t : targets) res.push_back(tempMap.count(t) ? tempMap[t] : -1);
                        return res;
                    }
                    return {5, 4, 0, 7, 27, 8, 28, 9, 29, 48, 52, 49, 53, 50, 54};
                };
            
                std::vector<int> indices = getFinalIndices(MeshOffset, boneCache, ClassName, isBossIndices);
            
                for (size_t i = 0; i < indices.size() && i < 17; i++) {
                    int idx = indices[i];
                    if (idx == -1) continue;
            
                    FTransform bTrans;
                    读写.read(BoneArray + idx * 48, &bTrans, sizeof(FTransform));
                    FMatrix bMat = transformToMatrix(bTrans);
                    matrixMultiply(对象信息.敌人信息.骨骼坐标[i], bMat, c2wMatrix);
                }
            
                对象信息.敌人信息.骨骼坐标[0].Z += 7.0f;
                
                if (对象信息.敌人信息.骨骼坐标[13].Z < 对象信息.敌人信息.骨骼坐标[14].Z) {
                    对象信息.敌人信息.骨骼坐标[14].Z = 对象信息.敌人信息.骨骼坐标[13].Z;
                }
            }

            char temp[648];
            读写.getUTF8(temp, 读写.getPtr64(对象地址.敌人地址 + 0xAF8));
            对象信息.敌人信息.名字 = temp;
      /*      {
                float healthPct = (对象信息.敌人信息.最大血量 > 0) ? (对象信息.敌人信息.当前血量 / 对象信息.敌人信息.最大血量 * 100.0f) : (float)TXNET_RADAR_NO_VALUE;
                txnet_radar_item_pubgmhd pl = {};
                pl.type = "player";
                pl.address = (unsigned long long)对象地址.敌人地址;
                pl.name = 对象信息.敌人信息.名字.c_str();
                pl.team = 对象信息.敌人信息.队伍;
                pl.pos_x = 对象信息.敌人信息.坐标.X;
                pl.pos_y = 对象信息.敌人信息.坐标.Y;
                pl.pos_z = 对象信息.敌人信息.坐标.Z;
                pl.health = healthPct;
                pl.state = 对象信息.敌人信息.状态;
                pl.is_bot = (对象信息.敌人信息.isboot != 0);
                pl.in_vehicle = 对象信息.敌人信息.乘坐载具;
                pl.distance = 对象信息.敌人信息.距离;
                pl.weapon = 对象信息.敌人信息.手持;
                pl.current_ammo = (对象信息.敌人信息.子弹数量 >= 0) ? 对象信息.敌人信息.子弹数量 : TXNET_RADAR_NO_VALUE;
                pl.max_ammo = (对象信息.敌人信息.子弹最大数量 >= 0) ? 对象信息.敌人信息.子弹最大数量 : TXNET_RADAR_NO_VALUE;
                txnet_radar_add_item(&pl);
            }*/
            bool 是否掐雷 = false;
            if (按钮.手雷预警)
            {
                if (对象信息.敌人信息.手持 == 602004 or 对象信息.敌人信息.手持 == 9825004)
                {
                    std::string 状态字符串 = std::to_string(对象信息.敌人信息.状态);
                    if (状态字符串.length() == 5)
                    {
                        计时器.addTimer(计算地址, 0);
                        是否掐雷 = true;
                    }
                    else
                    {
                        if (计时器.hasTimer(计算地址))
                        {
                            string 手雷递送 = 手雷类.calculateKey(对象信息.敌人信息.坐标.X, 对象信息.敌人信息.坐标.Y);
                            if (手雷递送 == "")
                            {
                                Antitankgrenade[计算地址] = Antitankgrenade[计算地址] + 1;
                                if (Antitankgrenade[计算地址] == 15)
                                {
                                    计时器.removeTimer(计算地址);
                                    Antitankgrenade.erase(计算地址);
                                }
                            }
                            else
                            {
                                计时器.renameTimer(计算地址, 手雷递送);
                                手雷类.remove(手雷递送);
                            }
                        }
                    }
                }
                else
                {
                    if (计时器.hasTimer(计算地址))
                    {
                        string 手雷递送 = 手雷类.calculateKey(对象信息.敌人信息.坐标.X, 对象信息.敌人信息.坐标.Y);
                        if (手雷递送 == "")
                        {
                            Antitankgrenade[计算地址] = Antitankgrenade[计算地址] + 1;
                            if (Antitankgrenade[计算地址] == 15)
                            {
                                计时器.removeTimer(计算地址);
                                Antitankgrenade.erase(计算地址);
                            }
                        }
                        else
                        {
                            计时器.renameTimer(计算地址, 手雷递送);
                            手雷类.remove(手雷递送);
                        }
                    }
                }
            }

            if (对象信息.敌人信息.队伍 == 自身数据.自身队伍)
                continue;

            bool LineOfSightToTab[15] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};
            
            if (t_屏幕坐标.W >= 0)
            {
                for (int j = 0; j < 16; j ++)
                {
                    LineOfSightToTab[j] = LineTrace::LineTraceSingle(LineTrace::ToPx(地址.PovLocation.X, 地址.PovLocation.Y, 地址.PovLocation.Z), LineTrace::ToPx(对象信息.敌人信息.骨骼坐标[j].X, 对象信息.敌人信息.骨骼坐标[j].Y, 对象信息.敌人信息.骨骼坐标[j].Z));
                }
            }

            if (按钮.雷达 && 对象信息.敌人信息.距离 <= 500) 
            {
                float dx = 对象信息.敌人信息.雷达.X;
                float dy = 对象信息.敌人信息.雷达.Y;
                float radius = 150.0f * 念宝.雷达大小 / 150.0f * 念宝.雷达大致;
            
                if (dx * dx + dy * dy <= radius * radius)
                {
                    ImVec2 敌人位置 = {按钮.雷达X + dx, 按钮.雷达Y + dy};
                    ImColor 圆点颜色 = (对象信息.敌人信息.isboot == 1) ? ImColor(200, 200, 200, 200) : ImColor(212, 175, 55, 255);
                    ImGui::GetForegroundDrawList()->AddCircleFilled(敌人位置, 4.0f, 圆点颜色);
                    string 距离文本 = to_string((int)对象信息.敌人信息.距离);
                    ImVec2 tSize = ImGui::CalcTextSize(距离文本.c_str());
                    绘图.绘制字体描边(13, 敌人位置.x - tSize.x/2, 敌人位置.y + 8, ImColor(180, 180, 180, 255), 距离文本.c_str());
                }
            }

            if (对象信息.敌人信息.状态 == 1048592 || 对象信息.敌人信息.状态 == 1048576)
                continue;
            if (对象信息.敌人信息.isboot == 1 || 念宝.高级人机 == 1)
            {
                绘制人机++;
            }
            else
            {
                绘制真人++;
            }
            // 下面开始绘制
            骨骼数据 t_骨骼数据 = 计算.计算骨骼(自身数据.矩阵, 对象信息.敌人信息.骨骼坐标, PX, PY);
            绘图.初始化坐标(t_屏幕坐标, t_骨骼数据);
            std::vector<D2DVector *> 骨骼二维坐标 = t_骨骼数据.获取所有骨骼指针();
            if (t_屏幕坐标.W >= 0) {
                D3DVector 头部坐标;
                float dmin = 666;
                float 与准距[15];
                
                与准距[1] = sqrt(pow(PX - t_骨骼数据.Head.X, 2) + pow(PY - t_骨骼数据.Head.Y, 2));
                与准距[2] = sqrt(pow(PX - t_骨骼数据.Chest.X, 2) + pow(PY - t_骨骼数据.Chest.Y, 2));
                与准距[3] = sqrt(pow(PX - t_骨骼数据.Pelvis.X, 2) + pow(PY - t_骨骼数据.Pelvis.Y, 2));
                与准距[4] = sqrt(pow(PX - t_骨骼数据.Left_Shoulder.X, 2) + pow(PY - t_骨骼数据.Left_Shoulder.Y, 2));
                与准距[5] = sqrt(pow(PX - t_骨骼数据.Right_Shoulder.X, 2) + pow(PY - t_骨骼数据.Right_Shoulder.Y, 2));
                与准距[6] = sqrt(pow(PX - t_骨骼数据.Left_Elbow.X, 2) + pow(PY - t_骨骼数据.Left_Elbow.Y, 2));
                与准距[7] = sqrt(pow(PX - t_骨骼数据.Right_Elbow.X, 2) + pow(PY - t_骨骼数据.Right_Elbow.Y, 2));
                与准距[8] = sqrt(pow(PX - t_骨骼数据.Left_Wrist.X, 2) + pow(PY - t_骨骼数据.Left_Wrist.Y, 2));
                与准距[9] = sqrt(pow(PX - t_骨骼数据.Right_Wrist.X, 2) + pow(PY - t_骨骼数据.Right_Wrist.Y, 2));
                与准距[10] = sqrt(pow(PX - t_骨骼数据.Left_Thigh.X, 2) + pow(PY - t_骨骼数据.Left_Thigh.Y, 2));
                与准距[11] = sqrt(pow(PX - t_骨骼数据.Right_Thigh.X, 2) + pow(PY - t_骨骼数据.Right_Thigh.Y, 2));
                与准距[12] = sqrt(pow(PX - t_骨骼数据.Left_Knee.X, 2) + pow(PY - t_骨骼数据.Left_Knee.Y, 2));
                与准距[13] = sqrt(pow(PX - t_骨骼数据.Right_Knee.X, 2) + pow(PY - t_骨骼数据.Right_Knee.Y, 2));
                与准距[14] = sqrt(pow(PX - t_骨骼数据.Left_Ankle.X, 2) + pow(PY - t_骨骼数据.Left_Ankle.Y, 2));
                与准距[15] = sqrt(pow(PX - t_骨骼数据.Right_Ankle.X, 2) + pow(PY - t_骨骼数据.Right_Ankle.Y, 2));
                
                int gmin;
                D3DVector 目标骨骼坐标;
                static bool islineofright = false;
                static bool islineofright2 = false;
                
                if (自瞄.追踪瞄准部位 == 0) 
                {
                    目标骨骼坐标 = 对象信息.敌人信息.骨骼坐标[0];
                } 
                else if (自瞄.追踪瞄准部位 == 1)
                {
                    目标骨骼坐标 = 对象信息.敌人信息.骨骼坐标[1];
                } 
                else if (自瞄.追踪瞄准部位 == 2) 
                {
                    目标骨骼坐标 = 对象信息.敌人信息.骨骼坐标[2];
                }
                else if (自瞄.追踪瞄准部位 == 3) 
                {
                    for(int i = 1; i < 16; i++)
                    {
                        if(与准距[i] < dmin)
                        {
                            gmin = i;
                            dmin = 与准距[i];
                        }
                    }
                
                    if (对象信息.敌人信息.距离 > 70.0f) 
                    {
                        bool isValidPart = (gmin == 1 || gmin == 4 || gmin == 5);
                        
                        if (!isValidPart) 
                        {
                            dmin = 666;
                            for(int i : {1, 4, 5}) 
                            {
                                if(与准距[i] < dmin) 
                                {
                                    gmin = i;
                                    dmin = 与准距[i];
                                }
                            }
                        }
                    }   
                
                    switch (gmin)
                    {
                        case 1:
                            目标骨骼坐标 = 对象信息.敌人信息.骨骼坐标[0];
                            break;
                        case 2:
                            目标骨骼坐标 = 对象信息.敌人信息.骨骼坐标[1];
                            break;
                        case 3:
                            目标骨骼坐标 = 对象信息.敌人信息.骨骼坐标[2];
                            break;
                        case 4:
                            目标骨骼坐标 = 对象信息.敌人信息.骨骼坐标[3];
                            break;
                        case 5:
                            目标骨骼坐标 = 对象信息.敌人信息.骨骼坐标[4];
                            break;
                        case 6:
                            目标骨骼坐标 = 对象信息.敌人信息.骨骼坐标[5];
                            break;
                        case 7:
                            目标骨骼坐标 = 对象信息.敌人信息.骨骼坐标[6];
                            break;
                        case 8:
                            目标骨骼坐标 = 对象信息.敌人信息.骨骼坐标[7];
                            break;
                        case 9:
                            目标骨骼坐标 = 对象信息.敌人信息.骨骼坐标[8];
                            break;
                        case 10:
                            目标骨骼坐标 = 对象信息.敌人信息.骨骼坐标[9];
                            break;
                        case 11:
                            目标骨骼坐标 = 对象信息.敌人信息.骨骼坐标[10];
                            break;
                        case 12:
                            目标骨骼坐标 = 对象信息.敌人信息.骨骼坐标[11];
                            break;
                        case 13:
                            目标骨骼坐标 = 对象信息.敌人信息.骨骼坐标[12];
                            break;
                        case 14:
                            目标骨骼坐标 = 对象信息.敌人信息.骨骼坐标[13];
                            break;
                        case 15:
                            目标骨骼坐标 = 对象信息.敌人信息.骨骼坐标[14];
                            break;
                        case 16:
                            目标骨骼坐标 = 对象信息.敌人信息.骨骼坐标[15];
                            break;
                    }
                }
                else if (自瞄.追踪瞄准部位 == 4)
                {
                    int targetBoneIndex = -1;
                
                    for (int i = 0; i < 16; i++)
                    {
                        if (!自瞄函数[自瞄.瞄准目标].掩体部位 == 999)
                        {
                            targetBoneIndex = i;
                        }
                    }
                
                    目标骨骼坐标 = 对象信息.敌人信息.骨骼坐标[targetBoneIndex];
                }
                
                float NowCoor[3];
                NowCoor[0] = 目标骨骼坐标.X;
                NowCoor[1] = 目标骨骼坐标.Y;
                NowCoor[2] = 目标骨骼坐标.Z;
                float 目标距离 = 对象信息.敌人信息.距离;
                float FlyTime;
                float 预判力度b = 自瞄.追踪预判力度;
                
                if (目标距离 >= 40) {
                    FlyTime = 目标距离 / (自身数据.子弹速度 * 0.01f) * 预判力度b;
                } else {
                    FlyTime = 目标距离 / (自身数据.子弹速度 * 0.0055f) * 预判力度b;
                }
                
                float BulletFlightTime = 目标距离 / 自身数据.子弹速度;
                float DropM = 140.0f * BulletFlightTime * BulletFlightTime;
                D3DVector 预判坐标 = 目标骨骼坐标;
                
                if (目标距离 > 50) 
                {
                    float dx = 目标骨骼坐标.X - PX;
                    float dy = 目标骨骼坐标.Y - PY;    
                    float angle = atan2(dy, dx);    
                    float 预判系数 = 0.3f;
                    float 预判量 = FlyTime * 预判系数;    
                    预判坐标.X = 目标骨骼坐标.X + cos(angle) * 预判量;
                    预判坐标.Y = 目标骨骼坐标.Y + sin(angle) * 预判量;    
                    预判坐标.Z = 目标骨骼坐标.Z + DropM;
                }
                else
                {
                    预判坐标.Z = 目标骨骼坐标.Z + DropM;
                }
                
                头部坐标 = 预判坐标;
                
                float currXdis = abs(PX - t_骨骼数据.Head.X);
                float currYdis = abs(PY - t_骨骼数据.Head.Y);
                
                if (currXdis < 追踪范围限制 && currYdis < 追踪范围限制)
                {
                    float currDis = (currXdis + currYdis) / 2;
                    if (currDis < tempDis)
                    {
                        tempDis = currDis;
                        tempBase = 对象地址.敌人地址;
                    }
                }
                
                if (对象地址.敌人地址 == lockBase)
                {
                    if (自瞄.倒地不追 && 对象信息.敌人信息.当前血量 <= 0)
                    {
                        倒地不追 = false;
                    }
                    else
                    {
                        倒地不追 = true;
                    }
                
                    if (自瞄.人机不追 && 对象信息.敌人信息.isboot == 1)
                    {
                        人机不追 = false;
                    }
                    else
                    {
                        人机不追 = true;
                    }
                
                    人物距离 = sqrt(pow(对象信息.敌人信息.坐标.X - 自身数据.坐标.X, 2) + pow(对象信息.敌人信息.坐标.Y - 自身数据.坐标.Y, 2) + pow(对象信息.敌人信息.坐标.Z - 自身数据.坐标.Z, 2)) * 0.01;
                
                    vector31 local = {0};
                    uintptr_t PlayerCameraManager = 读写.getPtr64(自身数据.Controller + 0x660);
                    读写.read(PlayerCameraManager + 0x640, &local, sizeof(local));
                    
                    FRotator rot = toRotator(local, 头部坐标);
                    
                    uintptr_t EquipWeapon = 读写.getPtr64(地址.自身地址 + 0x1108);
                    uintptr_t ShootWeaponComponent = 读写.getPtr64(EquipWeapon + 0x1f58);           
                
                    读写.writev(ShootWeaponComponent + 0x50, &rot, sizeof(rot));                          
                    读写.writev(PlayerCameraManager + 0x50, &rot, sizeof(rot));                 
                }
                if (自瞄.倒地不瞄 && 对象信息.敌人信息.当前血量 <= 0) {
                }
                else if (自瞄.人机不瞄 && 对象信息.敌人信息.isboot == 1 or 念宝.高级人机 == 1)
                {
                }
                else
                {
                    自瞄函数[自瞄.瞄准对象数量].距离 = 对象信息.敌人信息.距离;
                    自瞄函数[自瞄.瞄准对象数量].人物向量 = 对象信息.敌人信息.向量;
                    自瞄函数[自瞄.瞄准对象数量].血量 = 对象信息.敌人信息.当前血量;
                    自瞄函数[自瞄.瞄准对象数量].Bone = 读写.getPtr64(MeshOffset + 0x820) + 0x30;
                    自瞄函数[自瞄.瞄准对象数量].Human = MeshOffset + 0x1f0;
                    自瞄函数[自瞄.瞄准对象数量].名字 = 对象信息.敌人信息.名字;
                    自瞄函数[自瞄.瞄准对象数量].阵营 = 对象信息.敌人信息.队伍;
                    自瞄函数[自瞄.瞄准对象数量].头 = 对象信息.敌人信息.头;
                    自瞄函数[自瞄.瞄准对象数量].甲 = 对象信息.敌人信息.甲;
                    memcpy(自瞄函数[自瞄.瞄准对象数量].骨骼坐标, 对象信息.敌人信息.骨骼坐标, sizeof(对象信息.敌人信息.骨骼坐标));

                    if (自瞄.瞄准部位 == 0)
                    {
                        int BonePointer = ShelterJudgment(LineOfSightToTab);
                        自瞄函数[自瞄.瞄准对象数量].掩体部位 = BonePointer;
                        if (BonePointer == 999)
                        {
                            BonePointer = 0;
                        }
                        自瞄函数[自瞄.瞄准对象数量].瞄准坐标 = 对象信息.敌人信息.骨骼坐标[BonePointer];
                        自瞄函数[自瞄.瞄准对象数量].准心距离 = sqrt(pow(PX - 骨骼二维坐标[BonePointer]->X, 2) + pow(PY - 骨骼二维坐标[BonePointer]->Y, 2));
                        自瞄函数[自瞄.瞄准对象数量].对象骨骼 = *骨骼二维坐标[BonePointer];
                    }
                    else if (自瞄.瞄准部位 == 3)
                    {
                        std::string targetId = 自瞄函数[自瞄.瞄准对象数量].名字;    
                        static std::map<std::string, int> lastBoneIndexMap;
                        static std::map<std::string, bool> firingStateMap;
    
                        if (自身数据.开火 == 1 && firingStateMap.count(targetId) > 0 && firingStateMap[targetId] && lastBoneIndexMap.count(targetId) > 0)
                        {
                            int lastBoneIndex = lastBoneIndexMap[targetId];
                            自瞄函数[自瞄.瞄准对象数量].瞄准坐标 = 对象信息.敌人信息.骨骼坐标[lastBoneIndex];
                            自瞄函数[自瞄.瞄准对象数量].准心距离 = sqrt(pow(PX - 骨骼二维坐标[lastBoneIndex]->X, 2) + pow(PY - 骨骼二维坐标[lastBoneIndex]->Y, 2));
                            自瞄函数[自瞄.瞄准对象数量].对象骨骼 = *骨骼二维坐标[lastBoneIndex];
                            firingStateMap[targetId] = true;
                        }
                        else
                        {
                            float minDistance = FLT_MAX;
                            int closestBoneIndex = 0;
                            for (size_t anian = 0; anian < 16; anian++)
                            {
                                if (骨骼二维坐标[anian] != nullptr)
                                {
                                    float distance = sqrt(pow(PX - 骨骼二维坐标[anian]->X, 2) + pow(PY - 骨骼二维坐标[anian]->Y, 2));
                                    if (distance < minDistance)
                                    {
                                        minDistance = distance;
                                        closestBoneIndex = anian;
                                    }
                                }
                            }        
                            lastBoneIndexMap[targetId] = closestBoneIndex;
                            firingStateMap[targetId] = (自身数据.开火 == 1);        
                            自瞄函数[自瞄.瞄准对象数量].瞄准坐标 = 对象信息.敌人信息.骨骼坐标[closestBoneIndex];
                            自瞄函数[自瞄.瞄准对象数量].准心距离 = minDistance;
                            自瞄函数[自瞄.瞄准对象数量].对象骨骼 = *骨骼二维坐标[closestBoneIndex];
                        }   
                    }
                    else if (自瞄.瞄准部位 == 1)
                    {
                        int BonePointer = ShelterJudgment(LineOfSightToTab);
                        自瞄函数[自瞄.瞄准对象数量].掩体部位 = BonePointer;
                        if (BonePointer == 999)
                        {
                            BonePointer = 1;
                        }
                        自瞄函数[自瞄.瞄准对象数量].瞄准坐标 = 对象信息.敌人信息.骨骼坐标[BonePointer];
                        自瞄函数[自瞄.瞄准对象数量].准心距离 = sqrt(pow(PX - 骨骼二维坐标[BonePointer]->X, 2) + pow(PY - 骨骼二维坐标[BonePointer]->Y, 2));
                        自瞄函数[自瞄.瞄准对象数量].对象骨骼 = *骨骼二维坐标[BonePointer];
                    }
                    else if (自瞄.瞄准部位 == 2)
                    {
                        int BonePointer = ShelterJudgment(LineOfSightToTab);
                        自瞄函数[自瞄.瞄准对象数量].掩体部位 = BonePointer;
                        if (BonePointer == 999)
                        {
                            BonePointer = 2;
                        }
                        自瞄函数[自瞄.瞄准对象数量].瞄准坐标 = 对象信息.敌人信息.骨骼坐标[BonePointer];
                        自瞄函数[自瞄.瞄准对象数量].准心距离 = sqrt(pow(PX - 骨骼二维坐标[BonePointer]->X, 2) + pow(PY - 骨骼二维坐标[BonePointer]->Y, 2));
                        自瞄函数[自瞄.瞄准对象数量].对象骨骼 = *骨骼二维坐标[BonePointer];
                    }

                    float ui_yuan = sqrt(pow(PX - t_骨骼数据.Head.X, 2) + pow(PY - t_骨骼数据.Head.Y, 2));
                    
                    if (自瞄.智能自瞄){
                        if (ui_yuan <= 自瞄.自瞄范围 && 对象信息.敌人信息.状态 == 2097168){
                            自瞄.瞄准部位 = 2;
                        }
                        
                        if (ui_yuan <= 自瞄.自瞄范围 && 对象信息.敌人信息.状态 == 4112 or 对象信息.敌人信息.状态 == 4128 or 对象信息.敌人信息.状态 == 12320 or 对象信息.敌人信息.状态 == 5153 or 对象信息.敌人信息.状态 == 5409 or 对象信息.敌人信息.状态 == 5137 or 对象信息.敌人信息.状态 == 5393){
                            自瞄.瞄准部位 = 0;
                        }
  
                        if (ui_yuan <= 自瞄.自瞄范围 && 对象信息.敌人信息.状态 == 32 or 对象信息.敌人信息.状态 == 64 or 对象信息.敌人信息.状态 == 35 or 对象信息.敌人信息.状态 == 65 or 对象信息.敌人信息.状态 == 67){
                            自瞄.瞄准部位 = 0;
                        }
                        
                        if (ui_yuan <= 自瞄.自瞄范围 && 对象信息.敌人信息.状态 == 273 or 对象信息.敌人信息.状态 == 16 or 对象信息.敌人信息.状态 == 19 or 对象信息.敌人信息.状态 == 147)
                        {
                            自瞄.瞄准部位 = 2;
                        }  
                    }
                    自瞄.瞄准对象数量++;
                }
            }
    
            if (按钮.被瞄预警)
            {
                auto aimAngle = 计算.rotateCoord(对象信息.敌人信息.骨骼坐标[1], 自身数据.坐标);
                auto aimMZ = FRotator(0, 对象信息.敌人信息.Rotator, 0);
                aimMZ.Clamp();
                float AimX = abs(aimAngle.X - aimMZ.Yaw);

                if (AimX <= 4)
                {
                    被瞄信息[被瞄准对象数量].距离 = 对象信息.敌人信息.距离;
                    被瞄信息[被瞄准对象数量].名字 = 对象信息.敌人信息.名字;
                    被瞄信息[被瞄准对象数量].瞄准武器 = GetHolGunItem(对象信息.敌人信息.手持);
                    被瞄准对象数量++;
                }
            }

            if (按钮.背敌预警)
            {
                if (对象信息.敌人信息.isboot == 1 || 念宝.高级人机 == 1)
                {
                    OffScreen(ImGui::GetForegroundDrawList(), 屏外预警坐标, camear_r, ImColor(0, 255, 0, 200), 自瞄.自瞄范围 + 90, 对象信息.敌人信息.距离);
                }
                else
                {
                    OffScreen(ImGui::GetForegroundDrawList(), 屏外预警坐标, camear_r, ImColor(255, 0, 0, 200), 自瞄.自瞄范围 + 90, 对象信息.敌人信息.距离);
                }
            }

            if (t_屏幕坐标.W >= 0)
            {
                float 计算_max = 7 - (计时器.getTimerSeconds(计算地址) / 7 * (60 / ImGui::GetIO().Framerate * 0.115));
                if (按钮.方框)
                    绘图.绘制方框(对象信息.敌人信息.isboot || 念宝.高级人机);
                if (按钮.头甲包显示)
                    绘图.头甲包显示(t_骨骼数据, 对象信息.敌人信息.敌人头, 对象信息.敌人信息.敌人甲, 对象信息.敌人信息.敌人包, t_屏幕坐标);
                if (按钮.射线)
                    绘图.绘制射线(t_骨骼数据, LineOfSightToTab);
                if (按钮.名字)
                {
                    float top = t_骨骼数据.Pelvis.Y - ((t_骨骼数据.Head.X != 0) ? (t_骨骼数据.Pelvis.Y - t_骨骼数据.Head.Y) : (t_骨骼数据.Pelvis.Y - t_骨骼数据.Chest.Y)) - t_屏幕坐标.Z / 5;
                    auto *drawList = ImGui::GetForegroundDrawList();
                    const float 血条顶部偏移 = 30.0f;
                    float 名字标签顶部 = top - 血条顶部偏移 - 2.0f - 22.0f + 5.0f;
                    string a = 念宝.高级人机 ? "高级RoBot 别打!" : (对象信息.敌人信息.isboot ? "RoBot" : 对象信息.敌人信息.名字);
                    if (strstr(ClassName, "BOSS") != 0) {
                        a += "[BOSS]";
                   }

                    string TeamText = to_string(对象信息.敌人信息.队伍);
                    static ImU32 teamRectColors[100];
                    static bool colorsInitialized = false;
                    if (!colorsInitialized) {
                        for (int i = 0; i < 100; i++) {
                            teamRectColors[i] = IM_COL32(rand() % 256, rand() % 256, rand() % 256, 255);
                        }
                        colorsInitialized = true;
                    }    
                    ImU32 rectColor = teamRectColors[对象信息.敌人信息.队伍 % 100];
                    const float 最小有效距离 = 1.0f;
                    const float 最大有效距离 = 1000.0f;
                    const float 最小缩放 = 0.4f;    
                    float 原始距离 = 对象信息.敌人信息.距离;
                    float 安全距离 = std::clamp(原始距离, 最小有效距离, 最大有效距离);    
                    float 距离比例 = (安全距离 - 最小有效距离) / (最大有效距离 - 最小有效距离);
                    float 距离缩放 = 1.0f - 0.6f * std::log(1.0f + 距离比例 * 2.0f);
                    距离缩放 = std::clamp(距离缩放, 最小缩放, 1.0f);
                    const float 最小字体大小 = 8.0f;
                    float 名字基础大小 = std::max(19.2f * 距离缩放, 最小字体大小);
                    float 队伍基础大小 = std::max(17.4f * 距离缩放, 最小字体大小);    
                    ImVec2 TeamTextSize = ImGui::CalcTextSize(TeamText.c_str(), nullptr, 队伍基础大小);
                    ImVec2 NameTextSize = ImGui::CalcTextSize(a.c_str(), nullptr, 名字基础大小);
    
                    string 手雷倒计时文本 = "";
                    ImVec2 手雷倒计时文本大小(0, 0);
                    if (是否掐雷) {
                        std::string timeStr = std::to_string(计算_max);
                        size_t dotPos = timeStr.find('.');
                        if (dotPos != std::string::npos && dotPos + 2 < timeStr.length()) {
                            timeStr = timeStr.substr(0, dotPos + 2);
                        }    
                        手雷倒计时文本 = " [" + timeStr + "s]";
                        手雷倒计时文本大小 = ImGui::CalcTextSize(手雷倒计时文本.c_str(), nullptr, 名字基础大小);
                    }
   
                    const float 最小矩形尺寸 = 10.0f;
                    float 矩形基础边长 = std::max(TeamTextSize.y * 1.5f, 最小矩形尺寸);
                    float rectWidth = std::max(矩形基础边长 * 距离缩放, 最小矩形尺寸);
                    float rectHeight = rectWidth;
                    const float 血条基础宽度 = 85.0f;
                    float healthBarLeft = MIDDLE - 血条基础宽度 / 2 - 15.0f;
                    healthBarLeft = std::max(healthBarLeft, 0.0f);
                    float 红色竖条左侧 = std::max(healthBarLeft - 5.0f * 距离缩放, 0.0f);
                    float 红色竖条宽度 = std::max(2.0f * 距离缩放, 1.0f);

                    drawList->AddRectFilled({红色竖条左侧, 名字标签顶部}, {红色竖条左侧 + 红色竖条宽度, 名字标签顶部 + rectHeight}, IM_COL32(255, 0, 0, 255));    
                    ImVec2 rectPos = {healthBarLeft, 名字标签顶部};
                    drawList->AddRectFilled(rectPos, {rectPos.x + rectWidth, rectPos.y + rectHeight}, rectColor);
  
                    if (TeamTextSize.x > 0 && TeamTextSize.y > 0) {
                        ImVec2 TeamTextPos = {
                            rectPos.x + (rectWidth - TeamTextSize.x) / 2, rectPos.y + (rectHeight - TeamTextSize.y) / 2
                        };
                        drawList->AddText(nullptr, 队伍基础大小, TeamTextPos, IM_COL32(255, 255, 255, 255), TeamText.c_str());
                    }

                    if (NameTextSize.x > 0 && NameTextSize.y > 0) {
                        ImVec2 NameTextPos = {
                            rectPos.x + rectWidth + std::max(8.0f * 距离缩放, 2.0f), rectPos.y + (rectHeight - NameTextSize.y) / 2
                        };
        
                        drawList->AddText(nullptr, 名字基础大小, NameTextPos, IM_COL32(255, 255, 255, 255), a.c_str());
        
                        if (是否掐雷) {
                            ImVec2 手雷倒计时文本位置 = {
                                NameTextPos.x + NameTextSize.x + std::max(8.0f * 距离缩放, 2.0f), NameTextPos.y
                            };
                            drawList->AddText(nullptr, 名字基础大小, 手雷倒计时文本位置, IM_COL32(255, 0, 0, 255), 手雷倒计时文本.c_str());
                        }
                    }
                }                
                if (按钮.距离)
                {
                    string 距离文本 = to_string((int)对象信息.敌人信息.子弹数量) + " / " + to_string((int)对象信息.敌人信息.子弹最大数量) + " " + to_string((int)对象信息.敌人信息.距离) + "m";
                    float top = t_骨骼数据.Pelvis.Y - ((t_骨骼数据.Head.X != 0) ? (t_骨骼数据.Pelvis.Y - t_骨骼数据.Head.Y) : (t_骨骼数据.Pelvis.Y - t_骨骼数据.Chest.Y)) - t_屏幕坐标.Z / 5;
                    auto *drawList = ImGui::GetForegroundDrawList();
                    const float 血条顶部偏移 = 30.0f;
                    float 名字标签顶部 = top - 血条顶部偏移 - 2.0f - 22.0f - 10.0f;
    
                    float 距离信息顶部 = 名字标签顶部 - ImGui::CalcTextSize(距离文本.c_str()).y - 2.0f;
                    float 距离信息水平位置 = MIDDLE - ImGui::CalcTextSize(距离文本.c_str()).x / 2;
                    drawList->AddText(nullptr, 17.0f, {距离信息水平位置, 距离信息顶部}, IM_COL32(255, 255, 255, 255), 距离文本.c_str());
                }   
                if (按钮.血量)
                    绘图.绘制血量(对象信息.敌人信息.最大血量, 对象信息.敌人信息.当前血量, 对象信息.敌人信息.距离);
                if (按钮.骨骼)
                    绘图.绘制骨骼(t_骨骼数据, t_屏幕坐标, LineOfSightToTab, 对象信息.敌人信息.距离);
                if (按钮.漏手)
                    绘图.绘制漏手(对象信息.敌人信息.isboot || 念宝.高级人机);
                if (按钮.手持)
                    绘图.绘制手持(对象信息.敌人信息.手持, 对象信息.敌人信息.状态, 自身数据.投掷);
            } else {
                continue;
            }
        }
    }
    
    if (自身数据.开镜 == 1 || 自身数据.开镜 == 257) 
    {
        追踪范围限制 = 自瞄.追踪开镜范围;
    } 
    else 
    {
        追踪范围限制 = 自瞄.追踪腰射范围;
    }
    
    if (自身数据.开镜 == 1 || 自身数据.开镜 == 257) 
    {
        追踪概率限制 = 自瞄.追踪开镜概率;
    } 
    else 
    {
        追踪概率限制 = 自瞄.追踪腰射概率;
    }
    
    if (自身数据.开镜 == 1 || 自身数据.开镜 == 257) 
    {
        追踪距离限制 = 自瞄.追踪开镜距离;
    } 
    else if (自身数据.开镜 == 1) 
    {
        追踪距离限制 = 自瞄.追踪腰射距离;
    }
    
    if (散弹枪(自身数据.手持) == 1.0f)
    {
        追踪概率限制 = 自瞄.喷子追踪概率;
    } 
    else if (是弓(自身数据.手持)) 
    {
        追踪概率限制 = 自瞄.弓追踪概率;
    } 
    else if (是冲锋枪(自身数据.手持)) 
    {
        追踪概率限制 = 自瞄.冲锋枪追踪概率;
    } 
    else if (是连狙(自身数据.手持)) 
    {
        追踪概率限制 = 自瞄.连狙追踪概率;
    } 
    else if (是单发狙(自身数据.手持)) 
    {
        追踪概率限制 = 自瞄.栓狙追踪概率;
    }
    
    if (对象地址.敌人地址 == lockBase) 
    {
        人物距离 = sqrt(pow(对象信息.敌人信息.坐标.X - 自身数据.坐标.X, 2) + pow(对象信息.敌人信息.坐标.Y - 自身数据.坐标.Y, 2) + pow(对象信息.敌人信息.坐标.Z - 自身数据.坐标.Z, 2)) * 0.01f;
    }
    
    lockBase = tempBase;

    if (自瞄.开启追踪 && 自瞄.准星射线 && 自瞄.瞄准目标 != -1 && 自瞄函数[自瞄.瞄准目标].准心距离 <= 自瞄.自瞄范围) 
    {
        float FlyTime = 自瞄函数[自瞄.瞄准目标].距离 / (自身数据.子弹速度 * 0.01f) * 自瞄.预判力度;
        float BulletFlightTime = 自瞄函数[自瞄.瞄准目标].距离 / 自身数据.子弹速度;
        float DropM = 140.0f * BulletFlightTime * BulletFlightTime;
        
        D3DVector 念宝预判坐标;
        念宝预判坐标.X = 自瞄函数[自瞄.瞄准目标].瞄准坐标.X + 自瞄函数[自瞄.瞄准目标].人物向量.X * FlyTime;
        念宝预判坐标.Y = 自瞄函数[自瞄.瞄准目标].瞄准坐标.Y + 自瞄函数[自瞄.瞄准目标].人物向量.Y * FlyTime;
        念宝预判坐标.Z = 自瞄函数[自瞄.瞄准目标].瞄准坐标.Z + 自瞄函数[自瞄.瞄准目标].人物向量.Z * FlyTime + DropM;
    
        D2DVector 念宝屏幕坐标 = 计算.计算屏幕坐标2(自身数据.矩阵, 念宝预判坐标, PX, PY);
    
        ImGui::GetForegroundDrawList()->AddLine(ImVec2(PX, PY), ImVec2(念宝屏幕坐标.X, 念宝屏幕坐标.Y), ImColor(255, 255, 255, 255), 1.0f);
        ImGui::GetForegroundDrawList()->AddCircleFilled(ImVec2(念宝屏幕坐标.X, 念宝屏幕坐标.Y), 1.5f, ImColor(255, 255, 255, 255));
    }
    
    if (自瞄.初始化 && 自瞄.准星射线 && 自瞄.瞄准目标 != -1 && 自瞄函数[自瞄.瞄准目标].准心距离 <= 自瞄.自瞄范围)
    {
        float FlyTime = 自瞄函数[自瞄.瞄准目标].距离 / (自身数据.子弹速度 * 0.01f) * 自瞄.预判力度;
        float BulletFlightTime = 自瞄函数[自瞄.瞄准目标].距离 / 自身数据.子弹速度;
        float DropM = 140.0f * BulletFlightTime * BulletFlightTime;
        
        D3DVector 念宝预判坐标;
        念宝预判坐标.X = 自瞄函数[自瞄.瞄准目标].瞄准坐标.X + 自瞄函数[自瞄.瞄准目标].人物向量.X * FlyTime;
        念宝预判坐标.Y = 自瞄函数[自瞄.瞄准目标].瞄准坐标.Y + 自瞄函数[自瞄.瞄准目标].人物向量.Y * FlyTime;
        念宝预判坐标.Z = 自瞄函数[自瞄.瞄准目标].瞄准坐标.Z + 自瞄函数[自瞄.瞄准目标].人物向量.Z * FlyTime + DropM;
    
        D2DVector 念宝屏幕坐标 = 计算.计算屏幕坐标2(自身数据.矩阵, 念宝预判坐标, PX, PY);
    
        ImGui::GetForegroundDrawList()->AddLine(ImVec2(PX, PY), ImVec2(念宝屏幕坐标.X, 念宝屏幕坐标.Y), ImColor(255, 255, 255, 255), 1.0f);
        ImGui::GetForegroundDrawList()->AddCircleFilled(ImVec2(念宝屏幕坐标.X, 念宝屏幕坐标.Y), 1.5f, ImColor(255, 255, 255, 255));
    }
    
    SmokeCount = SmokeNum;
    
    if (按钮.被瞄预警)
        绘图.绘制瞄准信息();
    if (按钮.人数)
        绘图.绘制人数(绘制人机, 绘制真人);
    自瞄.瞄准总数量 = 自瞄.瞄准对象数量;
}

void 绘制::运行绘制() {
    更新地址数据();
    更新对象数据();    
    DrawESP(ImGui::GetForegroundDrawList());

    static bool 已调用 = false;
    static int 上一次方案 = -1;
    
    if (自瞄.初始化)
    {
        if (!已调用 || (自瞄.自瞄算法 != 上一次方案))
        {
            switch (上一次方案)
            {
                case 0: 停止触摸自瞄(); break;
                case 1: 停止陀螺仪自瞄(); break;
            }

            上一次方案 = 自瞄.自瞄算法;

            if (自瞄.自瞄算法 == 1 || 自瞄.自瞄算法 == 2)
            {
                GetTouch();
                自瞄主线程();
            }
            else if (自瞄.自瞄算法 == 0)
            {
                陀螺仪自瞄主线程();
            }

            已调用 = true;
        }
    }
    
    if (按钮.手雷预警) {
        计时器.updateTimers();
        计时器.checkAndRemoveTimers();
    }
}

const char *绘制::Level(char *name)
{
    if (strstr(name, "Lv1") != 0)
    {
        return "Lv1";
    }
    else if (strstr(name, "Lv2") != 0)
    {
        return "Lv2";
    }
    else if (strstr(name, "Lv3") != 0)
    {
        return "Lv3";
    }
    else if (strstr(name, "Lv4") != 0)
    {
        return "Lv4";
    }
    else if (strstr(name, "Lv5") != 0)
    {
        return "Lv5";
    }
    else if (strstr(name, "Lv6") != 0)
    {
        return "Lv6";
    }
    else if (strstr(name, "Lv7") != 0)
    {
        return "Lv7";
    }
    return "未知";
}

const char *绘制::getMaterialName(char *name)
{
    static char dealname[32];
    memset(dealname, '\0', sizeof(dealname));
    if (strstr(name, "VH_MotorcycleCart") != 0)
    {
        strcpy(dealname, "三轮摩托车");
        按钮.VehicleID = 43;
    }
    else if (strstr(name, "VH_Scooter") != 0)
    {
        strcpy(dealname, "小绵羊车");
        按钮.VehicleID = 45;
    }
    else if (strstr(name, "VH_Motorcycle") != 0)
    {
        strcpy(dealname, "摩托车");
        按钮.VehicleID = 44;
    }
    else if (strstr(name, "VH_Tuk") != 0)
    {
        strcpy(dealname, "三轮摩托车");
        按钮.VehicleID = 48;
    }
    else if (strstr(name, "Buggy") != 0)
    {
        strcpy(dealname, "蹦蹦");
        按钮.VehicleID = 30;
    }
    else if (strstr(name, "Mirado") != 0)
    {
        strcpy(dealname, "敞篷跑车");
        按钮.VehicleID = 35;
    }
    else if (strstr(name, "CoupeRB") != 0)
    {
        strcpy(dealname, "跑车");
        按钮.VehicleID = 39;
    }
    else if (strstr(name, "Dacia") != 0)
    {
        strcpy(dealname, "轿车");
        按钮.VehicleID = 31;
    }
    else if (strstr(name, "PickUp_02_C") != 0)
    {
        strcpy(dealname, "皮卡");
        按钮.VehicleID = 33;
    }
    else if (strstr(name, "Rony") != 0)
    {
        strcpy(dealname, "皮卡");
        按钮.VehicleID = 38;
    }
    else if (strstr(name, "_StationWagon_C") != 0)
    {
        strcpy(dealname, "旅行车");
        按钮.VehicleID = 40;
    }
    else if (strstr(name, "UAZ") != 0)
    {
        strcpy(dealname, "吉普");
        按钮.VehicleID = 32;
    }
    else if (strstr(name, "PG117") != 0)
    {
        strcpy(dealname, "快艇");
        按钮.VehicleID = 49;
    }
    else if (strstr(name, "AquaRail") != 0)
    {
        strcpy(dealname, "冲锋艇");
        按钮.VehicleID = 50;
    }
    else if (strstr(name, "MiniBus") != 0)
    {
        strcpy(dealname, "面包车");
        按钮.VehicleID = 37;
    }
    else if (strstr(name, "VH_BRDM") != 0)
    {
        strcpy(dealname, "两栖装甲车");
        按钮.VehicleID = 0;
    }
    else if (strstr(name, "LadaNiva") != 0)
    {
        strcpy(dealname, "雪地越野车");
        按钮.VehicleID = 36;
    }
    else if (strstr(name, "_4SportCar_C") != 0)
    {
        strcpy(dealname, "敞篷跑车");
        按钮.VehicleID = 41;
    }
    else if (strstr(name, "_Bigfoot_C") != 0)
    {
        strcpy(dealname, "大脚车");
        按钮.VehicleID = 53;
    }
    else if (strstr(name, "_Training_C") != 0)
    {
        strcpy(dealname, "自行车");
        按钮.VehicleID = 0;
    }
    else if (strstr(name, "ATV1_C") != 0)
    {
        strcpy(dealname, "越野摩托车");
        按钮.VehicleID = 0;
    }
    else if (strstr(name, "_Horse") != 0)
    {
        strcpy(dealname, "马");
        按钮.VehicleID = 52;
    }
    else if (strstr(name, "_02_C") != 0)
    {
        strcpy(dealname, "雪地摩托车");
        按钮.VehicleID = 46;
    }
    else if (strstr(name, "Snowmobile_C") != 0)
    {
        strcpy(dealname, "雪地摩托车");
        按钮.VehicleID = 47;
    }
    else if (strstr(name, "VH_BBCar_C") != 0)
    {
        strcpy(dealname, "波波车");
        按钮.VehicleID = 54;
    }
    else if (strstr(name, "_Motorglider_C") != 0)
    {
        strcpy(dealname, "滑翔机");
        按钮.VehicleID = 51;
    }
    else if (strstr(name, "_DesertCar_C") != 0)
    {
        strcpy(dealname, "大脚越野车");
        按钮.VehicleID = 0;
    }
    else if (strstr(name, "_New_C") != 0)
    {
        strcpy(dealname, "赛车");
        按钮.VehicleID = 0;
    }
    else if (strstr(name, "VH_Blanc_C") != 0)
    {
        strcpy(dealname, "布朗SUV");
        按钮.VehicleID = 0;
    }
    else if (strstr(name, "VH_Picobus_C") != 0)
    {
        strcpy(dealname, "电动巴士");
        按钮.VehicleID = 0;
    }
    else if (strstr(name, "VH_DumpTrugk_C") != 0)
    {
        strcpy(dealname, "大运");
        按钮.VehicleID = 0;
    }
    else if (strstr(name, "VH_LostMobile_C") != 0)
    {
        strcpy(dealname, "霹雳车");
        按钮.VehicleID = 0;
    }
    else
    {
        strcpy(dealname, "Error");
    }
    return dealname;
}

string 绘制::getBoxName(int id) {
    if (按钮.狙击枪械) {
        if (id == 107001)
            return "十字弩";
        if (id == 103003)
            return "AWM";
        if (id == 103001)
            return "Kar98K";
        if (id == 103002)
            return "M24";
        if (id == 103006)
            return "Mini14";
        if (id == 103008)
            return "Win94";
        if (id == 103009)
            return "SLR";
        if (id == 103012)
            return "AMR";
        if (id == 103013)
            return "M417";
        if (id == 103014)
            return "MK20-H";
        if (id == 103100)
            return "MK12";
        if (id == 103010)
            return "QBU";
        if (id == 103004)
            return "SKS";
        if (id == 103007)
            return "MK14";
        if (id == 103015)
            return "M200";
        if (id == 103011)
            return "莫辛纳甘";   
        if (id == 103017)
            return "M1加兰德";
        if (id == 103016)
            return "SVD狙击枪";
    }
    if (按钮.显示步枪) {
        if (id == 101012)
            return "蜜獾";
        if (id == 101008)
            return "M762";
        if (id == 101011)
            return "VC-VAL";
        if (id == 101010)
            return "G36C";
        if (id == 101013)
            return "Famas";
        if (id == 105002)
            return "DP28";
        if (id == 101005)
            return "狗砸";
        if (id == 101001)
            return "AKM";
        if (id == 101007)
            return "QBZ";
        if (id == 101009)
            return "MK47";
        if (id == 101006)
            return "AUG";
        if (id == 101002)
            return "M16A4";
        if (id == 101003)
            return "SCAR";
        if (id == 101004)
            return "M416";
        if (id == 105001)
            return "M249";
        if (id == 105012)
            return "PKM";
        if (id == 105010)
            return "MG3";
        if (id == 101016)
            return "ARX";
        if (id == 101014)
            return "ACE32";
    }
    if (按钮.冲锋枪械) {
        if (id == 102001)
            return "UZI";
        if (id == 102004)
            return "汤姆逊";
        if (id == 102003)
            return "维克托";
        if (id == 102002)
            return "UMP9";
        if (id == 103005)
            return "VSS";
        if (id == 102005)
            return "野牛";
        if (id == 102007)
            return "MP5K";
        if (id == 102105)
            return "P90";
        if (id == 102009)
            return "JS9";
        if (id == 102008)
            return "AKS-74U";
    }
    if (按钮.散弹枪械) {
        if (id == 104003)
            return "S12K";
        if (id == 104002)
            return "S1897";
        if (id == 104004)
            return "DBS";
        if (id == 104100)
            return "SPAS-12";
        if (id == 104005)
            return "AA12-G";
    }

    return "Error";
}

float 绘制::陀螺仪计算(float fov)
{
    float 阿念の陀螺仪预设 = 250.0f;
  
    按钮.不开镜灵敏度 = (30.0f * 600.0f) / 按钮.不开镜基准值;
    按钮.机瞄红点灵敏度 = (27.0f * 600.0f) / 按钮.机瞄红点基准值;
    按钮.二倍灵敏度 = (16.0f * 600.0f) / 按钮.二倍基准值;
    按钮.三倍灵敏度 = (9.0f * 600.0f) / 按钮.三倍基准值;
    按钮.四倍灵敏度 = (7.0f * 600.0f) / 按钮.四倍基准值;
    按钮.六倍灵敏度 = (4.0f * 600.0f) / 按钮.六倍基准值;
    按钮.八倍灵敏度 = (1.0f * 600.0f) / 按钮.八倍基准值;
    
    按钮.不开镜灵敏度 = std::max(10.0f, std::min(100.0f, 按钮.不开镜灵敏度));
    按钮.机瞄红点灵敏度 = std::max(8.0f, std::min(80.0f, 按钮.机瞄红点灵敏度));
    按钮.二倍灵敏度 = std::max(5.0f, std::min(60.0f, 按钮.二倍灵敏度));
    按钮.三倍灵敏度 = std::max(3.0f, std::min(40.0f, 按钮.三倍灵敏度));
    按钮.四倍灵敏度 = std::max(2.0f, std::min(30.0f, 按钮.四倍灵敏度));
    按钮.六倍灵敏度 = std::max(1.0f, std::min(20.0f, 按钮.六倍灵敏度));
    按钮.八倍灵敏度 = std::max(0.5f, std::min(10.0f, 按钮.八倍灵敏度));

    if (fov > 75 && fov <= 130)
    {
        return 按钮.不开镜灵敏度 / 阿念の陀螺仪预设;
    } else if (fov == 70 || fov == 75)
    {
        return 按钮.机瞄红点灵敏度 / 阿念の陀螺仪预设;
    } else if (fov == 55 || fov == 60)
    {
        return 按钮.机瞄红点灵敏度 / 阿念の陀螺仪预设;
    } else if ((int)fov == 44)
    {
        return 按钮.二倍灵敏度 / 阿念の陀螺仪预设;
    } else if ((int)fov == 26)
    {
        return 按钮.三倍灵敏度 / 阿念の陀螺仪预设;
    } else if ((int)fov == 20)
    {
        return 按钮.四倍灵敏度 / 阿念の陀螺仪预设;
    } else if ((int)fov == 13)
    {
        return 按钮.六倍灵敏度 / 阿念の陀螺仪预设;
    } else if ((int)fov == 11)
    {
        return 按钮.八倍灵敏度 / 阿念の陀螺仪预设;
    }

    return 按钮.不开镜灵敏度 / 阿念の陀螺仪预设;
}