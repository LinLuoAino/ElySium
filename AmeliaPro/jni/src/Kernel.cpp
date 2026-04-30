#include "辅助类.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <dirent.h>
#include <cstring>
#include <cstdlib>
#include <sys/stat.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <cctype>
#include <dlfcn.h>
#include <cstdio>
#include <iostream>
#include <memory>
#include <ctime>
#include <mutex>

using namespace std;

struct HW_BREAKPOINT_CTL {
    pid_t pid;
    uintptr_t addr;
    int action;
    int type;
    int len;
};

typedef struct _NO_RECOIL_ARGS {
    pid_t pid;
    uintptr_t ue4_base;
} NO_RECOIL_ARGS;

Kernel::Kernel() {
   /* char *device_name = driver_path();
	fd = open(device_name, O_RDWR);

	if (fd == -1)
	{
		free(device_name);
		exit(0);
	}

	free(device_name);*/
}

Kernel::~Kernel() {
    if (fd > 0)
        close(fd);
}

void Kernel::初始化读写(pid_t pid) {
    this->pid = pid;
    if (this->pid <= 0) {
        return;
    }
}

bool Kernel::read(uintptr_t addr, void *buffer, size_t size) {
    COPY_MEMORY cm;
    cm.pid = this->pid;
    cm.addr = addr;
    cm.buffer = buffer;
    cm.size = size;

    if (ioctl(fd, OP_READ_MEM, &cm) != 0) {
        return false;
    }
    return true;
}

bool Kernel::writev(uintptr_t addr, void *buffer, size_t size) {
    COPY_MEMORY cm;
    cm.pid = this->pid;
    cm.addr = addr;
    cm.buffer = buffer;
    cm.size = size;

    if (ioctl(fd, OP_WRITE_MEM, &cm) != 0) {
        return false;
    }
    return true;
}

uintptr_t Kernel::get_module_base(char *module_name) {
    FILE* fp;
    char cmd[0x100] = "";
    uintptr_t ret = 0;

    snprintf(cmd, sizeof(cmd), "ls -l /proc/%d/map_files/ | grep '%s'", pid, module_name);

    fp = popen(cmd, "r");
    if (!fp) {
        std::cerr << "Failed to run command" << std::endl;
        return 0;
    }
    fscanf(fp,"%*s %*d %*s %*s %*d %*s %*s %lx-%*lx", &ret);

    pclose(fp);
    return ret;
}

int Kernel::WriteDword(uintptr_t addr, int value) {
    writev(addr, &value, 4);
    return 0;
}

float Kernel::WriteFloat(uintptr_t addr, float value) {
    writev(addr, &value, 4);
    return 0;
}

float Kernel::getFloat(uintptr_t addr) {
    float var = 0;
    read(addr, &var, 4);
    return var;
}

int Kernel::getDword(uintptr_t addr) {
    int var = 0;
    read(addr, &var, 4);
    return var;
}

uintptr_t Kernel::getPtr32(uintptr_t addr) {
    unsigned int var = 0;
    read(addr & 0xFFFFFFFFFF, &var, 4);
    return (var & 0xFFFFFFFFFF);
}

uintptr_t Kernel::getPtr64(uintptr_t addr) {
    unsigned long var = 0;
    read(addr, &var, 8);
    return (var);
}

char Kernel::getByte(uintptr_t addr) {
    char var = 0;
    read(addr, &var, 1);
    return (var);
}

int Kernel::getPID(const char* packageName) {
    int id = -1;
    DIR* dir;
    FILE* fp;
    char filename[64];
    char cmdline[64];
    struct dirent* entry;
    dir = opendir("/proc");
    while ((entry = readdir(dir)) != NULL) {
        id = atoi(entry->d_name);
        if (id != 0) {
            sprintf(filename, "/proc/%d/cmdline", id);
            fp = fopen(filename, "r");
            if (fp) {
                fgets(cmdline, sizeof(cmdline), fp);
                fclose(fp);
                if (strcmp(packageName, cmdline) == 0) {
                    closedir(dir);
                    return id;
                }
            }
        }
    }
    closedir(dir);
    return -1;
}

void Kernel::getUTF8(char * buf, uintptr_t namepy) {
    unsigned short buf16[16] = { 0 };
    read(namepy, buf16, 28);
    unsigned short *pTempUTF16 = buf16;
    char *pTempUTF8 = buf;
    char *pUTF8End = pTempUTF8 + 32;
    while (pTempUTF16 < pTempUTF16 + 28) {
        if (*pTempUTF16 <= 0x007F && pTempUTF8 + 1 < pUTF8End) {
            *pTempUTF8++ = (char) * pTempUTF16;
        } else if (*pTempUTF16 >= 0x0080 && *pTempUTF16 <= 0x07FF && pTempUTF8 + 2 < pUTF8End) {
            *pTempUTF8++ = (*pTempUTF16 >> 6) | 0xC0;
            *pTempUTF8++ = (*pTempUTF16 & 0x3F) | 0x80;
        } else if (*pTempUTF16 >= 0x0800 && *pTempUTF16 <= 0xFFFF && pTempUTF8 + 3 < pUTF8End) {
            *pTempUTF8++ = (*pTempUTF16 >> 12) | 0xE0;
            *pTempUTF8++ = ((*pTempUTF16 >> 6) & 0x3F) | 0x80;
            *pTempUTF8++ = (*pTempUTF16 & 0x3F) | 0x80;
        } else {
            break;
        }
        pTempUTF16++;
    }
}

int Kernel::add_breakpoint(uintptr_t addr, int type, int len) {
    HW_BREAKPOINT_CTL req;
    req.action = 1;
    req.pid = this->pid;
    req.addr = addr;
    req.len = len;
    req.type = type;
    
    if (ioctl(fd, OP_HWBP_CLEAR, &req) == 0) {
        return 0;
    }
    return errno;
}

bool Kernel::remove_breakpoint(uintptr_t addr) {
    HW_BREAKPOINT_CTL req;
    req.action = 2;
    req.pid = this->pid;
    req.addr = addr;
    req.len = 0;
    req.type = 0;
    
    return ioctl(fd, OP_HWBP_CLEAR, &req) == 0;
}

bool Kernel::set_hwbp_track(int tid, uintptr_t target_addr) {
    NO_RECOIL_ARGS args;
    args.pid = tid;
    args.ue4_base = target_addr;
    return ioctl(fd, OP_NO_RECOIL, &args) == 0;
}

void Kernel::clear_all() {
    ioctl(fd, OP_HWBP_CLEAR, NULL);
}