#ifndef 辅助类_H
#define 辅助类_H

#include <iostream>
#include <pthread.h>
#include <dirent.h>
#include <regex.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <vector>
#include <map>
#include <cstdio>
#include <stdio.h>
#include <sys/syscall.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "timer.h"
#include "obfuscate.h"
#include <sys/uio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>

#define PI 3.141592653589793238

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <ctime>
#include <arpa/inet.h>
#include <netinet/in.h>

struct MemSeg {
    uintptr_t start;
    uintptr_t end;
};

class Kernel
{
public:
    int has_upper = 0;
    int has_lower = 0;
    int has_symbol = 0;
    int has_digit = 0;
    int fd;
    pid_t pid;
    bool 断点;
    
    typedef struct _COPY_MEMORY
    {
        pid_t pid;
        uintptr_t addr;
        void *buffer;
        size_t size;
    } COPY_MEMORY, *PCOPY_MEMORY;

    typedef struct _HIDE_PROCESS
    {
        pid_t pid;
    } HIDE_PROCESS, *PHIDE_PROCESS;

    enum OPERATIONS
    {
        /*OP_INIT_KEY = 0x800,
        OP_READ_MEM = 0x801,
        OP_WRITE_MEM = 0x802,
        OP_MODULE_BASE = 0x803,
        OP_HWBP_CLEAR = 0x804,
        OP_NO_RECOIL = 0x805,
        OP_HIDE_PROCESS = 0x806,
        OP_RECOVER_PROCESS = 0x807,*/
        
        OP_INIT_KEY = 600,
        OP_READ_MEM = 601,
        OP_WRITE_MEM = 602,
        OP_MODULE_BASE = 603,
        OP_HWBP_CLEAR = 608,
        OP_NO_RECOIL = 605,
        OP_HIDE_PROCESS = 606,
        OP_RECOVER_PROCESS = 607,
    };

public:
    Kernel();
    ~Kernel();
    
    char *driver_path()
	{
		const char *dev_path = "/dev";
		DIR *dir = opendir(dev_path);
		if (dir == NULL)
		{
			return NULL;
		}

		const std::vector<std::string> excluded_names = {
			"binder", "common", "ashmem", "stdin", "stdout", "stderr"};

		struct dirent *entry;
		char *file_path = NULL;
		while ((entry = readdir(dir)) != NULL)
		{
			const char *current_name = entry->d_name;

			if (strcmp(current_name, ".") == 0 || strcmp(current_name, "..") == 0)
			{
				continue;
			}

			if (strstr(current_name, "gpiochip") != NULL ||
				strchr(current_name, '_') != NULL ||
				strchr(current_name, '-') != NULL ||
				strchr(current_name, ':') != NULL)
			{
				continue;
			}

			bool is_excluded = false;
			for (const auto &name : excluded_names)
			{
				if (strcmp(current_name, name.c_str()) == 0)
				{
					is_excluded = true;
					break;
				}
			}
			if (is_excluded)
			{
				continue;
			}

			size_t path_length = strlen(dev_path) + strlen(current_name) + 2;
			file_path = (char *)malloc(path_length);
			if (!file_path)
				continue;

			snprintf(file_path, path_length, "%s/%s", dev_path, current_name);

			struct stat file_info;
			if (stat(file_path, &file_info) < 0)
			{
				free(file_path);
				file_path = NULL;
				continue;
			}

			if (S_ISCHR(file_info.st_mode) || S_ISBLK(file_info.st_mode))
			{
				if (localtime(&file_info.st_ctime)->tm_year + 1900 <= 1980)
				{
					free(file_path);
					file_path = NULL;
					continue;
				}

				if (file_info.st_atime == file_info.st_ctime &&
					file_info.st_size == 0 &&
					file_info.st_gid == 0 &&
					file_info.st_uid == 0 &&
					strlen(current_name) == 6)
				{
					closedir(dir);
					return file_path;
				}
			}

			free(file_path);
			file_path = NULL;
		}

		closedir(dir);
		return NULL;
	}
	
