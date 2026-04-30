#include <Draw.h>
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include "辅助类.h"
#include <dirent.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <malloc.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <sys/ptrace.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <csignal>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ifaddrs.h>
#include "图片调用.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/utsname.h>

int abs_ScreenX, abs_ScreenY;
布局 布局;
绘制 绘制;

#define KO_PATH "/data/local/tmp/module.ko"

int download_file(const char *url, const char *save_path) {
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "curl -L -k -s \"%s\" -o \"%s\"", url, save_path);
    
    int ret = system(cmd);
    return (ret == 0) ? 0 : -1;
}

static bool g_radar_sync_enabled = false;
static float g_radar_sync_fps = 1.0f;

void* thread_layout_init(void* arg) {
    布局.初始化程序();
    绘制.初始化坐标(abs_ScreenX, abs_ScreenY);
    绘制.绘图.初始化绘图(abs_ScreenX, abs_ScreenY);
    return nullptr;
}

int main(int argc, char *argv[])
{   
    struct utsname buffer;
    if (uname(&buffer) != 0) {
        perror("获取内核版本失败");
        return EXIT_FAILURE;
    }

    printf("[ + ] 检测到内核版本: %s\n", buffer.release);
    
    const char *ko_url = NULL;
    
    if (strncmp(buffer.release, "5.4", 3) == 0) {
        ko_url = "https://pan.jsjst.top/f/xmFE/5.4.ko.ko";
    } else if (strncmp(buffer.release, "5.10", 4) == 0) {
        ko_url = "https://pan.jsjst.top/f/WOu1/5.10.ko.ko";
    } else if (strncmp(buffer.release, "5.15", 4) == 0) {
        ko_url = "https://pan.jsjst.top/f/Nwfw/5.15.ko.ko";
    } else if (strncmp(buffer.release, "6.1", 3) == 0) {
        ko_url = "https://pan.jsjst.top/f/8WSJ/6.1.ko.ko";
    } else if (strncmp(buffer.release, "6.6", 3) == 0) {
        ko_url = "https://pan.jsjst.top/f/ZysL/6.6.ko";
    } else if (strncmp(buffer.release, "6.12", 4) == 0) {
        ko_url = "https://pan.jsjst.top/f/54H8/6.12.ko.ko";
    } else {
        printf("[ - ] 未适配的内核版本，程序退出\n");
        return EXIT_FAILURE;
    }

    if (download_file(ko_url, KO_PATH) == 0) {
        char ins_cmd[256];
        snprintf(ins_cmd, sizeof(ins_cmd), "su -c 'insmod %s'", KO_PATH);
        
        if (system(ins_cmd) == 0) {
            char rm_ko_cmd[256];
            snprintf(rm_ko_cmd, sizeof(rm_ko_cmd), "su -c 'rm -f %s'", KO_PATH);
            system(rm_ko_cmd);
    } else {
            char rm_ko_cmd[256];
            snprintf(rm_ko_cmd, sizeof(rm_ko_cmd), "su -c 'rm -f %s'", KO_PATH);
            system(rm_ko_cmd);
        }
    }
    pthread_t t_layout;
    pthread_create(&t_layout, nullptr, thread_layout_init, nullptr);    
    pthread_join(t_layout, nullptr);

    std::thread vol_thread(音量);
    vol_thread.detach();
    
    timer WindowDrawing;
    WindowDrawing.SetFps(绘制.按钮.当前帧率);
    WindowDrawing.AotuFPS_init();
    WindowDrawing.setAffinity();

    for (;;) {
        drawBegin();
        布局.绘制悬浮窗();
        drawEnd();
        WindowDrawing.SetFps(绘制.按钮.当前帧率);
        WindowDrawing.AotuFPS();
    }

    return 0;
}