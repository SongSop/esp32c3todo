// display_manager.h - 显示管理器
#pragma once
#include <TFT_eSPI.h>
#include "display_config.h"
#include "tft_statusbar/tft_statusbar.h"
#include "tft_weather/tft_weather.h"
#include "tft_todo/tft_todo.h"
#include "../weather/weather.h"
#include "../todo/todo_mgr.h"

class DisplayManager {
public:
    DisplayManager() : 
        statusBar(tft), 
        weatherDisplay(tft), 
        todoDisplay(tft) {}
    
    // 初始化显示系统
    void begin();
    
    // 更新所有显示
    void update(time_t currentTime, bool wifiOK,
                const NowWeather& today, const TomorrowWeather& tomorrow, 
                TodoManager& todoMgr);
    
    // 单独更新模块
    void updateStatus(time_t currentTime, bool wifiOK);
    void updateWeather(const NowWeather& today, const TomorrowWeather& tomorrow, bool force = false);
    void updateTodo(TodoManager& todoMgr, bool force = false);
    
    // 滚动 Todo
    void todoScrollUp() { todoDisplay.scrollUp(); }
    void todoScrollDown() { todoDisplay.scrollDown(); }
    
private:
    TFT_eSPI tft;
    TFTStatusBar statusBar;
    TFTWeather weatherDisplay;
    TFTTodo todoDisplay;
};