	void GetAllTids(int pid, std::vector<int>& vOutput) {
        vOutput.clear();
        char szTaskPath[256] = {0};
        sprintf(szTaskPath, "/proc/%d/task", pid);
        DIR* dir = opendir(szTaskPath);
        if (dir != NULL) {
            struct dirent* ptr = NULL;
            while ((ptr = readdir(dir)) != NULL) {
                if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0)) continue;
                if (ptr->d_type != DT_DIR) continue;
                vOutput.push_back(atoi(ptr->d_name));
            }
            closedir(dir);
        }
    }
    
    void 初始化读写(pid_t pid);
    bool read(uintptr_t addr, void *buffer, size_t size);
    bool writev(uintptr_t addr, void *buffer, size_t size);
    
    template <typename T>
    T read(uintptr_t addr) {
        T data{};
        if (read(addr, &data, sizeof(T))) {
            return data;
        }
        return T{};
    }
    
    uintptr_t get_module_base(char *name);
    uintptr_t getPtr64(uintptr_t addr);
    uintptr_t getPtr32(uintptr_t addr);
    
    float getFloat(uintptr_t addr);
    int getDword(uintptr_t addr);
    int getWord(uintptr_t addr);
    char getByte(uintptr_t addr);
    
    int WriteDword(uintptr_t addr, int value);
    float WriteFloat(uintptr_t addr, float value);
    bool writeFloat(uintptr_t addr, float data);
    
    int getPID(const char *packageName);
    void getUTF8(char *buf, uintptr_t namepy);

    int add_breakpoint(uintptr_t addr, int type, int len);
    bool remove_breakpoint(uintptr_t addr);
    bool set_hwbp_track(int tid, uintptr_t target_addr);
    void clear_all();
};

#include "Draw.h"
#include "结构体.h"

struct smokeObject
{
    float screenDistance;
    Vec3 worldPos;
    Vec2 screenPos[8];
};
extern std::vector<smokeObject> smokeObjects;
extern std::vector<smokeObject> aimSmokeObjects;
class StringFloatMap
{
private:
    map<string, vector<float>> data;

public:
    void add(string key, float value1, float value2)
    {
        vector<float> values;
        values.push_back(value1);
        values.push_back(value2);
        data[key] = values;
    }

    void remove(string key)
    {
        data.erase(key);
    }

    bool exists(string key)
    {
        return data.find(key) != data.end();
    }

    string calculateKey(float 坐标X, float 坐标Y)
    {
        int maps = 999;
        string 键名;
        for (const auto &pair : data)
        {
            const vector<float> &values = pair.second;
            int result = (int)sqrt(pow(坐标X - values[0], 2) + pow(坐标Y - values[1], 2)) * 0.01;
            // printf("测试值:%f \n",result);
            if (result < maps)
            {
                // return pair.first;
                maps = result;
                键名 = pair.first;
                // break;
            }
        }
        if (maps < 20)
        {
            return 键名;
        }
        return ""; // 如果没有满足条件的键名，则返回空字符串
    }
};

class Timer
{
private:
    std::map<std::string, int> timers;

public:
    void addTimer(const std::string &name, int seconds)
    {
        if (timers.find(name) == timers.end())
        {
            timers[name] = seconds;
        }
    }

    void updateTimers()
    {
        for (auto &timer : timers)
        {
            timer.second++;
        }
    }

