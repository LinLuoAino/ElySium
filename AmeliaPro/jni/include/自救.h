// CountdownTimers.h
#ifndef COUNTDOWN_TIMER_H
#define COUNTDOWN_TIMER_H

#include <unordered_map>
#include <chrono>
#include <iostream>
#include <cmath>  // 用于四舍五入

class CountdownTimers {
public:
    CountdownTimers(int duration = 10) : duration_(duration) {}

    // 开始倒计时，只在第一次调用时计时
    void startCountdown(int enemyId) {
        // 如果该敌人已经开始计时，则不做处理
        if (timers_.find(enemyId) == timers_.end()) {
            auto now = std::chrono::steady_clock::now();
            timers_[enemyId] = now; // 记录第一次开始的时间
        }
    }

    // 清除倒计时，如果该敌人没有倒计时记录则不做任何清除操作
    void clearCountdown(int enemyId) {
        // 只有在存在该敌人的倒计时记录时才清除
        if (timers_.find(enemyId) != timers_.end()) {
            timers_.erase(enemyId);
        }
    }

    // 获取倒计时剩余时间（秒），返回浮动类型，保留到0.1秒的精度
    float getRemainingTime(int enemyId) {
        auto now = std::chrono::steady_clock::now();

        // 如果没有倒计时记录，返回0
        if (timers_.find(enemyId) == timers_.end()) {
            return 0.0f;
        }

        auto start_time = timers_[enemyId];
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();

        float remainingTime = duration_ * 1000.0f - elapsed; // 剩余时间（毫秒）

        if (remainingTime <= 0) {
            return 0.0f;
        }

        // 保留到0.1秒精度，四舍五入
        return std::round(remainingTime / 100.0f) * 0.1f;
    }

private:
    int duration_; // 倒计时的时长（秒）
    std::unordered_map<int, std::chrono::steady_clock::time_point> timers_; // 存储每个敌人ID和倒计时的开始时间
};

#endif // COUNTDOWN_TIMER_H
