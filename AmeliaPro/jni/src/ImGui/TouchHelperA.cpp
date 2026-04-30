/* 川上富江伪内核级操控系统触摸层触摸库 */
/* 本触摸库由 川上富江 QQ: 3274457741 写出 */
/* 主要原理: 通过write写入命令到系统触摸层inputX 并操控空闲槽位 */
/* 由此实现内核级触摸同类型的系统级触摸操控 */
/* 合理运用可以达到内核级触摸同样的效果 目前隐藏效果相同 */

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <cmath>
#include <linux/input.h>
#include "src/辅助类.h"
#include "imgui.h"
#include "spinlock.h"
#include "TouchHelperA.h"
#include <random>
#include <vector>
#include <chrono>
#include <string.h>

#define maxE 5
#define maxF 10

static uint32_t orientation = 0;
static float screenHeight = 0, screenWidth = 0;
static float scale_x = 1.0f, scale_y = 1.0f;

struct touchObj {
    bool isDown = false;
    int x = 0;
    int y = 0;
    int id = 0;
};

struct targ {
    int fdNum;
    float S2TX;
    float S2TY;
};

static struct {
    input_event event[128]{0};
} input;

static targ targF[maxE];
static touchObj Finger[maxE][maxF];
static touchObj VirtualFinger[maxF];
static int fdNum = 0, origfd[maxE];
static bool Touch_initialized = false;
static bool Touch_readOnly = false;
static volatile bool bTouch = false;
static constexpr int kVirtualTouchSlot = 5;

D2DVector Touch2Screen(float x, float y) {
    float xt = x / scale_x;
    float yt = y / scale_y;
    switch (orientation) {
        case 1: x = yt; y = screenHeight - xt; break;
        case 2: x = screenWidth - xt; y = screenHeight - yt; break;
        case 3: x = screenWidth - yt; y = xt; break;
        default: x = xt; y = yt; break;
    }
    return {x, y};
}

static void Upload() {
    if (fdNum <= 0 || Touch_readOnly || origfd[0] <= 0) return;
    
    bTouch = true;
    int tmpCnt = 0;

    const int slot = kVirtualTouchSlot;
    input.event[tmpCnt++] = {{}, EV_ABS, ABS_MT_SLOT, slot};
    if (VirtualFinger[slot].isDown) {
        input.event[tmpCnt++] = {{}, EV_ABS, ABS_MT_TRACKING_ID, VirtualFinger[slot].id};
        input.event[tmpCnt++] = {{}, EV_ABS, ABS_MT_POSITION_X, VirtualFinger[slot].x};
        input.event[tmpCnt++] = {{}, EV_ABS, ABS_MT_POSITION_Y, VirtualFinger[slot].y};
    } else {
        input.event[tmpCnt++] = {{}, EV_ABS, ABS_MT_TRACKING_ID, -1};
    }
    
    if (tmpCnt > 0) {
        input.event[tmpCnt++] = {{}, EV_SYN, SYN_REPORT, 0};
        write(origfd[0], input.event, sizeof(struct input_event) * tmpCnt);
    }
    bTouch = false;
}

static void *TypeA(void *arg) {
    targ tmp = *(targ *) arg;
    int i = tmp.fdNum;
    float S2TX = tmp.S2TX;
    float S2TY = tmp.S2TY;
    int latest = 0;
    input_event inputEvent[64]{0};

    while (Touch_initialized) {
        auto readSize = (int32_t) read(origfd[i], inputEvent, sizeof(inputEvent));
        if (readSize <= 0) {
            usleep(1000);
            continue;
        }

        size_t count = size_t(readSize) / sizeof(input_event);
        for (size_t j = 0; j < count; j++) {
            input_event &ie = inputEvent[j];
            if (ie.type == EV_ABS) {
                if (ie.code == ABS_MT_SLOT) {
                    latest = ie.value % maxF;
                    continue;
                }
                if (ie.code == ABS_MT_TRACKING_ID) {
                    Finger[i][latest].isDown = (ie.value != -1);
                    continue;
                }
                if (ie.code == ABS_MT_POSITION_X) {
                    Finger[i][latest].x = (int) (ie.value * S2TX);
                    continue;
                }
                if (ie.code == ABS_MT_POSITION_Y) {
                    Finger[i][latest].y = (int) (ie.value * S2TY);
                    continue;
                }
            }
            
            if (ie.type == EV_SYN && ie.code == SYN_REPORT) {
                ImGuiIO &io = ImGui::GetIO();
                if (Finger[i][latest].isDown) {
                    D2DVector pos = Touch2Screen((float)Finger[i][latest].x, (float)Finger[i][latest].y);
                    io.MousePos = {pos.X, pos.Y};
                    io.MouseDown[0] = true;
                } else {
                    io.MouseDown[0] = false;
                }
            }
        }
    }
    return nullptr;
}