    void checkAndRemoveTimers()
    {
        for (auto it = timers.begin(); it != timers.end();)
        {
            if (it->second == 1500)
            {
                it = timers.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    int getTimerSeconds(const std::string &name)
    {
        auto it = timers.find(name);
        if (it != timers.end())
        {
            return it->second;
        }
        return -1;
    }

    void removeTimer(const std::string &name)
    {
        timers.erase(name);
    }

    bool hasTimer(const std::string &name)
    { // 判断是否存在
        return timers.find(name) != timers.end();
    }

    void renameTimer(const std::string &oldName, const std::string &newName)
    {
        if (timers.find(oldName) != timers.end())
        {
            timers[newName] = timers[oldName];
            timers.erase(oldName);
        }
    }
};

class 计算
{
public:
    int 计算距离(D3DVector 自身坐标, D3DVector 对方坐标);
    D4DVector 计算屏幕坐标(float 矩阵[16], D3DVector 人物坐标, float px, float py);
    D2DVector 计算屏幕坐标2(float 矩阵[16], D3DVector 人物坐标, float px, float py);
    float 计算屏幕距离(D2DVector &坐标, float px, float py);
    骨骼数据 计算骨骼(float (&矩阵)[16], D3DVector (&骨骼)[17], float px, float py);
    D2DVector rotateCoord(float angle, float objRadar_x, float objRadar_y);
    D2DVector rotateCoord(D3DVector Enemy, D3DVector RealPerson);
};
// 绘图
class 绘图
{
public:
    struct 颜色
    {
        ImColor 红色 = ImColor(255, 0, 0, 255);
        ImColor 白色 = ImColor(255, 255, 255, 255);
        ImColor 蓝色 = ImColor(0, 0, 255, 255);
        ImColor 绿色 = ImColor(0, 255, 0, 255);
        ImColor 黄色 = ImColor(255, 255, 0, 255);
        ImColor 黑色 = ImColor(0, 0, 0, 255);
    };

    struct VecTor2
    {
        float x;
        float y;
        VecTor2()
        {
            this->x = 0;
            this->y = 0;
        }
        VecTor2(float x, float y)
        {
            this->x = x;
            this->y = y;
        }
        
        static float Distance(const VecTor2& a, const VecTor2& b) {
            float dx = a.x - b.x;
            float dy = a.y - b.y;
            return std::sqrt(dx * dx + dy * dy);
        }
        
        bool operator!=(const VecTor2 &Pos)
        {
            if (this->x != Pos.x || this->y != Pos.y)
            {
                return true;
            }
            return false;
        }
        VecTor2 operator+(float v) const
        {
            return VecTor2(x + v, y + v);
        }
        VecTor2 operator-(float v) const
        {
            return VecTor2(x - v, y - v);
        }
        VecTor2 operator*(float v) const
        {
            return VecTor2(x * v, y * v);
        }
        VecTor2 operator/(float v) const
        {
            return VecTor2(x / v, y / v);
        }
        VecTor2 &operator+=(float v)
        {
            x += v;
            y += v;
            return *this;
        }
        VecTor2 &operator-=(float v)
        {
            x -= v;
            y -= v;
            return *this;
        }
        VecTor2 &operator*=(float v)
        {
            x *= v;
            y *= v;
            return *this;
        }
        VecTor2 &operator/=(float v)
        {
            x /= v;
            y /= v;
            return *this;
        }
        VecTor2 operator+(const VecTor2 &v) const
        {
            return VecTor2(x + v.x, y + v.y);
        }
        VecTor2 operator-(const VecTor2 &v) const
        {
            return VecTor2(x - v.x, y - v.y);
        }
        VecTor2 operator*(const VecTor2 &v) const
        {
            return VecTor2(x * v.x, y * v.y);
        }
        VecTor2 operator/(const VecTor2 &v) const
        {
            return VecTor2(x / v.x, y / v.y);
        }
        VecTor2 &operator+=(const VecTor2 &v)
        {
            x += v.x;
            y += v.y;
            return *this;
        }
        VecTor2 &operator-=(const VecTor2 &v)
        {
            x -= v.x;
            y -= v.y;
            return *this;
        }
        VecTor2 &operator*=(const VecTor2 &v)
        {
            x *= v.x;
            y *= v.y;
            return *this;
        }
        VecTor2 &operator/=(const VecTor2 &v)
        {
            x /= v.x;
            y /= v.y;
            return *this;
        }
        
    };
    struct VecTor3
    {
        float x;
        float y;
        float z;
        
        float Length() const {
            return sqrtf(x*x + y*y + z*z);
        }
        
        VecTor3()
        {
            this->x = 0;
            this->y = 0;
            this->z = 0;
        }
        VecTor3(float x, float y, float z)
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }
        bool operator!=(const VecTor3 &Pos)
        {
            if (this->x != Pos.x || this->y != Pos.y || this->z != Pos.z)
            {
                return true;
            }
            return false;
        }
        VecTor3 operator+(float v) const
        {
            return VecTor3(x + v, y + v, z + v);
        }
        VecTor3 operator-(float v) const
        {
            return VecTor3(x - v, y - v, z - v);
        }
        VecTor3 operator*(float v) const
        {
            return VecTor3(x * v, y * v, z * v);
        }
        VecTor3 operator/(float v) const
        {
            return VecTor3(x / v, y / v, z / v);
        }
        bool operator==(const VecTor3& other) const {
            return x == other.x && y == other.y && z == other.z;
        }    
        bool operator<(const VecTor3& other) const {
            if (x != other.x) return x < other.x;
            if (y != other.y) return y < other.y;
            return z < other.z;
        }        
        VecTor3 &operator+=(float v)
        {
            x += v;
            y += v;
            z += v;
            return *this;
        }
        VecTor3 &operator-=(float v)
        {
            x -= v;
            y -= v;
            z -= v;
            return *this;
        }
        VecTor3 &operator*=(float v)
        {
            x *= v;
            y *= v;
            z *= v;
            return *this;
        }
        VecTor3 &operator/=(float v)
        {
            x /= v;
            y /= v;
            z /= v;
            return *this;
        }
        VecTor3 operator+(const VecTor3 &v) const
        {
            return VecTor3(x + v.x, y + v.y, z + v.z);
        }
        VecTor3 operator-(const VecTor3 &v) const
        {
            return VecTor3(x - v.x, y - v.y, z - v.z);
        }
        VecTor3 operator*(const VecTor3 &v) const
        {
            return VecTor3(x * v.x, y * v.y, z * v.z);
        }
        VecTor3 operator/(const VecTor3 &v) const
        {
            return VecTor3(x / v.x, y / v.y, z / v.z);
        }
        VecTor3 &operator+=(const VecTor3 &v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }
        VecTor3 &operator-=(const VecTor3 &v)
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            return *this;
        }
        VecTor3 &operator*=(const VecTor3 &v)
        {
            x *= v.x;
            y *= v.y;
            z *= v.z;
            return *this;
        }
        VecTor3 &operator/=(const VecTor3 &v)
        {
            x /= v.x;
            y /= v.y;
            z /= v.z;
            return *this;
        }
    };

    struct VecTor4
    {
        float x;
        float y;
        float z;
        float w;
        VecTor4()
        {
            this->x = 0;
            this->y = 0;
            this->z = 0;
            this->w = 0;
        }
        VecTor4(float x, float y, float z, float w)
        {
            this->x = x;
            this->y = y;
            this->z = z;
            this->w = w;
        }
        bool operator!=(const VecTor4 &Pos)
        {
            if (this->x != Pos.x || this->y != Pos.y || this->z != Pos.z || this->w != Pos.w)
            {
                return true;
            }
            return false;
        }
        VecTor4 operator+(float v) const
        {
            return VecTor4(x + v, y + v, z + v, w + v);
        }
        VecTor4 operator-(float v) const
        {
            return VecTor4(x - v, y - v, z - v, w - v);
        }
        VecTor4 operator*(float v) const
        {
            return VecTor4(x * v, y * v, z * v, w * v);
        }
        VecTor4 operator/(float v) const
        {
            return VecTor4(x / v, y / v, z / v, w / v);
        }
        VecTor4 &operator+=(float v)
        {
            x += v;
            y += v;
            z += v;
            w += v;
            return *this;
        }
        VecTor4 &operator-=(float v)
        {
            x -= v;
            y -= v;
            z -= v;
            w -= v;
            return *this;
        }
        VecTor4 &operator*=(float v)
        {
            x *= v;
            y *= v;
            z *= v;
            w *= v;
            return *this;
        }
        VecTor4 &operator/=(float v)
        {
            x /= v;
            y /= v;
            z /= v;
            w /= v;
            return *this;
        }
        VecTor4 operator+(const VecTor4 &v) const
        {
            return VecTor4(x + v.x, y + v.y, z + v.z, w + v.w);
        }
        VecTor4 operator-(const VecTor4 &v) const
        {
            return VecTor4(x - v.x, y - v.y, z - v.z, w - v.w);
        }
        VecTor4 operator*(const VecTor4 &v) const
        {
            return VecTor4(x * v.x, y * v.y, z * v.z, w * v.w);
        }
        VecTor4 operator/(const VecTor4 &v) const
        {
            return VecTor4(x / v.x, y / v.y, z / v.z, w / v.w);
        }
        VecTor4 &operator+=(const VecTor4 &v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
            w += v.w;
            return *this;
        }
        VecTor4 &operator-=(const VecTor4 &v)
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            w -= v.w;
            return *this;
        }
        VecTor4 &operator*=(const VecTor4 &v)
        {
            x *= v.x;
            y *= v.y;
            z *= v.z;
            w *= v.w;
            return *this;
        }
        VecTor4 &operator/=(const VecTor4 &v)
        {
            x /= v.x;
            y /= v.y;
            z /= v.z;
            w /= v.w;
            return *this;
        }
    };
    
