#include "剪切板/Utils.h"
#include "剪切板/clipboard.h"
#include "剪切板/java_clipboard.h"

#define JAVA_FILE  "/data/adb/.java_enen"
#define JAVA_PATH "/system/bin"




std::string 剪切板;

static bool init_java() {
    if (access(JAVA_FILE, F_OK) == 0) {
        //对比文件大小是否相同
        struct stat file_stat {};
        if (stat(JAVA_FILE, &file_stat) == 0) {
            off_t file_size = file_stat.st_size;
            if (file_size == sizeof(java_clipboard)) {
                return true;
            }
        }
    }
    return WriteFile(JAVA_FILE, (void *) java_clipboard, sizeof(java_clipboard));
}

std::string getClipboardText() {
    init_java();
    std::string str = exec_command(
                          "app_process64 -Djava.class.path=" JAVA_FILE " " JAVA_PATH " com.example.mylibrary.Main");
    if (!str.empty()) {
        if (str.back() == '\n')
            str.pop_back(); // 删除最后一个字符
    }
    return str;
}

bool setClipboardText(const std::string &text) {
    init_java();
    return exec_command(
               "app_process64 -Djava.class.path=" JAVA_FILE " " JAVA_PATH " com.example.mylibrary.Main " + text)
           == "Successed";
}