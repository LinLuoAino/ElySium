#ifndef TXNET_H
#define TXNET_H

#ifdef __cplusplus
extern "C" {
#endif

// ==================== 连接状态常量 ====================
#define TXNET_STATUS_DISCONNECTED   0   // 已断开
#define TXNET_STATUS_CONNECTING     1   // 连接中
#define TXNET_STATUS_CONNECTED      2   // 已连接
#define TXNET_STATUS_RECONNECTING   3   // 重连中
#define TXNET_STATUS_FAILED         4   // 连接失败

// ==================== 模式常量 ====================
#define TXNET_MODE_DEFAULT  0   // 默认模式
#define TXNET_MODE_CUSTOM   1   // 自定义模式

// ==================== 1. 配置系统 ====================

/**
 * 初始化配置结构体
 */
typedef struct {
    const char* app_id;             // 应用ID
    const char* server_public_key;  // 服务器RSA公钥(PEM格式)
    const char* client_private_key; // 客户端RSA私钥(PEM格式)
    int version_code;               // 版本号
    const char* version_name;       // 版本名称(如"1.0.0")
    const char* login_save_path;    // 登录码保存文件路径(如"/data/local/tmp/login.dat")
} txnet_config;

/**
 * 初始化库(必须首先调用)
 * @param config 配置结构体指针
 */
void txnet_init(const txnet_config* config);

// ==================== 2. 服务器模块 ====================

/**
 * 连接状态回调函数
 * @param status  状态码(TXNET_STATUS_*)
 * @param message 状态描述
 */
typedef void (*txnet_connection_callback)(int status, const char* message);

/**
 * 设置连接模式
 * @param mode 0=默认模式(自动重连/超时退出), 1=自定义模式(仅回调)
 */
void txnet_set_connection_mode(int mode);

/**
 * 设置最大重连次数(默认模式有效)
 * @param count 最大次数(默认5次)
 */
void txnet_set_max_reconnect(int count);

/**
 * 设置重连间隔(默认模式有效)
 * @param seconds 间隔秒数(默认3秒)
 */
void txnet_set_reconnect_interval(int seconds);

/**
 * 设置连接状态回调(默认/自定义模式都会回调)
 * @param cb 回调函数
 */
void txnet_set_connection_callback(txnet_connection_callback cb);

/**
 * 连接服务器(阻塞直到连接成功或失败)
 * 默认模式: 失败后重试,超过最大次数则exit(1)
 * 自定义模式: 失败后直接返回false
 * @return 是否连接成功
 */
bool txnet_connect();

/**
 * 断开连接
 */
void txnet_disconnect();

/**
 * 检查连接状态
 * @return 是否已连接
 */
bool txnet_is_connected();

// ==================== 3. 登录系统 ====================

/**
 * 登录回调函数
 * @param success     是否成功
 * @param code        错误码(成功时为0)
 * @param message     消息
 * @param expire_time 到期时间戳毫秒(成功时有效)
 */
typedef void (*txnet_login_callback)(bool success, int code, const char* message, long long expire_time);

/**
 * 设置登录模式
 * @param mode 0=默认模式(失败重试直到成功), 1=自定义模式(直接返回)
 */
void txnet_set_login_mode(int mode);

/**
 * 设置登录回调(默认/自定义模式都会回调)
 * @param cb 回调函数
 */
void txnet_set_login_callback(txnet_login_callback cb);

/**
 * 预设登录码(可选,不设置则使用保存的登录码或控制台输入)
 * @param code 登录码字符串
 */
void txnet_set_login_code(const char* code);

/**
 * 开始登录(阻塞直到登录成功或退出)
 * 默认模式: 失败后提示输入卡密重试,直到成功
 * 自定义模式: 单次尝试后返回
 */
void txnet_login();

/**
 * 检查登录状态
 * @return 是否已登录
 */
bool txnet_is_logged_in();

/**
 * 获取到期时间
 * @return 到期时间戳(毫秒)
 */
long long txnet_get_expire_time();

// ==================== 4. 更新系统 ====================

/**
 * 更新回调函数
 * @param need_update    是否需要更新
 * @param force_update   是否强制更新
 * @param latest_version 最新版本号
 * @param version_name   版本名称
 * @param update_content 更新内容
 * @param download_url   下载链接
 * @param netdisk_url    网盘链接(可为空)
 */
typedef void (*txnet_update_callback)(
    bool need_update,
    bool force_update,
    int latest_version,
    const char* version_name,
    const char* update_content,
    const char* download_url,
    const char* netdisk_url
);

/**
 * 设置更新模式
 * @param mode 0=默认模式(控制台交互), 1=自定义模式(仅回调)
 */
void txnet_set_update_mode(int mode);

/**
 * 设置更新回调(默认/自定义模式都会回调)
 * @param cb 回调函数
 */
void txnet_set_update_callback(txnet_update_callback cb);

/**
 * 检查更新(阻塞直到完成)
 * 默认模式: 显示更新信息,强制更新打开浏览器并exit(0)
 * 自定义模式: 仅回调,由主程序处理
 */
void txnet_check_update();

// ==================== 5. 远程配置系统 ====================

/**
 * 配置类型常量
 */
#define TXNET_CONFIG_INT    0
#define TXNET_CONFIG_FLOAT  1
#define TXNET_CONFIG_BOOL   2
#define TXNET_CONFIG_STRING 3
#define TXNET_CONFIG_BUTTON 7   /* 按钮类型：无绑定值，仅通过 buttonTrigger 回调响应 */

/**
 * 配置变更回调函数
 * @param key   配置键名
 * @param type  配置类型(TXNET_CONFIG_*)
 * @param name  配置显示名称
 * @param value 配置值(字符串形式)
 */
typedef void (*txnet_config_callback)(const char* key, int type, const char* name, const char* value);

/**
 * 绑定int配置
 * @param key           配置键名
 * @param ptr           指针
 * @param default_value 默认值
 */
void txnet_bind_config_int(const char* key, int* ptr, int default_value);

/**
 * 绑定float配置
 * @param key           配置键名
 * @param ptr           指针
 * @param default_value 默认值
 */
void txnet_bind_config_float(const char* key, float* ptr, float default_value);

/**
 * 绑定bool配置
 * @param key           配置键名
 * @param ptr           指针
 * @param default_value 默认值
 */
void txnet_bind_config_bool(const char* key, bool* ptr, bool default_value);

/**
 * 绑定string配置
 * @param key           配置键名
 * @param ptr           字符数组指针
 * @param max_len       最大长度
 * @param default_value 默认值
 */
void txnet_bind_config_string(const char* key, char* ptr, int max_len, const char* default_value);

/**
 * 设置配置变更回调
 * @param cb 回调函数
 */
void txnet_set_config_callback(txnet_config_callback cb);

/**
 * 按钮触发回调（后台点击按钮后推送）
 * @param key  功能键名
 * @param name 功能显示名称
 */
typedef void (*txnet_button_callback)(const char* key, const char* name);

/**
 * 设置按钮触发回调
 * @param cb 回调函数，可为 NULL
 */
void txnet_set_button_callback(txnet_button_callback cb);

/**
 * 提交配置结果回调（单条/批量 setConfig 的响应）
 * @param key    功能键名
 * @param name   功能显示名称
 * @param status "success" 或 "fail"
 * @param reason 成功/失败原因
 */
typedef void (*txnet_config_set_result_callback)(const char* key, const char* name, const char* status, const char* reason);

/**
 * 设置提交配置结果回调
 * @param cb 回调函数，可为 NULL
 */
void txnet_set_config_set_result_callback(txnet_config_set_result_callback cb);

/**
 * 获取远程配置(阻塞直到完成)
 */
void txnet_get_config();

/**
 * 提交单条配置到云端（非阻塞，结果通过 txnet_config_set_result_callback 回调）
 * @param key   功能键名
 * @param value 新值（字符串）
 */
void txnet_config_set(const char* key, const char* value);

/**
 * 批量提交配置到云端（非阻塞，每项结果通过 txnet_config_set_result_callback 回调）
 * @param keys   键名数组
 * @param values 值数组（与 keys 一一对应）
 * @param count  数量
 */
void txnet_config_set_batch(const char** keys, const char** values, int count);

/**
 * 手动获取int配置
 * @param key           配置键名
 * @param default_value 默认值
 * @return 配置值
 */
int txnet_config_get_int(const char* key, int default_value);

/**
 * 手动获取float配置
 * @param key           配置键名
 * @param default_value 默认值
 * @return 配置值
 */
float txnet_config_get_float(const char* key, float default_value);

/**
 * 手动获取bool配置
 * @param key           配置键名
 * @param default_value 默认值
 * @return 配置值
 */
bool txnet_config_get_bool(const char* key, bool default_value);

/**
 * 手动获取string配置
 * @param key           配置键名
 * @param default_value 默认值
 * @return 配置值(内部缓存,不要释放)
 */
const char* txnet_config_get_string(const char* key, const char* default_value);

// ==================== 6. 远程变量系统 ====================

/**
 * 远程变量回调函数
 * @param var_name 变量名
 * @param data     变量数据(JSON字符串)
 */
typedef void (*txnet_variable_callback)(const char* var_name, const char* data);

/**
 * 设置远程变量回调
 * @param cb 回调函数
 */
void txnet_set_variable_callback(txnet_variable_callback cb);

/**
 * 获取远程变量(非阻塞,结果通过回调返回)
 * @param var_name 变量名
 * @param data_ptr 可选,接收数据的字符数组(收到响应时自动填充)
 * @param max_len  data_ptr的最大长度
 */
void txnet_get_variable(const char* var_name, char* data_ptr, int max_len);

// ==================== 7. 远程退出系统 ====================

/**
 * 退出类型常量
 */
#define TXNET_EXIT_KICK  0  // 被踢出
#define TXNET_EXIT_EXIT  1  // 服务器退出指令

/**
 * 退出回调函数
 * @param type   退出类型(TXNET_EXIT_KICK/TXNET_EXIT_EXIT)
 * @param reason 退出原因
 */
typedef void (*txnet_exit_callback)(int type, const char* reason);

/**
 * 设置退出模式
 * @param mode TXNET_MODE_DEFAULT(默认,自动退出) / TXNET_MODE_CUSTOM(自定义,仅回调)
 */
void txnet_set_exit_mode(int mode);

/**
 * 设置退出回调
 * @param cb 回调函数
 */
void txnet_set_exit_callback(txnet_exit_callback cb);

// ==================== 8. 共享雷达系统 ====================

/** 可选数值字段“无数据”时的约定值，与前端一致（前端不绘制、不参与计算） */
#define TXNET_RADAR_NO_VALUE (-999)

/**
 * 游戏类型枚举
 */
typedef enum {
    TXNET_RADAR_GAME_PUBGMHD = 0,   // 和平精英
    TXNET_RADAR_GAME_CODEV = 1      // 无畏契约
} txnet_radar_game_type;

/**
 * 初始化雷达模块（程序启动时调用一次）
 * @param enabled_ptr 雷达开关指针
 * @param fps_ptr     雷达帧率指针
 * @param game_type   游戏类型枚举
 */
void txnet_radar_init(bool* enabled_ptr, float* fps_ptr, txnet_radar_game_type game_type);

/**
 * 设置对局数据（每帧调用）
 * 内部自动检测 world_address 变化，变化时清空所有数据（self + items）
 * @param world_address 游戏世界地址（用于换局检测）
 * @param state         游戏状态（0=大厅, 1=对局中）
 * @param map_id        地图ID
 */
void txnet_radar_set_game(unsigned long long world_address, int state, int map_id);

/**
 * 设置自身数据（每帧调用）
 * 仅用于告诉前端“哪条 address 是自身”。其它字段（昵称/队伍/坐标等）由前端根据 items 自行匹配处理。
 * @param address 内存地址
 */
void txnet_radar_set_self(unsigned long long address);

/**
 * PUBGMHD 物品结构体 (玩家/载具/空投统一)
 * 必填字段：address、pos_x/pos_y/pos_z、name；其余均有默认值，按 type 只填该类型所需字段即可。
 * 可选数值“无数据”在库内使用 TXNET_RADAR_NO_VALUE，主程序无需定义，缺省即按无数据处理。
 */
#ifdef __cplusplus
struct txnet_radar_item_pubgmhd {
    const char* type = "player";
    unsigned long long address = 0;
    const char* uid = nullptr;
    const char* name = nullptr;
    int team = 0;
    float pos_x = 0, pos_y = 0, pos_z = 0;
    float health = (float)TXNET_RADAR_NO_VALUE;
    float signal = (float)TXNET_RADAR_NO_VALUE;
    float fuel = (float)TXNET_RADAR_NO_VALUE;
    float rotator = (float)TXNET_RADAR_NO_VALUE;
    float pitch = 0;
    int state = 0;
    bool is_bot = false;
    bool in_vehicle = false;
    int distance = 0;
    int weapon = 0;
    const char* weapon_name = nullptr;
    int current_ammo = TXNET_RADAR_NO_VALUE;
    int max_ammo = TXNET_RADAR_NO_VALUE;
};
#else
typedef struct txnet_radar_item_pubgmhd {
    const char* type;
    unsigned long long address;
    const char* uid;
    const char* name;
    int team;
    float pos_x, pos_y, pos_z;
    float health, signal, fuel, rotator, pitch;
    int state;
    bool is_bot;
    bool in_vehicle;
    int distance;
    int weapon;
    const char* weapon_name;
    int current_ammo, max_ammo;
} txnet_radar_item_pubgmhd;
#endif

/**
 * CODEV 物品结构体 (仅玩家)。必填：address、name、pos_x/pos_y/pos_z；其余有默认值。
 */
#ifdef __cplusplus
struct txnet_radar_item_codev {
    const char* type = "player";
    unsigned long long address = 0;
    const char* uid = nullptr;
    const char* name = nullptr;
    int team = 0;
    float pos_x = 0, pos_y = 0, pos_z = 0;
    float health = (float)TXNET_RADAR_NO_VALUE;
    float rotator = (float)TXNET_RADAR_NO_VALUE;
    int state = 0;
    int distance = 0;
    int weapon = 0;
    const char* weapon_name = nullptr;
};
#else
typedef struct txnet_radar_item_codev {
    const char* type;
    unsigned long long address;
    const char* uid;
    const char* name;
    int team;
    float pos_x, pos_y, pos_z;
    float health, rotator;
    int state, distance, weapon;
    const char* weapon_name;
} txnet_radar_item_codev;
#endif

/**
 * 添加/更新物品（每个物品调用）
 * 根据内存地址判断是新增还是更新，更新时刷新 updateTime
 * @param item 物品数据指针 (txnet_radar_item_pubgmhd* 或 txnet_radar_item_codev*)
 */
void txnet_radar_add_item(const void* item);

// ==================== 同行检测系统 ====================

/**
 * 同行检测结果回调
 * @param uid 玩家UID
 * @param is_same_server 是否为同行
 * @param programs 程序名称数组
 * @param program_count 程序数量
 */
typedef void (*txnet_peer_callback)(
    const char* uid,
    bool is_same_server,
    const char** programs,
    int program_count
);

/**
 * 初始化同行检测
 * @param callback 检测结果回调(可为NULL)
 */
void txnet_peer_init(txnet_peer_callback callback);

/**
 * 提交自身信息
 * 每帧调用，内部自动管理换局检测
 * @param uid 自身游戏UID
 * @param nickname 自身昵称
 * @param world_address 当前世界地址(用于换局检测)
 */
void txnet_peer_submit_self(const char* uid, const char* nickname, unsigned long long world_address);

/**
 * 添加待检测玩家
 * 内部自动去重、限流、批量发送
 * @param uid 玩家UID
 * @param nickname 玩家昵称
 */
void txnet_peer_add_player(const char* uid, const char* nickname);

/**
 * 查询是否为同行(基于缓存)
 * @param uid 玩家UID
 * @return 是否为同行
 */
bool txnet_peer_is_same_server(const char* uid);

/**
 * 获取同行使用的程序列表
 * @param uid 玩家UID
 * @param out_programs 输出数组(调用者分配)
 * @param max_count 最大返回数量
 * @return 实际程序数量
 */
int txnet_peer_get_programs(const char* uid, const char** out_programs, int max_count);

/**
 * 手动清空同行检测状态
 */
void txnet_peer_clear();

#ifdef __cplusplus
}
#endif

#endif // TXNET_H