    struct GrenadeTrajectory {
        std::vector<VecTor3> path;
        float animationProgress = 0.0f;
        float lifetime = 0.0f;
        bool isActive = true;
    };

    颜色 颜色; // 颜色的类
    float PX, PY;
    float MIDDLE, BOTTOM, TOP;
    float left, right, top, top1, bottom;
    bool isAiming;
    void 初始化绘图(int X, int Y);
    void 初始化坐标(D4DVector &屏幕坐标, 骨骼数据 &骨骼);
    void 绘制方框(bool isboot);
    void 绘制人数(int 人机, int 真人);
    void 绘制射线(骨骼数据 &骨骼, bool LineOfSightToTab[15]);
    void 绘制血量(float 最大血量, float 当前血量, float 敌人距离);
    void 绘制骨骼(骨骼数据 &骨骼, D4DVector &屏幕坐标, bool LineOfSightToTab[15], int 距离);
    void 绘制手持(int 手持, int 状态, int 手雷);
    void 绘制名字(骨骼数据 &t_骨骼数据, D4DVector &t_屏幕坐标, char *类名, bool 是否掐雷, float 计算_max);
    void 绘制距离(int 当前子弹, int 最大子弹, int 距离, 骨骼数据& t_骨骼数据, D4DVector& t_屏幕坐标);
    void 头甲包显示(骨骼数据& t_骨骼数据, int 敌人头, int 敌人甲, int 敌人包, D4DVector& t_屏幕坐标);
    void 绘制漏手(bool isboot);
    void 绘制自瞄触摸范围(float 触摸范围, float 触摸范围X, float 触摸范围Y);
    void 绘制连点触摸圆点(float 连点X, float 连点Y, float 连点次数, float 连点间隔);
    void 绘制自救(float 自救倒计时);
    void 绘制加粗文本(float size, float x, float y, ImColor color, ImColor color1, const char *str);
    void 绘制字体描边(float size, int x, int y, ImVec4 color, const char *str);
    void 绘制瞄准信息();
    ImTextureID 获取纹理ID(int 纹理索引);

