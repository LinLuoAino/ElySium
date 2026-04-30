#ifndef GRYO_H
#define GRYO_H
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include "辅助类.h"
#include "Gryo_Module.h"
#include "../Ptrace/PtraceUtils.h"

struct sGyro
{
    bool enable;
    float x;
    float y;
};

class FGyro
{
private:
    Kernel Sys;
    uint64_t GryoAddr;
    sGyro gyro;
    static inline FGyro *instance = nullptr;

    uint64_t find_maps_vm(pid_t pid, const char *vm_name)
    {
        char maps_path[64];
        snprintf(maps_path, sizeof(maps_path), "/proc/%d/maps", pid);
        FILE *fp = fopen(maps_path, "r");
        if (!fp)
        {
            return 0;
        }
        char line[512];
        uint64_t start_addr = 0;
        while (fgets(line, sizeof(line), fp))
        {
            if (strstr(line, vm_name))
            {
                if (sscanf(line, "%lx-", &start_addr) == 1)
                {
                    break;
                }
            }
        }
        fclose(fp);
        return start_addr;
    }
    
    bool write_and_inject()
    {
        unsigned int Gryo_Len = sizeof(Gryo_Module);
        const char *file_path = "/data/app/libaa.so";
        int fd = open(file_path, O_WRONLY | O_CREAT | O_TRUNC, 0755);
        if (fd < 0)
        {
            return false;
        }
        ssize_t written = write(fd, Gryo_Module, Gryo_Len);
        close(fd);
        if (written != (ssize_t)Gryo_Len)
        {
            return false;
        }
        chmod(file_path, 0777);
        int result = PtraceInject("system_server", "/data/app/libaa.so");
        if (unlink(file_path) == 0)
        {
        }
        else
        {
            return false;
        }
        return true;
    }

public:
    bool Init()
    {
    Gryo_Start:
        pid_t pid = Sys.getPID("system_server");
        Sys.初始化读写(pid);
        if (pid <= 0)
            return false;
        GryoAddr = find_maps_vm(pid, "FuckMem");
        if (GryoAddr)
        {
            Unlock_Event();
            Sys.read(GryoAddr, &gyro, sizeof(sGyro));
            return true;
        }
        if (!GryoAddr)
        {
            if (write_and_inject())
                goto Gryo_Start;
        }
        return false;
    }

    void Send_Event(float x, float y)
    {
        if (GryoAddr)
        {
            gyro.enable = true;
            gyro.x = x;
            gyro.y = y;
            Sys.writev(GryoAddr, &gyro, sizeof(sGyro));
        }
    }

    void Unlock_Event()
    {
        if (GryoAddr)
        {
            gyro.enable = false;
            gyro.x = 0;
            gyro.y = 0;
            Sys.writev(GryoAddr, &gyro, sizeof(sGyro));
        }
    }

    sGyro GetGyroInfo() const
    {
        return gyro;
    }

    static FGyro *GetInstance()
    {
        if (!instance)
        {
            instance = new FGyro();
        }
        return instance;
    }
};

static FGyro Gyro;

void GTA(uint64_t arg1, uint64_t arg2, float *out_result);
void GTB(float arg1, float arg2, float *out_result);

#endif