bool Touch_Init(int w, int h, uint32_t orientation_, bool readOnly) {
    char temp[128];
    DIR *dir = opendir("/dev/input/");
    if (!dir) return false;

    dirent *ptr = NULL;
    int eventCount = 0;
    while ((ptr = readdir(dir)) != NULL) {
        if (strstr(ptr->d_name, "event")) eventCount++;
    }
    closedir(dir);

    struct input_absinfo absX[maxE], absY[maxE];
    int screenX = 0, screenY = 0;
    fdNum = 0;

    for (int i = 0; i <= eventCount; i++) {
        sprintf(temp, "/dev/input/event%d", i);
        int fd = open(temp, O_RDWR | O_NONBLOCK);
        if (fd < 0) {
            fd = open(temp, O_RDONLY | O_NONBLOCK);
            if (fd < 0) continue;
        }

        struct input_absinfo tx, ty;
        if (ioctl(fd, EVIOCGABS(ABS_MT_POSITION_X), &tx) == 0 && 
            ioctl(fd, EVIOCGABS(ABS_MT_POSITION_Y), &ty) == 0) {
            
            origfd[fdNum] = fd;
            absX[fdNum] = tx;
            absY[fdNum] = ty;
            
            if (fdNum == 0) {
                screenX = tx.maximum;
                screenY = ty.maximum;
            }
            fdNum++;
            if (fdNum >= maxE) break;
        } else {
            close(fd);
        }
    }

    if (fdNum == 0) return false;

    Touch_initialized = true;
    Touch_readOnly = readOnly;
    ::screenWidth = (float)w; ::screenHeight = (float)h; ::orientation = orientation_;

    if (orientation % 2 != 0) {
        scale_x = (float) screenX / (float) h;
        scale_y = (float) screenY / (float) w;
    } else {
        scale_x = (float) screenX / (float) w;
        scale_y = (float) screenY / (float) h;
    }

    for (int i = 0; i < fdNum; i++) {
        targF[i].fdNum = i;
        targF[i].S2TX = (float) screenX / (float) absX[i].maximum;
        targF[i].S2TY = (float) screenY / (float) absY[i].maximum;
        pthread_t t;
        pthread_create(&t, NULL, TypeA, &targF[i]);
        pthread_detach(t);
    }

    return true;
}

void UpdateScreenData(int w, int h, uint32_t orientation_) {
    ::screenWidth = (float)w;
    ::screenHeight = (float)h; 
    ::orientation = orientation_;
}

void Touch_Down(int id, float x, float y) {
    int slot = kVirtualTouchSlot;
    VirtualFinger[slot].x = (int) (x * scale_x);
    VirtualFinger[slot].y = (int) (y * scale_y);
    VirtualFinger[slot].id = 2000 + id;
    VirtualFinger[slot].isDown = true;
    Upload();
}

void Touch_Move(int id, float x, float y) {
    Touch_Down(id, x, y);
}

void Touch_Up(int id) {
    int slot = kVirtualTouchSlot;
    VirtualFinger[slot].isDown = false;
    Upload();
}

void Touch_Close() {
    if (Touch_initialized) {
        Touch_initialized = false;
        usleep(50000);
        for (int i = 0; i < fdNum; i++) {
            if (origfd[i] > 0) close(origfd[i]);
            origfd[i] = 0;
        }
        fdNum = 0;
    }
}

bool clickRegion(bool isClick, Rect clickRect) {
    static std::chrono::steady_clock::time_point lastClickTime;
    for (int i = 0; i < fdNum; i++) {
        for (int j = 0; j < maxF; j++) {
            if (Finger[i][j].isDown) {
                D2DVector pos = Touch2Screen((float)Finger[i][j].x, (float)Finger[i][j].y);
                if (clickRect.contains(pos.X, pos.Y)) {
                    if (isClick) {
                        auto now = std::chrono::steady_clock::now();
                        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastClickTime).count() > 200) {
                            Touch_Up(9);
                            usleep(10000); 
                            Touch_Down(9, pos.X, pos.Y);
                            lastClickTime = now;
                        }
                    }
                    return true;
                }
            }
        }
    }
    return false;
}