    bool WorldTurnScreen(VecTor2 &Screen, VecTor3 World, float Matrix[]);

    void ExplosionRange(D3DVector Obj, ImColor color, float Range, float thickn, float Matrix[]);

    void Parabola(VecTor3 obj, float Matrix[]);
    static void UpdateTrajectories(float deltaTime);
    static std::map<VecTor3, GrenadeTrajectory> grenadeTrajectories;
    static const int MAX_PATH_POINTS = 50;
    static const float ANIMATION_SPEED;
    static const float TRAJECTORY_LIFETIME;    
    static ImColor GetGrenadeColor(float progress);
    static void DrawSmoothTrail(const std::vector<VecTor2>& screenPath, float animationProgress);
    static void DrawHeadEffect(VecTor2 position, float pulse);
};
// 绘制

class 念宝
{
public:
    bool 单透模式 = false;
    float 雷达大小 = 150;
    float 雷达缩放 = 1.0f;
    float 雷达大致 = 1.0f;
    bool 同行识别开关 = false;
    bool 同行识别连接失败 = false;
    int 当前子弹数量;
    int 自瞄方案 = 0;
    int 漏打方案 = 0;
    bool 射手自瞄 = false;
    bool 弓弩自瞄 = false;
    float 趴下压枪力度 = 0.65f;
    float 蹲下压枪力度 = 1.85f;
    float 喷子压枪力度 = 3.50f;
    uintptr_t CameraCache;
    uintptr_t CameraCachebjolu;
    bool BoneVis[15];
    bool 连点位置 = false;
    bool 连点自瞄 = false;
    float 连点范围X = 1600.0f;
    float 连点范围Y = 750.0f;
    float 连点范围 = 200.0f;
    int 连点次数 = 20;
    int 连点间隔 = 60;
    int 喵UID呦;
    bool 防录屏 = false;
    bool 无后台 = false;
    bool 高级人机;
    bool 手雷圆圈;
    bool 手雷轨迹;
    
    D3DVector SelfCameraCoordinate;
};

class 绘制
{
    bool SmokeLine(D2DVector Poin, float distance);
    struct ColorTable {
        ImColor 方框颜色 = ImColor(255, 255, 255, 255);
        ImColor 骨骼颜色 = ImColor(255, 0, 0, 255);
        ImColor 射线颜色 = ImColor(255, 255, 255, 255);
        ImColor 血量颜色 = ImColor(0, 255, 0, 255);
        ImColor 阵营颜色 = ImColor(255, 255, 0, 255);
        ImColor 距离颜色 = ImColor(255, 255, 255, 255);
        ImColor 名称颜色 = ImColor(255, 255, 255, 255);
    };

    struct map_node
    {
        long int start_addr;   // 起始地址
        long int end_addr;     // 结束地址
        struct map_node *next; // 下一个节点
    };

