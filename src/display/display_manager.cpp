// display_manager.cpp - 显示管理器实现
#include "display_manager.h"

using namespace HarmonyOS;
using namespace Layout;

void DisplayManager::begin() {
    // 初始化 TFT
    tft.begin();
    tft.setRotation(0);
    tft.fillScreen(BG_PRIMARY);
    
    // 初始化各个模块
    statusBar.begin();
    weatherDisplay.begin();
    todoDisplay.begin();
    
    // 显示欢迎消息
    tft.setFreeFont(&FreeMonoBold12pt7b);
    tft.setTextColor(ACCENT_BLUE, BG_PRIMARY);
    tft.setCursor(SCREEN_W / 2 - 60, SCREEN_H / 2);
    tft.print("ESP32 Todo");
    
    delay(1000);
    
    // 清屏准备正式显示
    tft.fillScreen(BG_PRIMARY);
    statusBar.begin();
    weatherDisplay.begin();
    todoDisplay.begin();
}

void DisplayManager::update(time_t currentTime, bool wifiOK,
                            const NowWeather& today, const TomorrowWeather& tomorrow,
                            TodoManager& todoMgr) {
    updateStatus(currentTime, wifiOK);
    updateWeather(today, tomorrow);
    updateTodo(todoMgr);
}

void DisplayManager::updateStatus(time_t currentTime, bool wifiOK) {
    statusBar.update(currentTime, wifiOK);
}

void DisplayManager::updateWeather(const NowWeather& today, const TomorrowWeather& tomorrow, bool force) {
    weatherDisplay.update(today, tomorrow, force);
}

void DisplayManager::updateTodo(TodoManager& todoMgr, bool force) {
    todoDisplay.update(todoMgr, force);
}
