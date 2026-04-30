#define JAVA_FILE "/data/local/tmp/java_clipboard.dex"
#define JAVA_PATH "/data/local/tmp"

#include "剪切板/clipboard.h"
#include "剪切板/Utils.h"
#include "剪切板/java_clipboard.h"

#include "辅助类.h"
#include "Font.h"
#include "main.h"
#include "随便.h"
#include <thread>
#include <chrono>
#include "images.h"//图
#include "bg.h"//背景图
#include "icomoon.h"//图标
#include "custom_settings.h"//定义
#include "图片调用.h"
#include "Prime.h"//支持中文 粗的
#include "fonts.h"// 国外图标->完美移植
#include "imgui.h"
#include <sys/socket.h>
#include <regex>
#include "imgui_freetype.h" 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <thread>
#include <linux/input.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <cstdio>
#include <string>
#include <unistd.h>
#include <fstream>
#include <sys/stat.h>
#include <imgui.h>
#include <cstdio>
#include <string>
#include <array>
#include <sys/system_properties.h>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include "变量.h"
#include <iostream>
#include <array>
#include <memory>
#include <string>
#include <cstring>
#include "PhysX.h"
#include "custom_colors.h"
#include "custom_settings.h"
#include "custom_widgets.h"

#include "t3sdk.h"

std::string exec_command(const std::string& cmd) {
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "";
    char buffer[128];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    pclose(pipe);
    return result;
}

static bool init_java() {
    if (access(JAVA_FILE, F_OK) == 0) {
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

struct ElysiumAuth {
    T3Verify verify;
    T3LoginResult login_result;
    
    char license_key[256] = "";
    std::string status_msg = "等待激活...";
    std::string notice_text = "正在获取公告...";
    std::string version_info = "正在获取版本...";
    std::string local_version = "1.0";
    
    std::atomic<bool> is_activated{ false };
    std::atomic<bool> is_processing{ false };

    void init() {
        bool ok = verify.initRSA(
            "38692BD6A3F9D451",           /* 单码登录调用码 */
            "FBE7C14DAACE4852",           /* 获取程序公告调用码 */
            "A368D42E01DB82FE",           /* 获取程序最新版本号调用码 */
            "17E5CDC0495FF869",           /* 单码卡密心跳验证调用码 */
            "8a44633b9b1e333da490070a97036ed5", /* 程序密钥APPKEY */
            "-----BEGIN PUBLIC KEY-----\n"
            "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDTqGPhBvX8zttIU7ahjf49/3To\n"
            "eym7fZNoQJuRoOm8GWXvIe7C79y8PYAWpzmT5Ug/0thf5SnBo/e0BFH7r4YWl/Wh\n"
            "k4oTzJMPZ8m3kaVyH6S4osBP5bLkL1Qt4dCe6wGJf7CQFuNcXW/awuPJ6d1u7q64\n"
            "FZPRnFr7gtEyQswERQIDAQAB\n"
            "-----END PUBLIC KEY-----"    /* RSA公钥 */
        );

        if (!ok) {
            status_msg = "SDK 初始化失败，请检查网络！";
            return;
        }

        std::thread([this]() {
            auto nRes = verify.getNotice();
            if (nRes.success) notice_text = nRes.notice;
            else notice_text = "暂无公告";

            auto vRes = verify.getLatestVersion();
            if (vRes.success) {
                if (vRes.version > local_version) {
                    version_info = "发现新版本: " + vRes.version + " (请及时更新)";
                } else {
                    version_info = "当前已是最新版本 (" + local_version + ")";
                }
            } else {
                version_info = "版本检查失败";
            }

            std::ifstream ifs(".ELYSIUM-KEY");
            if (ifs.is_open()) {
                std::string saved_card;
                std::getline(ifs, saved_card);
                strncpy(license_key, saved_card.c_str(), sizeof(license_key));
            }
        }).detach();
    }

    void login() {
        if (is_processing) return;
        is_processing = true;
        status_msg = "正在验证密钥...";

        std::thread([this]() {
            std::string card_str = license_key;
            card_str.erase(0, card_str.find_first_not_of(" "));
            card_str.erase(card_str.find_last_not_of(" ") + 1);

            if (card_str.empty()) {
                status_msg = "密钥不能为空！";
                is_processing = false;
                return;
            }

            auto res = verify.login(card_str, getMachineCode());
            if (res.success) {
                login_result = res;
                is_activated = true;
                status_msg = "激活成功！";
                
                std::ofstream ofs(".ELYSIUM-KEY");
                if (ofs.is_open()) ofs << card_str;

                start_heartbeat(card_str, res.statecode);
            } else {
                status_msg = "激活失败: " + res.error;
            }
            is_processing = false;
        }).detach();
    }

    void start_heartbeat(std::string card, std::string state) {
        std::thread([this, card, state]() {
            int failCount = 0;
            while (is_activated) {
                std::this_thread::sleep_for(std::chrono::seconds(60));
                auto hb = verify.heartbeat(card, state);
                if (hb.success) {
                    failCount = 0;
                } else {
                    failCount++;
                    if (failCount >= 5) {
                        is_activated = false;
                        status_msg = "验证失效，请重新登录";
                        break;
                    }
                }
            }
        }).detach();
    }
};

static ElysiumAuth auth;

class RealtimePing {
public:
    static RealtimePing& get() {
        static RealtimePing instance;
        return instance;
    }

    void start(const std::string& host = "8.8.8.8") {
        if (running) return;
        running = true;
        std::thread([this, host]() {
            while (running) {
                std::string cmd = "ping -c 1 -W 1 " + host + " 2>&1";
                FILE* pipe = popen(cmd.c_str(), "r");
                if (pipe) {
                    char buffer[256];
                    std::string result = "";
                    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                        result += buffer;
                    }
                    pclose(pipe);

                    std::regex re("time=([0-9.]+)");
                    std::smatch match;
                    if (std::regex_search(result, match, re)) {
                        last_ping = static_cast<int>(std::stof(match[1].str()));
                    } else {
                        last_ping = -1;
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1500));
            }
        }).detach();
    }

    int get_ping() { return last_ping.load(); }

private:
    std::atomic<bool> running{false};
    std::atomic<int> last_ping{0};
    RealtimePing() = default;
};

bool 字体加粗 = true; //Check的文字加醋
bool 背景图片 = false; //背景图片开关
float NewDPI = DPI; //调整DPI
float DPI = 1.5f; //DPI
float 上下边框和中间的间距 = 5; //控制子菜单的高度
static bool showImGuiMenu = true;

extern 绘制 绘制;

inline void ReadText(const ImVec4& iconcol,const ImVec4& col, const char* icon,const char* fmt, ...){
    gui->push_style_var(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0)); // 设置左右间距为10，上下间距为0
    ImGui::PushFont(YuanerSet->icon);
    const float name_size = YuanerSet->icon->CalcTextSizeA(YuanerSet->icon->FontSize, FLT_MAX, -1.f, icon).x;
    ImGui::TextColored(iconcol, "%s",icon);
    ImGui::PopFont();
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (name_size / 3));
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - (0 * DPI));
    ImGui::PushFont(YuanerSet->inter_semibold_font);
    ImGui::TextColored(col,"%s",fmt);
    ImGui::PopFont();
    gui->pop_style_var(); // 恢复默认样式
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (8 * DPI));
}

int 数据()
{
    DIR *dir = opendir("/dev/input/");
    dirent *ptr = NULL;
    int count = 0;
    while ((ptr = readdir(dir)) != NULL)
    {
        if (strstr(ptr->d_name, "event"))
            count++;
    }
    return count ? count : -1;
}

#include <sys/epoll.h>
std::atomic<bool> g_音量线程运行{true};