    struct 压枪
    {
        float m416;
        float M338;
        float scar_l;
        float aug;
        float famas;
        float g36c;
        float m249;
        float akm;
        float m762;
        float 蜜獾;
        float pkm;
        float mg3;
        float mg_36;
        float groza;
        float qbz;
        float mk47;
        float ace32;
        float p90;
        float uzi;
        float ump45;
        float vector;
        float 汤姆逊;
        float 野牛;
        float mp5k;
        float js9;
        float mp7;
        float s1897;
        float s686;
        float spas12;
        float mini14;
        float sks;
        float m417;
        float mk20_h;
        float mk12;
        float mk14;
        float vss;
        float slr;
        float qbu;
        float awm;
        float m24;
        float kar98;
        float mosinnagant;
        float lynx_amr;
        float m110;
        float m338;
        float dp28;
        float m134;
        float m16a4;
        float acval;
        float type89;
        float arx;
        float qbz191;
        float an94;
        float aks74u;
        float win94;
        float mosin;
        float amr;
        float m200;        // 103015
        float svd;         // 103016
        float m1garand;    // 103017
        float as50;        // 103018
        float mk3;         // 103019
        float s12k;        // 104003
        float dbs;         // 104004
        float aa12g;       // 104005
        float 扫车 = 1.2f; 
    };

public:
    Vec2 WorldToScreen(Vec3 obj);
    bool AnianSmoke(D3DVector Pos);
    float mk20, m417 = 1.0f;
    float 轻型压枪力度 = 1.f;
    float 拇指压枪力度 = 0.2f;
    float 垂直压枪力度 = 0.2f;
    float 直角压枪力度 = 0.2f;
    int 距离字体大小 = 18;
    int 名称字体大小 = 18;
    int 手持字体大小 = 18;
    int 动作字体大小 = 18;
    int 世界数量;
    float 骨骼距离限制 = 300;
    int 漏打模式, 防录屏, 自瞄模式, 无后台开关, 控制延迟;
    char 卡密[250];
    bool 漏打开关;
    bool Winorlose = false;
    ImVec2 Pos;
    int winWidth = 684;
    int winHeith = 896;

    压枪 压枪力;
    压枪 预判度;
    float 握把[100];

    uintptr_t 解密数组;
    long int 解密模式 = 0x4000;

    int 被瞄准对象数量 = 0;

    ColorTable Colorset[2]; // 颜色配置
    int pid;
    bool 线程 = 0; // 用于判断更新数据显示是否加载
    float PX, PY;  // 绘制用的分辨率
    float 真实PX, 真实PY;
    bool Validate;
    Kernel 读写; // 创建读写结构体

    ImColor 物资颜色 = ImColor(0, 255, 0, 255);
    float 车辆颜色 = ImColor(0, 255, 0, 255);
    float 手持颜色 = ImColor(255, 255, 255, 255);

    StringFloatMap 手雷类;
    Timer 计时器; // 计时器

    bool Shelter[14];
    ImFont *font_24 = nullptr; // 24像素字体
    地址 地址;
    开关 按钮;
    计算 计算;
    念宝 念宝;
    绘图 绘图;
    自瞄 自瞄;
    备份 备份;
    std::mutex mtx;
    自瞄信息 自瞄函数[100];
    瞄准信息 被瞄信息[100];
    自身数据 自身数据; // 创建自身数据结构体
    对象地址 对象地址; // 创建对象地址结构体
    对象信息 对象信息; // 创建敌人信息结构体数组
    void 初始化坐标(int 真实X, int 真实Y);
    void 初始化绘制(char* 包名);
    void 自瞄主线程();
    void 陀螺仪自瞄主线程();
    int findminat();
    void 更新地址数据();
    void 更新对象地址();
    void 更新对象数据();
    void 逻辑采集线程();
    void 逻辑处理线程();
    void 模型绘制();
    void 运行绘制();
    ImColor floatArrToImColor(ImColor color);
    string getBoxName(int id);
    float 陀螺仪计算(float fov);
    void OffScreen(ImDrawList *ImDraw, D4DVector Obj, float camear, ImColor color, float Radius, float 距离);
    void GetTouch();
    static string GetFilePath(const char* name) {
        return "/sdcard/Android/" + string(name);
    }
    void ReadsSaveData(ifstream& file);
    void WriteSaveData(ofstream& file);
    void 保存配置(const char* name);
    void 读取配置(const char* name);
    bool 自瞄触发(float 距离);
    const char *getMaterialName(char *name);
    int Cloudcheck();
    const char *Level(char *name);
    int Acquisitionsite();
    D3DVector Missedtyping();
};

class 布局
{
public:
    // 布局UI
    void 开启悬浮窗();
    void 绘制悬浮窗();
    int 初始化程序();
};

#endif