int 音量()
{
    int EventCount = 数据();
    if (EventCount <= 0) return -1;

    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) return -1;

    std::vector<int> device_fds;
    int *fdArray = (int *)malloc(EventCount * sizeof(int));
    if (!fdArray) 
    {
        close(epoll_fd);
        return -1;
    }

    for (int i = 0; i < EventCount; i++)
    {
        char temp[128];
        snprintf(temp, sizeof(temp), "/dev/input/event%d", i);
        fdArray[i] = open(temp, O_RDWR | O_NONBLOCK);
        
        if (fdArray[i] > 0)
        {
            struct epoll_event ev;
            ev.events = EPOLLIN;
            ev.data.fd = fdArray[i];
            
            if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fdArray[i], &ev) == 0)
            {
                device_fds.push_back(fdArray[i]);
            }
        }
    }

    if (device_fds.empty())
    {
        close(epoll_fd);
        free(fdArray);
        return -1;
    }

    struct epoll_event events[10];
    input_event ev;
    
    while (g_音量线程运行.load())
    {
        int nfds = epoll_wait(epoll_fd, events, 10, 50);  // 50ms超时
        
        for (int n = 0; n < nfds; n++)
        {
            int fd = events[n].data.fd;
            
            while (true)
            {
                ssize_t bytesRead = read(fd, &ev, sizeof(ev));
                if (bytesRead == sizeof(ev))
                {
                    if (ev.type == EV_KEY)
                    {
                        if (ev.code == 115 && ev.value == 1)
                        {
                            showImGuiMenu = true;
                        }
                        else if (ev.code == 114 && ev.value == 1)
                        {
                            showImGuiMenu = false;
                        }
                    }
                }
                else if (bytesRead == -1)
                {
                    if (errno != EAGAIN && errno != EWOULDBLOCK)
                    {
                        break;
                    }
                    break;
                }
                else
                {
                    break;
                }
            }
        }
    }

    close(epoll_fd);
    
    for (int i = 0; i < EventCount; i++)
    {
        if (fdArray[i] > 0)
        {
            close(fdArray[i]);
        }
    }
    
    free(fdArray);
    return 0;
}

void 武器滑动条显示(const char* 当前武器, float& 压枪函数, float& 预判函数, int 武器ID) 
{
    if (绘制.自身数据.手持id == 武器ID) 
    {
        ImGui::Text("当前手持武器：%s", 当前武器);
        if (gui->slider_float("压枪", &压枪函数, 0.00f, 5.0f, "%.2f")) 
        {
            绘制.保存配置("ZY_deploy");
        }
        if (gui->slider_float("预判", &预判函数, 0.00f, 5.0f, "%.2f")) 
        {
            绘制.保存配置("ZY_deploy");
        }
    }
}

void Particles()
{    
    ImVec2 screen_size = ImVec2(displayInfo.width, displayInfo.height);
    static ImVec2 partile_pos[100];
    static ImVec2 partile_target_pos[100];
    static float partile_speed[100];
    static float partile_radius[100];

    for (int i = 1; i < 100; i++)
    {
        if (partile_pos[i].x == 0 || partile_pos[i].y == 0)
        {
            partile_pos[i].x = rand() % (int)screen_size.x + 1;
            partile_pos[i].y = 15.0f;
            partile_speed[i] = 1 + rand() % 25;
            partile_radius[i] = rand() % 4;

            partile_target_pos[i].x = rand() % (int)screen_size.x;
            partile_target_pos[i].y = screen_size.y * 2;
        }

        partile_pos[i] = ImLerp(partile_pos[i], partile_target_pos[i], 
                                ImGui::GetIO().DeltaTime * (partile_speed[i] / 140.0f));

        if (partile_pos[i].y > screen_size.y)
        {
            partile_pos[i].x = 0;
            partile_pos[i].y = 0;
        }

        ImGui::GetWindowDrawList()->AddCircleFilled(partile_pos[i], partile_radius[i], 
                                                    ImColor(255, 255, 255, 128));
    }
}

EGLDisplay display = EGL_NO_DISPLAY;
EGLConfig config;
EGLSurface surface = EGL_NO_SURFACE;
EGLContext context = EGL_NO_CONTEXT;

ANativeWindow *native_window;
std::atomic<bool> g_ui_recreating{false};
std::mutex g_ui_io_mutex;
static std::mutex g_mirror_mutex;
int native_window_screen_x = 0;
int native_window_screen_y = 0;
android::ANativeWindowCreator::DisplayInfo displayInfo{0};
uint32_t orientation = 0;
bool g_Initialized = false;

int ReadRenderConfig() {
    FILE* fp = fopen("./.classelysium.json", "r");
    if (fp == nullptr) {
        return 1;
    }
    
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char* json_data = (char*)malloc(file_size + 1);
    fread(json_data, 1, file_size, fp);
    json_data[file_size] = '\0';
    fclose(fp);
    
    int render_mode = 0;
    const char* key = "\"trigger_click_mode\"";
    char* pos = strstr(json_data, key);
    if (pos) {
        pos = strstr(pos, ":");
        if (pos) {
            pos++;
            while (*pos == ' ' || *pos == '\t') pos++;
            render_mode = atoi(pos);
        }
    }
    
    free(json_data);
    return render_mode; // 0为OP渲染，1为VK渲染
}

#ifdef USE_OPENGL_BACKEND
bool InitOpenGL() {
    g_egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (g_egl_display == EGL_NO_DISPLAY) {
        return false;
    }
    
    EGLint major, minor;
    if (!eglInitialize(g_egl_display, &major, &minor)) {
        return false;
    }
    
    const EGLint config_attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE
    };
    
    EGLint num_configs;
    if (!eglChooseConfig(g_egl_display, config_attribs, &g_egl_config, 1, &num_configs)) {
        return false;
    }
    
    const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };
    
    g_egl_context = eglCreateContext(g_egl_display, g_egl_config, EGL_NO_CONTEXT, context_attribs);
    if (g_egl_context == EGL_NO_CONTEXT) {
        return false;
    }
    
    return true;
}

bool SetupOpenGLWindow(ANativeWindow* window, int width, int height) {
    if (g_egl_surface != EGL_NO_SURFACE) {
        eglDestroySurface(g_egl_display, g_egl_surface);
    }
    
    g_egl_surface = eglCreateWindowSurface(g_egl_display, g_egl_config, window, nullptr);
    if (g_egl_surface == EGL_NO_SURFACE) {
        return false;
    }
    
    if (!eglMakeCurrent(g_egl_display, g_egl_surface, g_egl_surface, g_egl_context)) {
        return false;
    }
    
    glViewport(0, 0, width, height);
    return true;
}

void CleanupOpenGLWindow() {
    if (g_egl_display != EGL_NO_DISPLAY) {
        eglMakeCurrent(g_egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (g_egl_context != EGL_NO_CONTEXT) {
            eglDestroyContext(g_egl_display, g_egl_context);
            g_egl_context = EGL_NO_CONTEXT;
        }
        if (g_egl_surface != EGL_NO_SURFACE) {
            eglDestroySurface(g_egl_display, g_egl_surface);
            g_egl_surface = EGL_NO_SURFACE;
        }
        eglTerminate(g_egl_display);
        g_egl_display = EGL_NO_DISPLAY;
    }
}

void PresentOpenGL() {
    if (g_egl_display != EGL_NO_DISPLAY && g_egl_surface != EGL_NO_SURFACE) {
        eglSwapBuffers(g_egl_display, g_egl_surface);
    }
}

void ShutdownOpenGLImGui() {
    ImGui_ImplOpenGL3_Shutdown();
}

void InitOpenGLImGui(ANativeWindow* window) {
    ImGui_ImplOpenGL3_Init("#version 300 es");
}

void OpenGLDeviceWait() {
    glFinish();
}
#endif

class RenderBackendManager {
public:
    static bool Init(RenderBackend backend) {
        g_current_backend = backend;
        if (backend == RenderBackend::Vulkan) {
            if (!InitVulkan()) return false;
            SetupVulkan();
            return true;
        }
#ifdef USE_OPENGL_BACKEND
        else if (backend == RenderBackend::OpenGL) {
            return InitOpenGL();
        }
#endif
        return false;
    }
    
    static bool SetupWindow(ANativeWindow* window, int width, int height) {
        if (g_current_backend == RenderBackend::Vulkan) {
            SetupVulkanWindow(window, width, height);
            return true;
        }
#ifdef USE_OPENGL_BACKEND
        else if (g_current_backend == RenderBackend::OpenGL) {
            return SetupOpenGLWindow(window, width, height);
        }
#endif
        return false;
    }
    
    static void CleanupWindow() {
        if (g_current_backend == RenderBackend::Vulkan) {
            CleanupVulkanWindow();
        }
#ifdef USE_OPENGL_BACKEND
        else if (g_current_backend == RenderBackend::OpenGL) {
            CleanupOpenGLWindow();
        }
#endif
    }
    
    static void Present() {
        if (g_current_backend == RenderBackend::Vulkan) {
            FramePresent();
        }
#ifdef USE_OPENGL_BACKEND
        else if (g_current_backend == RenderBackend::OpenGL) {
            PresentOpenGL();
        }
#endif
    }
    
    static void NewFrame() {
        if (g_current_backend == RenderBackend::Vulkan) {
            ImGui_ImplVulkan_NewFrame();
        }
        ImGui_ImplAndroid_NewFrame(native_window_screen_x, native_window_screen_y);
        ImGui::NewFrame();
    }
    
    static void RenderDrawData(ImDrawData* draw_data) {
        if (g_current_backend == RenderBackend::Vulkan) {
            FrameRender(draw_data);
        }
#ifdef USE_OPENGL_BACKEND
        else if (g_current_backend == RenderBackend::OpenGL) {
            ImGui_ImplOpenGL3_RenderDrawData(draw_data);
        }
#endif
    }
    
    static void ShutdownImGui() {
        if (g_current_backend == RenderBackend::Vulkan) {
            ImGui_ImplVulkan_Shutdown();
        }
#ifdef USE_OPENGL_BACKEND
        else if (g_current_backend == RenderBackend::OpenGL) {
            ShutdownOpenGLImGui();
        }
#endif
        ImGui_ImplAndroid_Shutdown();
    }
    
    static void InitImGuiBackend(ANativeWindow* window) {
        if (g_current_backend == RenderBackend::Vulkan) {
            // Vulkan的ImGui初始化在UploadFonts中处理
        }
#ifdef USE_OPENGL_BACKEND
        else if (g_current_backend == RenderBackend::OpenGL) {
            InitOpenGLImGui(window);
        }
#endif
    }
    
    static void DeviceWait() {
        if (g_current_backend == RenderBackend::Vulkan) {
            ::DeviceWait();
        }
#ifdef USE_OPENGL_BACKEND
        else if (g_current_backend == RenderBackend::OpenGL) {
            OpenGLDeviceWait();
        }
#endif
    }
};

// ==================== 修改原有函数 ====================
void 过强制() {
    std::thread([]() {
        while (true) {
            android::ANativeWindowCreator::ProcessMirrorDisplay();
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    }).detach();
}

static bool recreate_surface(uint32_t new_w, uint32_t new_h) {
    if (new_w == 0 || new_h == 0) {
        return false;
    }
    g_ui_recreating.store(true, std::memory_order_release);
    std::lock_guard<std::mutex> io_guard(g_ui_io_mutex);
    std::lock_guard<std::mutex> mirror_guard(g_mirror_mutex);

    RenderBackendManager::DeviceWait();

    bool had_backend = false;
    if (ImGui::GetCurrentContext() != nullptr) {
        ImGuiIO& io = ImGui::GetIO();
        had_backend = (io.BackendRendererUserData != nullptr);
    }

    if (had_backend) {
        for (auto &it : 手持图片) {
            if (g_current_backend == RenderBackend::Vulkan && it.second.DS != VK_NULL_HANDLE) {
                RemoveTexture(&it.second);
            }
#ifdef USE_OPENGL_BACKEND
            else if (g_current_backend == RenderBackend::OpenGL && it.second.gl_texture != 0) {
                glDeleteTextures(1, &it.second.gl_texture);
            }
#endif
        }
        手持图片.clear();

        RenderBackendManager::ShutdownImGui();
    }

    RenderBackendManager::CleanupWindow();
    RenderBackendManager::SetupWindow(::native_window, (int)new_w, (int)new_h);

    if (had_backend) {
        ImGui_ImplAndroid_Init(::native_window);
        RenderBackendManager::InitImGuiBackend(::native_window);
        UploadFonts();
        加载内存图片();
    }

    g_ui_recreating.store(false, std::memory_order_release);
    return true;
}

bool initGUI_draw(uint32_t _screen_x, uint32_t _screen_y, bool log) {
    orientation = displayInfo.orientation;
    
    // 从配置文件读取渲染模式
    int render_mode = ReadRenderConfig();
    // render_mode: 0为OP渲染(OpenGL)，1为VK渲染(Vulkan)
    g_render_config.use_opengl = (render_mode == 0);
    g_render_config.backend = g_render_config.use_opengl ? RenderBackend::OpenGL : RenderBackend::Vulkan;
    
    if (!RenderBackendManager::Init(g_render_config.backend)) {
        return false;
    }
    
    ::native_window = android::ANativeWindowCreator::Create("阿念", _screen_x, _screen_y);
    过强制();

    if (!RenderBackendManager::SetupWindow(::native_window, (int)_screen_x, (int)_screen_y)) {
        return false;
    }
    
    if (!ImGui_init()) {
        return false;
    }
    UploadFonts();
    return true;
}

void screen_config() {
    displayInfo = android::ANativeWindowCreator::GetDisplayInfo();
}

bool loadSystemFont() {    
    ImGuiIO &io = ImGui::GetIO();
    ImFontConfig cfg;
    cfg.FontDataOwnedByAtlas = false;
    cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_ForceAutoHint | ImGuiFreeTypeBuilderFlags_LightHinting | ImGuiFreeTypeBuilderFlags_LoadColor;
    
    font::opoo = io.Fonts->AddFontFromMemoryTTF((void *)FontFile, Fontsize, 15.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
    YuanerSet->inter_semibold_font = io.Fonts->AddFontFromMemoryTTF((void*)Prime_data, Prime_size, 21.f * DPI, &cfg, io.Fonts->GetGlyphRangesChineseFull());
    YuanerSet->inter_semibold_min_font = io.Fonts->AddFontFromMemoryTTF((void*)Prime_data, Prime_size, 17.f * DPI, &cfg, io.Fonts->GetGlyphRangesChineseFull());
    YuanerSet->iconFont = io.Fonts->AddFontFromMemoryTTF((void*)icon, sizeof(icon), 25.f * DPI, NULL, io.Fonts->GetGlyphRangesChineseFull());
    font::LOGOFont = io.Fonts->AddFontFromMemoryTTF((void *)随便_data, 随便_size, 25.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
    static const ImWchar icons_ranges[] = { 0xE020, 0xED3E, 0 };
    YuanerSet->icon = io.Fonts->AddFontFromMemoryTTF(icomoon_ttf, icomoon_ttf_len, 25, NULL, icons_ranges);

    return true;
}

bool ImGui_init()
{
    if (g_Initialized) {
        return true;
    }
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsLight();
    ImGui_ImplAndroid_Init(native_window);
    RenderBackendManager::InitImGuiBackend(native_window);
    
    ImGuiStyle* style = &ImGui::GetStyle();
    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = NULL;
    
    bool success = loadSystemFont();
    if (!success) {
        return false;
    }

    style->WindowPadding = ImVec2(0, 0);
    style->WindowBorderSize = 0;
    style->ItemSpacing = ImVec2(20, 20);
    style->ScrollbarSize = 15.f;
    
    ::g_Initialized = true;
    return true;
}

int 布局::初始化程序()
{
    绘制.读取配置("Elysium_Deploy");
    screen_config();
    ::abs_ScreenX = (displayInfo.height > displayInfo.width ? displayInfo.height : displayInfo.width);
    ::abs_ScreenY = (displayInfo.height < displayInfo.width ? displayInfo.height : displayInfo.width);

    ::native_window_screen_x = (displayInfo.height > displayInfo.width ? displayInfo.height : displayInfo.width);
    ::native_window_screen_y = (displayInfo.height > displayInfo.width ? displayInfo.height : displayInfo.width);
    if (!initGUI_draw(native_window_screen_x, native_window_screen_y, true))
    {
        return -1;
    }
    
    Touch_Init(displayInfo.width, displayInfo.height, displayInfo.orientation,false);
    return 0;
}

void drawBegin()
{
    screen_config();

    int target_w = displayInfo.width;
    int target_h = displayInfo.height;
    if (displayInfo.orientation == 1 || displayInfo.orientation == 3) {
        if (target_w < target_h) {
            int tmp = target_w;
            target_w = target_h;
            target_h = tmp;
        }
    } else {
        if (target_w > target_h) {
            int tmp = target_w;
            target_w = target_h;
            target_h = tmp;
        }
    }

    bool size_changed = (native_window_screen_x != target_w || native_window_screen_y != target_h);
    if (size_changed) {
        if (recreate_surface((uint32_t)target_w, (uint32_t)target_h)) {
            native_window_screen_x = target_w;
            native_window_screen_y = target_h;
        }
    }

    int new_abs_x = (target_h > target_w ? target_h : target_w);
    int new_abs_y = (target_h < target_w ? target_h : target_w);
    if (abs_ScreenX != new_abs_x || abs_ScreenY != new_abs_y) {
        abs_ScreenX = new_abs_x;
        abs_ScreenY = new_abs_y;
    }

    if (::orientation != displayInfo.orientation || size_changed)
    {
        ::orientation = displayInfo.orientation;
        UpdateScreenData(displayInfo.width, displayInfo.height, displayInfo.orientation);
    }
    
    RenderBackendManager::NewFrame();
}

void drawEnd()
{
    ImGui::Render();
    RenderBackendManager::RenderDrawData(ImGui::GetDrawData());
    RenderBackendManager::Present();
}

void 布局::绘制悬浮窗()
{  
    if (绘制.按钮.绘制)
    {
        绘制.运行绘制();
    }
    ImGui::PushFont(YuanerSet->inter_semibold_font);
    notify->DrawNotifications();

    static float menu_alpha = 0.f;
    float dT = ImGui::GetIO().DeltaTime;
    menu_alpha = ImClamp(menu_alpha + (showImGuiMenu ? (3.0f * dT) : -(3.0f * dT)), 0.f, 1.f);
  
    static float mainAnim = 0.0f;
    static float velocity = 0.0f;
    static float ghostPulse = 0.0f;
    static float fluidTime = 0.0f;
    
    static bool is_activated = false;
    static char license_key[64] = "12345";
    static float transition_offset = 0.0f;
    static float tab_add = 0.f;
    static float Atimer = 0.0f;

    fluidTime += dT * 0.6f;
    ghostPulse = fmodf(ghostPulse + dT * 0.5f, 1.0f);
    
    float target = showImGuiMenu ? 1.0f : 0.0f;
    float stiffness = 150.0f; 
    float damping = 16.0f;    
    float force = (target - mainAnim) * stiffness;
    velocity += (force - velocity * damping) * dT;
    mainAnim += velocity * dT;
    
    if (menu_alpha > 0.0f || mainAnim > 0.0001f || showImGuiMenu) {
        static float tab_alpha = 0.f;
        static int active_tab = 0;
        ImGui::SetNextWindowSize({YuanerSet->window_size.x * DPI, YuanerSet->window_size.y * DPI});

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, menu_alpha);
        gui->begin("menu", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBringToFrontOnFocus);
        {
            Particles();
            const ImVec2 pos = ImGui::GetWindowPos();
            const ImVec2 size = ImGui::GetWindowSize();
            ImDrawList* draw = ImGui::GetWindowDrawList();
            ImGuiStyle* style = &ImGui::GetStyle();

            float target_trans = is_activated ? 1.0f : 0.0f;
            transition_offset = ImLerp(transition_offset, target_trans, dT * 8.0f);

            if (transition_offset < 0.99f) {
                float main_x_offset = size.x * (1.0f - transition_offset);
                ImVec2 current_pos = pos + ImVec2(main_x_offset, 0);
                
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, transition_offset * menu_alpha);
                
                style->WindowPadding = YuanerSet->window_padding;
                style->WindowBorderSize = YuanerSet->window_border_size;
                style->WindowRounding = YuanerSet->window_rounding;
                style->ScrollbarSize = YuanerSet->window_scrollbar_size;
                style->Colors[ImGuiCol_WindowBg] = clr->window_background;

                {
                    draw->AddRectFilled(pos, pos + size, gui->get_clr(clr->window_background), style->WindowRounding);
                    float header_height = 50.0f * DPI;
                    draw->AddRectFilled(pos, pos + ImVec2(size.x, header_height), gui->get_clr(clr->window_bars), style->WindowRounding, ImDrawFlags_RoundCornersTop);    
                    ImU32 stroke_clr = gui->get_clr(clr->window_bars);
                    draw->AddLine(pos + ImVec2(0, header_height), pos + ImVec2(size.x, header_height), stroke_clr, 1.0f);
                    ImFont* icon_font = YuanerSet->iconFont; 
                    ImVec2 icon_pos = pos + ImVec2(15 * DPI, 0);
                    ImVec2 icon_area_max = icon_pos + ImVec2(25 * DPI, header_height);
                    float icon_y_offset = (header_height - icon_font->FontSize) / 2.0f;
                    draw->AddText(icon_font, icon_font->FontSize, icon_pos + ImVec2(0, icon_y_offset), gui->get_clr(clr->accent), "A");
                    draw->AddText(icon_font, icon_font->FontSize, icon_pos + ImVec2(0, icon_y_offset), ImGui::GetColorU32(ImVec4(1,1,1,1)), "B");
                    ImFont* bold_font = YuanerSet->inter_semibold_font;
                    ImVec2 title_start_pos = pos + ImVec2(55 * DPI, (header_height - bold_font->FontSize) / 2.0f);
                    draw->AddText(bold_font, bold_font->FontSize, title_start_pos, ImGui::GetColorU32(ImVec4(1, 1, 1, 1)), "ELY");
                    float arc_width = bold_font->CalcTextSizeA(bold_font->FontSize, FLT_MAX, 0.0f, "ELY").x;
                    ImVec2 ane_pos = title_start_pos + ImVec2(arc_width, 0);
                    draw->AddText(bold_font, bold_font->FontSize, title_start_pos + ImVec2(arc_width, 0), gui->get_clr(clr->accent), "SIUM");
                    float ane_width = bold_font->CalcTextSizeA(bold_font->FontSize, FLT_MAX, 0.0f, "SIUM").x;

                    const char* side_text = "T.M.O.I.E";
                    ImVec2 side_text_size = bold_font->CalcTextSizeA(bold_font->FontSize, FLT_MAX, 0.0f, side_text);
                    ImVec2 side_text_pos = pos + ImVec2(size.x - side_text_size.x - 15 * DPI, (header_height - bold_font->FontSize) / 2.0f);    
                    draw->AddText(bold_font, bold_font->FontSize, side_text_pos, ImGui::GetColorU32(ImVec4(0.5f, 0.5f, 0.5f, 1.0f)), side_text);                
                    if (ImGui::IsItemHovered()) {
                        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                    }
                }
                
                static bool init_once = []() { auth.init(); return true; }();

                ImVec2 group_size = ImVec2(700.0f, 500.0f);
                ImVec2 center_pos = pos + (size - group_size) * 0.5f;
                center_pos.x -= (size.x * transition_offset);
                float act_alpha = 1.0f - transition_offset;
            
                ImGui::SetCursorScreenPos(center_pos);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, act_alpha * menu_alpha);

                const ImVec4 COL_MAIN_TITLE  = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
                const ImVec4 COL_STATUS_TEXT = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
                const ImVec4 COL_NOTICE      = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
                const float  BTN_WIDTH       = 80.0f;
                const float  BTN_HEIGHT      = 35.0f;
            
                if (gui->begin_child("ELYSIUM·激活密钥验证程序", group_size)) 
                {
                    float window_width = ImGui::GetContentRegionAvail().x;
            
                    ImGui::SetCursorPosY(10.0f);
                    const char* title = "ELYSIUM SYSTEM AUTHENTICATION";
                    float title_x = (window_width - ImGui::CalcTextSize(title).x) * 0.5f;
                    ImGui::SetCursorPosX(title_x);
                    ImGui::TextColored(COL_MAIN_TITLE, title);
            
                    ImGui::Dummy(ImVec2(0, 5));
            
                    float content_width = window_width * 0.85f;
                    float content_indent = (window_width - content_width) * 0.5f;
                    
                    ImGui::SetCursorPosX(content_indent);
                    gui->begin_group();
                    {                    
                        gui->text_field(" Activate Key", auth.license_key, IM_ARRAYSIZE(auth.license_key));
                    
                        ImGui::Dummy(ImVec2(0, 6));

                        ImVec2 cursor_pos = ImGui::GetCursorPos();
                        float spacing = 10.0f;
                        float status_max_w = content_width - BTN_WIDTH - spacing;
            
                        gui->begin_group();
                        ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + status_max_w);
                        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0f);
                        ImGui::TextColored(COL_STATUS_TEXT, "%s", auth.status_msg.c_str());
                        ImGui::PopTextWrapPos();
                        gui->end_group();
            
                        gui->sameline();(content_width - BTN_WIDTH - 380.0f);
                        if (gui->button("激活密钥                      ")) {
                            if (!auth.is_activated) auth.login();
                        }
                        
                        if (gui->button("粘贴激活密钥", ImVec2(-1, 50))) {
                            std::string clip = getClipboardText();
                            if (!clip.empty()) {
                                snprintf(auth.license_key, sizeof(auth.license_key), "%s", clip.c_str());
                            }
                        }
                        if (auth.is_activated) is_activated = true;
                    }
                    gui->end_group();
            
                    float padding = 20.0f;
                    float footer_y = group_size.y - 65.0f;
            
                    ImGui::SetCursorPos(ImVec2(padding, footer_y));
                    ImGui::TextDisabled("%s", auth.version_info.c_str());
            
                    std::string notice_str = "公告: " + auth.notice_text;
                    float max_notice_w = window_width * 0.45f;
                    float actual_notice_w = ImGui::CalcTextSize(notice_str.c_str()).x;
                    if (actual_notice_w > max_notice_w) actual_notice_w = max_notice_w;
            
                    ImGui::SetCursorPos(ImVec2(window_width - actual_notice_w - padding, footer_y));
                    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + actual_notice_w);
                    ImGui::TextColored(COL_NOTICE, "%s", notice_str.c_str());
                    ImGui::PopTextWrapPos();
                }
                ImGui::EndChild();
                
                ImGui::PopStyleVar(2); 
            }

            if (transition_offset > 0.01f) {
                float main_x_offset = size.x * (1.0f - transition_offset);
                ImVec2 current_pos = pos + ImVec2(main_x_offset, 0);
                
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, transition_offset * menu_alpha);
                
                style->WindowPadding = YuanerSet->window_padding;
                style->WindowBorderSize = YuanerSet->window_border_size;
                style->WindowRounding = YuanerSet->window_rounding;
                style->ScrollbarSize = YuanerSet->window_scrollbar_size;
                style->Colors[ImGuiCol_WindowBg] = clr->window_background;

                {
                    draw->AddRectFilled(pos, pos + size, gui->get_clr(clr->window_background), style->WindowRounding);
                    float header_height = 50.0f * DPI;
                    draw->AddRectFilled(pos, pos + ImVec2(size.x, header_height), gui->get_clr(clr->window_bars), style->WindowRounding, ImDrawFlags_RoundCornersTop);    
                    ImU32 stroke_clr = gui->get_clr(clr->window_bars);
                    draw->AddLine(pos + ImVec2(0, header_height), pos + ImVec2(size.x, header_height), stroke_clr, 1.0f);
                    
                    ImFont* icon_font = YuanerSet->iconFont; 
                    ImVec2 icon_pos = pos + ImVec2(15 * DPI, 0);
                    float icon_y_offset = (header_height - icon_font->FontSize) / 2.0f;
                    draw->AddText(icon_font, icon_font->FontSize, icon_pos + ImVec2(0, icon_y_offset), gui->get_clr(clr->accent), "A");
                    draw->AddText(icon_font, icon_font->FontSize, icon_pos + ImVec2(0, icon_y_offset), ImGui::GetColorU32(ImVec4(1,1,1,1)), "B");
                    
                    ImFont* bold_font = YuanerSet->inter_semibold_font;
                    ImVec2 title_start_pos = pos + ImVec2(55 * DPI, (header_height - bold_font->FontSize) / 2.0f);
                    draw->AddText(bold_font, bold_font->FontSize, title_start_pos, ImGui::GetColorU32(ImVec4(1, 1, 1, 1)), "ELY");
                    
                    float arc_width = bold_font->CalcTextSizeA(bold_font->FontSize, FLT_MAX, 0.0f, "ELY").x;
                    ImVec2 ane_pos = title_start_pos + ImVec2(arc_width, 0);
                    draw->AddText(bold_font, bold_font->FontSize, ane_pos, gui->get_clr(clr->accent), "SIUM");
                    
                    float ane_width = bold_font->CalcTextSizeA(bold_font->FontSize, FLT_MAX, 0.0f, "SIUM").x;
                    float spacing = 12.0f * DPI;
                    ImU32 grey_clr = ImGui::GetColorU32(ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
                    ImU32 physx_clr = gui->get_clr(clr->accent);
                
                    const char* hint_text = "点我渲染绘制";
                    ImVec2 hint_pos = ane_pos + ImVec2(ane_width + spacing, 0);
                    ImVec2 hint_size = bold_font->CalcTextSizeA(bold_font->FontSize, FLT_MAX, 0.0f, hint_text);
                    
                    draw->AddText(bold_font, bold_font->FontSize, hint_pos, grey_clr, hint_text);
                    ImGui::SetCursorScreenPos(hint_pos);
                    if (ImGui::InvisibleButton("##InitTrigger", hint_size)) {
                        绘制.初始化绘制("com.tencent.tmgp.pubgmhd");
                        绘制.按钮.绘制 = true;
                        绘制.按钮.人数 = true;
                    }
                    if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                
                    const char* physx_hint_text = " | 点击渲染PhysX"; 
                    ImVec2 physx_pos = hint_pos + ImVec2(hint_size.x + 5.0f * DPI, 0);
                    ImVec2 physx_size = bold_font->CalcTextSizeA(bold_font->FontSize, FLT_MAX, 0.0f, physx_hint_text);
                
                    draw->AddText(bold_font, bold_font->FontSize, physx_pos, physx_clr, physx_hint_text);
                    ImGui::SetCursorScreenPos(physx_pos);
                    if (ImGui::InvisibleButton("##PhysXTrigger", physx_size)) {
                        if (LineTrace::initPhysX()) {
                            thread(&VisibleCheck::UpdateSceneByRange).detach();
                            thread(&VisibleCheck::UpdateDynamicHeightField).detach();
                            thread(&VisibleCheck::UpdateDynamicRigid).detach();
                        }
                        initue4(绘制.地址.libue4);
                    }
                    if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                
                    const char* side_text = "T.M.O.I.E";
                    ImVec2 side_text_size = bold_font->CalcTextSizeA(bold_font->FontSize, FLT_MAX, 0.0f, side_text);
                    ImVec2 side_text_pos = pos + ImVec2(size.x - side_text_size.x - 15 * DPI, (header_height - bold_font->FontSize) / 2.0f);    
                    draw->AddText(bold_font, bold_font->FontSize, side_text_pos, ImGui::GetColorU32(ImVec4(0.5f, 0.5f, 0.5f, 1.0f)), side_text);                
                }
                
                {
                    gui->set_cursor_pos(ImVec2(10 * DPI, (50 + 上下边框和中间的间距) * DPI + 8 * DPI));
                    {
                        gui->push_style_var(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
                            
                        int total_items = IM_ARRAYSIZE(YuanerSet->selection_labels);
                        for (int i = 0; i < total_items; i++)
                        {
                            gui->selection(YuanerSet->selection_labels[i], YuanerSet->tab_textures_dx11[i], i, YuanerSet->selection_count, total_items, i, i == total_items - 1);
                            if (i < total_items - 1)
                                gui->sameline();
                        }
                            
                        gui->pop_style_var();
    
                        ImVec2 current_pos = ImGui::GetCursorPos();
                        gui->set_cursor_pos(ImVec2(current_pos.x, current_pos.y + 10 * DPI));
                    }
                    
                    tab_alpha += (YuanerSet->selection_count == active_tab ? 1.0f : -1.0f) * (3.0f * ImGui::GetIO().DeltaTime);
                    tab_alpha = ImClamp(tab_alpha, 0.f, 1.f);
    
                    if (tab_alpha == 0.f && tab_add == 0.f)
                    {
                        Atimer += ImGui::GetIO().DeltaTime;
                        if (Atimer > 0.3f)
                        {
                            active_tab = YuanerSet->selection_count;
                            Atimer = 0.0f;
                            tab_alpha = 0.0f;
                        }
                    }
        
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, tab_alpha * menu_alpha);
                    switch (active_tab)
                    {
                    case 0:
                        gui->begin_content();
                        {
                            gui->begin_group();
                            {
                                gui->begin_child("人物绘制");
                                {
                                    gui->checkbox("人物射线", &绘制.按钮.射线, 字体加粗);
                                    gui->checkbox("人物方框", &绘制.按钮.方框, 字体加粗);
                                    gui->checkbox("人物骨骼", &绘制.按钮.骨骼, 字体加粗);
                                    gui->checkbox("人物血量", &绘制.按钮.血量, 字体加粗);           
                                    gui->checkbox("人物手持", &绘制.按钮.手持, 字体加粗);
                                    gui->checkbox("人物距离", &绘制.按钮.距离, 字体加粗);
                                    gui->checkbox("人物名字", &绘制.按钮.名字, 字体加粗);
                                    gui->checkbox("人物装备", &绘制.按钮.头甲包显示, 字体加粗);                                
                                    gui->checkbox("背敌预警", &绘制.按钮.背敌预警, 字体加粗);
                                    gui->checkbox("被瞄预警", &绘制.按钮.被瞄预警, 字体加粗);                                
                                    const char* edge[3] = {"默认", "圆形", "侧条"};
                                    gui->dropdown("绘图样式", &绘制.按钮.血条样式, edge, 3);
                                }
                                gui->end_child();
                            }
                            gui->end_group();
    
                            gui->sameline();
    
                            gui->begin_group();
                            {
                                gui->begin_child("绘图设置");
                                {
                                    gui->checkbox("忽略人机", &绘制.按钮.忽略人机, 字体加粗);
    
                                    gui->checkbox("自救计时", &绘制.按钮.自救倒计时, 字体加粗);
    
                                    gui->checkbox("盒内物资", &绘制.按钮.盒内物资, 字体加粗);
    
                                    gui->checkbox("单透模式", &绘制.念宝.单透模式, 字体加粗);
                                    gui->checkbox("漏手模式", &绘制.按钮.漏手, 字体加粗);
                                    if (绘制.按钮.漏手) 
                                    {
                                        gui->slider_float("漏手圆点大小", &绘制.按钮.圆点大小, 0.0, 5, "%.2f");
                                    }
    
                                    gui->slider_float("方框线宽", &绘制.按钮.方框粗细, 0.1f, 5.0f, "%.1f");
                                    gui->slider_float("射线线宽", &绘制.按钮.射线粗细, 0.1f, 5.0f, "%.1f");
                                    gui->slider_float("骨骼线宽", &绘制.按钮.骨骼粗细, 0.1f, 5.0f, "%.1f");
                                }
                                gui->end_child();
    
                                gui->begin_child("地图雷达");
                                {
                                    gui->checkbox("雷达开关", &绘制.按钮.雷达, 字体加粗); // 人物方框
                                    if (绘制.按钮.雷达)
                                    {
                                        gui->slider_float("雷达X", &绘制.按钮.雷达X, 0, displayInfo.width * 2, "%.f");
    
                                        gui->slider_float("雷达Y", &绘制.按钮.雷达Y, 0, displayInfo.height * 2, "%.f");
    
                                        gui->slider_float("雷达大小", &绘制.念宝.雷达大小, 1.0, 300, "%.0f%%");
    
                                        gui->slider_float("雷达缩放", &绘制.念宝.雷达缩放, 0, 2, "%.1d");
                                    }
                                    ImGui::Dummy(ImVec2(0, 5));
                                }
                                gui->end_child();
                            }
                            gui->end_group();
                        }
                        gui->end_content();
                        break;
    
                    case 1:
                        gui->begin_content();
                        {
                            gui->begin_group();
                            {
                                gui->begin_child("预警");
                                {
                                    //预警
                                    gui->checkbox("投掷物预警", &绘制.按钮.手雷预警, 字体加粗);
                                    gui->checkbox("投掷物预警圆圈", &绘制.念宝.手雷圆圈, 字体加粗);
                                }
                                gui->end_child();
    
    
                                gui->begin_child("盒子类");
                                {
                                    //盒子
                                    gui->checkbox("人物盒子", &绘制.按钮.盒子, 字体加粗);
    
                                    gui->checkbox("物资箱", &绘制.按钮.绘制宝箱, 字体加粗);
    
                                    gui->checkbox("遍历物资箱", &绘制.按钮.超级物资箱, 字体加粗);
    
                                    gui->checkbox("武器箱", &绘制.按钮.绘制武器箱, 字体加粗);
                                    
                                    gui->checkbox("经典空投", &绘制.按钮.空投, 字体加粗);
                                }
                                gui->end_child();
    
    
                                gui->begin_child("物资设置");
                                {
                                    gui->color_edit("物资颜色", (float*)&绘制.物资颜色, ImGuiColorEditFlags_AlphaBar, true);
                                }
                                gui->end_child();
                            }
                            gui->end_group();
    
                            gui->sameline();
    
                            gui->begin_group();
                            {
                                gui->begin_child("载具类");
                                {
                                    //载具
                                    gui->checkbox("绘制载具", &绘制.按钮.车辆, 字体加粗);
    
                                    gui->checkbox("载具耐久", &绘制.按钮.车辆血量, 字体加粗);
                                    
                                    gui->checkbox("载具油量", &绘制.按钮.车辆油量, 字体加粗);
                                }
                                gui->end_child();
    
    
                                gui->begin_child("道具类");
                                {
                                    gui->checkbox("信号枪", &绘制.按钮.绘制信号枪, 字体加粗);
                                    gui->checkbox("自救器", &绘制.按钮.自救器, 字体加粗);
                                    gui->checkbox("金色插件", &绘制.按钮.绘制金插, 字体加粗);
                                    gui->checkbox("密库钥匙", &绘制.按钮.密库钥匙, 字体加粗);
                                    gui->checkbox("召回信号枪", &绘制.按钮.召回信号枪, 字体加粗);
                                    gui->checkbox("信号枪子弹", &绘制.按钮.显示信号弹, 字体加粗);
                                }
                                gui->end_child();
    
    
                                gui->begin_child("其他物资");
                                {
                                    gui->checkbox("显示步枪", &绘制.按钮.显示步枪, 字体加粗);
                                    gui->checkbox("显示冲锋枪", &绘制.按钮.冲锋枪械, 字体加粗);
                                    gui->checkbox("显示狙击枪", &绘制.按钮.狙击枪械, 字体加粗);
                                    gui->checkbox("显示霰弹枪", &绘制.按钮.散弹枪械, 字体加粗);
    
                                    gui->checkbox("调试模式", &绘制.按钮.Debug, 字体加粗);
                                    ImGui::Dummy(ImVec2(0, 5));
                                }
                                gui->end_child();
                            }
                            gui->end_group();
                        }
                        gui->end_content();
                        break;
    
    
                    case 2:
                        gui->begin_content();
                        {
                            gui->begin_group();
                            {
                                gui->begin_child("自瞄调试");
                                {
                                    ReadText(ImColor(gui->get_clr(clr->TextColored)), ImColor(gui->get_clr(clr->Texticon)), YuanerSet->Texticon, "说明: 根据自己的设备选择方案");
                                    gui->checkbox("开启自瞄", &绘制.自瞄.初始化, 字体加粗);
                                    gui->checkbox("隐藏圆圈", &绘制.自瞄.隐藏自瞄圈, 字体加粗);
                                    gui->checkbox("动态圆圈", &绘制.自瞄.动态自瞄, 字体加粗);
                                    gui->checkbox("持续锁定", &绘制.自瞄.三倍压枪, 字体加粗);
                                    gui->checkbox("准星射线", &绘制.自瞄.准星射线, 字体加粗);
                                    gui->checkbox("智能自瞄", &绘制.自瞄.智能自瞄, 字体加粗);
    
                                    const char* AimingPart[4] = {"锁定头部", "锁定胸部", "锁定下体", "漏哪打哪"};
                                    gui->dropdown("自瞄部位", &绘制.自瞄.瞄准部位, AimingPart, 4);
    
                                    const char* condition[2] = {"准心优先", "距离优先"};
                                    gui->dropdown("判断条件", &绘制.自瞄.瞄准优先, condition, 2);
                                    
                                    const char* ensu_items[3] = {"开火锁定", "开镜锁定", "开火开镜"};
                                    gui->dropdown("自瞄条件", &绘制.自瞄.自瞄条件, ensu_items, 3);
                                    
                                    const char* conditionCDK[2] = {"充电口右", "充电口左"};
                                    gui->dropdown("充电口方向", &绘制.自瞄.充电口方向, conditionCDK, 2);
                                }
                                gui->end_child();
    
                                gui->begin_child("自瞄配置");
                                {
                                    gui->checkbox("不瞄人机", &绘制.自瞄.人机不瞄, 字体加粗); //忽略人机
                                    gui->checkbox("不瞄倒地", &绘制.自瞄.倒地不瞄, 字体加粗); //忽略倒地
                                    gui->checkbox("不瞄烟雾", &绘制.自瞄.烟雾不描, 字体加粗); //忽略烟雾
                                    
                                    gui->slider_float("自瞄范围", &绘制.自瞄.自瞄范围, 0.0f, 500.0f, "%.2f");
    
                                    gui->slider_float("腰射距离限制", &绘制.自瞄.腰射距离限制, 0.0f, 600.0f, "%.1f");
                                    gui->slider_float("开镜距离限制", &绘制.自瞄.自瞄距离限制, 0.0f, 600.0f, "%.1f");
                                    gui->slider_float("喷子距离限制", &绘制.自瞄.喷子距离限制, 0.0f, 200.0f, "%.1f");
                                    gui->slider_float("龙息弹距离限制", &绘制.自瞄.龙息弹距离限制, 0.0f, 50.0f, "%.1f");
                                    
                                    gui->slider_float("自瞄预判", &绘制.自瞄.预判力度, 0.0f, 2.0f, "%.1f");
                                    gui->slider_float("扫车预判", &绘制.预判度.扫车, 0.0f, 600.0f, "%.1f");
                                    gui->slider_float("喷子距离限制", &绘制.自瞄.喷子距离限制, 0.0f, 200.0f, "%.1f");
                                }
                                gui->end_child();
                            }
                            gui->end_group();
    
    
                            gui->sameline(); //不换行
    
    
                            gui->begin_group();
                            {
                                gui->begin_child("自瞄设置");
                                {
                                    const char *typegrons[2] = {"陀螺仪自瞄", "触摸自瞄"};
                                    gui->dropdown("自瞄算法", &绘制.自瞄.自瞄算法, typegrons, 2);
                                    if (绘制.自瞄.自瞄算法 != 0) 
                                    {
                                        gui->checkbox("触摸位置", &绘制.自瞄.触摸位置);
                                        gui->slider_float("触摸范围大小", &绘制.自瞄.触摸范围, 5, 600, "%.0f");
                                    } 
                                    if (绘制.自瞄.自瞄算法 != 0) 
                                    {
                                        gui->slider_float("触摸自瞄速度", &绘制.自瞄.自瞄速度, 1.0f, 40.0f, "%.2f");
                                    } 
                                    else 
                                    {
                                        gui->slider_float("陀螺仪自瞄速度", &绘制.自瞄.陀螺仪自瞄速度, 1.0f, 25.0f, "%.2f");
                                    }
                                }
                                gui->end_child();
    
    
                                gui->begin_child("高阶算法(非特殊情况勿动)");
                                {
                                    gui->checkbox("框内自瞄", &绘制.自瞄.框内自瞄, 字体加粗);
                                    gui->checkbox("掉血自瞄", &绘制.自瞄.掉血自瞄, 字体加粗);
                                    gui->checkbox("龙息弹自瞄", &绘制.自瞄.龙息弹, 字体加粗);
                                    gui->checkbox("近战不自瞄", &绘制.自瞄.近战限制开关, 字体加粗);
                                    
                                    ReadText(ImColor(gui->get_clr(clr->TextColored)), ImColor(gui->get_clr(clr->Texticon)), YuanerSet->Texticon, "说明: 不懂别乱调");
                                    if (绘制.自瞄.框内自瞄) 
                                    {
                                        gui->slider_float("框内左右范围", &绘制.按钮.框内范围X, 10, 250, "%.0f");
                                        gui->slider_float("框内上下范围", &绘制.按钮.框内范围Y, 10, 250, "%.0f");
                                    }
                                    if (绘制.自瞄.近战限制开关) 
                                    {
                                        gui->slider_float("近战限制距离", &绘制.自瞄.近战限制距离, 2, 15, "%.0f");
                                    }
                                    if (绘制.自瞄.掉血自瞄) 
                                    {
                                        gui->slider_float("掉血触发阉值", &绘制.自瞄.掉血自瞄数率, 5, 25, "%.0f");
                                    }
                                }
                                gui->end_child();
    
                                gui->begin_child("压枪调整");
                                {
                                    ReadText(ImColor(gui->get_clr(clr->TextColored)), ImColor(gui->get_clr(clr->Texticon)), YuanerSet->Texticon, "说明: 不同状态压枪调节");
                                    gui->slider_float("站立压枪", &绘制.自瞄.压枪力度, 0.0f, 4.0f, "%.2f");
                                    gui->slider_float("蹲下压枪", &绘制.念宝.蹲下压枪力度, 0.0f, 3.0f, "%.2f");
                                    gui->slider_float("趴下压枪", &绘制.念宝.趴下压枪力度, 0.0f, 2.0f, "%.2f");
                                    gui->slider_float("喷子压枪", &绘制.念宝.喷子压枪力度, 0.00f, 10.0f, "%.2f");
                                }
                                gui->end_child();
                            }
                            gui->end_group();
                        }
                        gui->end_content();
                        break;
                        
                    case 3:
                        gui->begin_content();
                        {
                            gui->begin_group();
                            {
                                gui->begin_child("追踪调试");
                                {
                                    ReadText(ImColor(gui->get_clr(clr->TextColored)), ImColor(gui->get_clr(clr->Texticon)), YuanerSet->Texticon, "说明: 根据自己的需求调整");
                                    gui->checkbox("初始化追踪", &绘制.自瞄.开启追踪, 字体加粗);
                                    gui->checkbox("显示圆圈", &绘制.自瞄.显示范围, 字体加粗);
                                    gui->checkbox("准星射线", &绘制.自瞄.准星射线, 字体加粗);
    
                                    const char* AimingZZPart[5] = {"头部", "腰部", "屁股", "指哪打哪", "漏哪打哪"};
                                    gui->dropdown("追踪部位", &绘制.自瞄.追踪瞄准部位, AimingZZPart, 5);
                                }
                                gui->end_child();
    
                                gui->begin_child("追踪配置");
                                {
                                    gui->checkbox("倒地不追", &绘制.自瞄.倒地不追, 字体加粗);
                                    gui->checkbox("人机不追", &绘制.自瞄.人机不追, 字体加粗);
                                    
                                    gui->slider_float("腰射距离", &绘制.自瞄.追踪腰射距离, 0, 300, "%.1f");
                                    gui->slider_float("开镜距离", &绘制.自瞄.追踪开镜距离, 0.0f, 300, "%.1f");
                                    gui->slider_float("追踪距离", &绘制.自瞄.喷子追踪距离, 0.0f, 300, "%.1f");
                                    
                                    gui->slider_float("追踪预判", &绘制.自瞄.追踪预判力度, 0, 5, "%.1f");
                                    gui->slider_float("腰射追踪范围", &绘制.自瞄.追踪腰射范围, 0, 500, "%.1f");
                                    gui->slider_float("开镜追踪范围", &绘制.自瞄.追踪开镜范围, 0, 500, "%.1f");
                                    gui->slider_float("喷子追踪范围", &绘制.自瞄.喷子开镜范围, 0.0f, 300, "%.1f");
                                }
                                gui->end_child();
                            }
                            gui->end_group();
    
                            gui->sameline(); // 换行
    
                            gui->begin_group();
                            {
                                gui->begin_child("追踪概率");
                                {
                                    gui->slider_float("腰射概率", &绘制.自瞄.追踪腰射概率, 0, 100.0f, "%.1f");
                                    gui->slider_float("开镜概率", &绘制.自瞄.追踪开镜概率, 0.0f, 100.0f, "%.1f");
                                    gui->slider_float("喷子概率", &绘制.自瞄.喷子追踪概率, 0.0f, 100, "%.1f");
                                    gui->slider_float("弓弩概率", &绘制.自瞄.弓追踪概率, 0.0f, 100, "%.1f");
                                    gui->slider_float("狙击枪概率", &绘制.自瞄.栓狙追踪概率, 0.0f, 100, "%.1f");
                                    gui->slider_float("射手步枪概率", &绘制.自瞄.连狙追踪概率, 0.0f, 100, "%.1f");
                                    gui->slider_float("冲锋枪概率", &绘制.自瞄.冲锋枪追踪概率, 0.0f, 100, "%.1f");
                                }
                                gui->end_child();
                            }
                            gui->end_group();
                        }
                        gui->end_content();
                        break;
    
                    case 4:
                        gui->begin_content();
                        {
                            gui->begin_group();
                            {
                                gui->begin_child("设置");
                                {
                                    gui->slider_int("绘图帧率", &绘制.按钮.当前帧率, 60.0, 166.0, "%.d");
                                    gui->slider_float("触摸采样率", &绘制.自瞄.触摸采样率, 200.0, 1600.0, "%.1f");
    
                                    if (gui->button("退出并注销"))
                                    {
                                        绘制.保存配置("Elysium_Deploy");
                                        quick_exit(0);
                                    }
                                }
                                gui->end_child();
                                gui->begin_child("数据");
                                {
                                    ImGui::TextColored(ImVec4(1,1,1,1), "世界地址: 0x%llX", 绘制.地址.世界地址);
                                    ImGui::TextColored(ImVec4(1,1,1,1), "自身地址: 0x%llX", 绘制.地址.自身地址);
                                    ImGui::TextColored(ImVec4(1,1,1,1), "矩阵地址: 0x%llX", 绘制.地址.矩阵地址);
                                    ImGui::TextColored(ImVec4(1,1,1,1), "数组地址: 0x%llX", 绘制.地址.数组地址);
                                    ImGui::TextColored(ImVec4(1,1,1,1), "类地址: 0x%llX", 绘制.地址.类地址);
                                    ImGui::TextColored(ImVec4(1,1,1,1), "LIBUE4: 0x%llX", 绘制.地址.libue4);
                                    
                                    ImGui::TextColored(ImVec4(1,1,1,1), "坐标: X=%.2f, Y=%.2f, Z=%.2f", 绘制.自身数据.坐标.X, 绘制.自身数据.坐标.Y, 绘制.自身数据.坐标.Z);
                                    ImGui::TextColored(ImVec4(1,1,1,1), "自身队伍: %d", 绘制.自身数据.自身队伍);
                                    ImGui::TextColored(ImVec4(1,1,1,1), "自身状态: %d", 绘制.自身数据.自身状态);
                                    ImGui::TextColored(ImVec4(1,1,1,1), "开镜: %d", 绘制.自身数据.开镜);
                                    ImGui::TextColored(ImVec4(1,1,1,1), "开火: %d", 绘制.自身数据.开火);
                                    ImGui::TextColored(ImVec4(1,1,1,1), "喷子开火: %d", 绘制.自身数据.喷子开火);
                                    ImGui::TextColored(ImVec4(1,1,1,1), "手持ID: %d", 绘制.自身数据.手持id);
                                    ImGui::TextColored(ImVec4(1,1,1,1), "FOV: %.2f", 绘制.自身数据.Fov);
                                    ImGui::TextColored(ImVec4(1,1,1,1), "子弹速度: %.2f", 绘制.自身数据.子弹速度);
                                    ImGui::TextColored(ImVec4(1,1,1,1), "旋转角: %.2f", 绘制.自身数据.准星Y);
                                    
                                    ImGui::TextColored(ImVec4(1,1,1,1), "POV位置: X=%.2f, Y=%.2f, Z=%.2f", 绘制.地址.PovLocation.X, 绘制.地址.PovLocation.Y, 绘制.地址.PovLocation.Z);
                                    ImGui::TextColored(ImVec4(1,1,1,1), "POV旋转: Pitch=%.2f, Yaw=%.2f, Roll=%.2f", 绘制.地址.PovRotation.Pitch, 绘制.地址.PovRotation.Yaw, 绘制.地址.PovRotation.Roll);
                                    
                                    for (int i = 0; i < 16; i++)
                                    {
                                        ImGui::TextColored(ImVec4(1,1,1,1), "Matrix[%d]: %.2f", i, 绘制.自身数据.矩阵[i]);
                                        if ((i + 1) % 4 == 0) ImGui::Separator();
                                    }
                                    
                                    ImGui::TextColored(ImVec4(1,1,1,1), "解密模式: %s", 绘制.按钮.解密 ? "开启" : "关闭");
                                    ImGui::Dummy(ImVec2(0, 5));
                                }
                                gui->end_child();
                            }
                            gui->end_group();
                            
                            gui->sameline(); // 换行
                            
                            gui->begin_group();
                            {
                                gui->begin_child("参数");
                                {
                                    gui->checkbox("模型显示", &绘制.按钮.模型绘制, 字体加粗);
                                    
                                    gui->checkbox("屏幕水印", &YuanerSet->watermark, "有关游戏中发生的情况的基本信息的描述");
    
                     				gui->dropdown("水印位置", &YuanerSet->watermark_selection, YuanerSet->watermark_list);
            
            						gui->checkbox("水印标签", &YuanerSet->water_name, 字体加粗);
            
                                    gui->slider_float("窗口DPI缩放", &NewDPI, 1.0, 2.0, "%.1f");
                                    if (gui->button("应用DPI缩放"))
                                    {
                                        DPI = NewDPI;
                                    }
                                }
                                gui->end_child();
                                
                                gui->begin_child("解密");
                                {
                                    gui->checkbox("数组解密", &绘制.按钮.解密, 字体加粗);
                                }
                                gui->end_child();
                            }
                            gui->end_group();
                        }
                        gui->end_content();
                    }
                    ImGui::PopStyleVar();
                    gui->end_content(); 
                }
            }
            gui->end();
            ImGui::PopStyleVar();
        }
    }
    
    bool ping = false;
        
    if (ping) {
        RealtimePing::get().start("223.5.5.5"); 
        ping = true;
    }
        
    int p_val = RealtimePing::get().get_ping();
    std::string ping_str = (p_val <= 0) ? "TIMEOUT" : (std::to_string(p_val) + "MS");
    static float last_fps_update = 0.0f;
    static std::string cached_fps = "0FPS";
    if (ImGui::GetTime() - last_fps_update >= 1.0f) {
        cached_fps = std::to_string(static_cast<int>(ImGui::GetIO().Framerate)) + "FPS";
        last_fps_update = ImGui::GetTime();
    }
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    char time_str[32];
    std::strftime(time_str, sizeof(time_str), "%H:%M:%S", std::localtime(&now_time));
    std::vector<std::string> watermark_content = { "T.O.M.I.E", "正义", cached_fps, ping_str, time_str };    
    gui->water_mark("watermark", watermark_content, static_cast<watermark_position>(YuanerSet->watermark_selection), &YuanerSet->watermark);
    ImGui::PushFont(font::opoo);
}