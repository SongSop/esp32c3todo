// tft_statusbar.cpp - 状态栏实现 (简洁现代风格)
#include "tft_statusbar.h"
#include "../display_config.h"
#include "../../font/HarmonyOSSans18.h"

using namespace HarmonyOS;
using namespace Layout;

// 星期名称 (需要中文字体支持)
static const char* weekdays[] = {"周日", "周一", "周二", "周三", "周四", "周五", "周六"};

void TFTStatusBar::begin() {
    drawBackground();
}

void TFTStatusBar::drawBackground() {
    tft.fillRect(0, STATUSBAR_Y, SCREEN_W, STATUSBAR_HEIGHT, BG_PRIMARY);
}

void TFTStatusBar::update(time_t currentTime, bool wifiOK) {
    struct tm t;
    localtime_r(&currentTime, &t);
    
    // 只在分钟变化时重绘（减少闪烁）
    if (t.tm_min == lastMin) return;
    lastMin = t.tm_min;
    
    // 清除并重绘
    drawBackground();
    
    // 加载字体
    tft.loadFont(HarmonyOSSans18);
    
    // === 第一行：WiFi图标 + 时间 ===
    drawWiFi(wifiOK);
    
    char timeBuf[8];
    snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d", t.tm_hour, t.tm_min);
    
    int timeWidth = tft.textWidth(timeBuf);
    int timeX = (SCREEN_W - timeWidth) / 2;  // 时间居中
    
    tft.setTextColor(TEXT_PRIMARY, BG_PRIMARY);
    tft.setCursor(timeX, STATUSBAR_Y + 18);
    tft.print(timeBuf);
    
    // === 第二行：星期 + 日期 ===
    char dateBuf[16];
    snprintf(dateBuf, sizeof(dateBuf), "%s %02d/%02d", weekdays[t.tm_wday], t.tm_mon + 1, t.tm_mday);
    
    int dateWidth = tft.textWidth(dateBuf);
    int dateX = (SCREEN_W - dateWidth) / 2;
    
    tft.setTextColor(TEXT_SECONDARY, BG_PRIMARY);
    tft.setCursor(dateX, STATUSBAR_Y + 38);
    tft.print(dateBuf);
    
    tft.unloadFont();
}

void TFTStatusBar::drawTime(time_t currentTime) {
    // 已在 update 中处理
}

void TFTStatusBar::drawWiFi(bool wifiOK) {
    uint16_t color = wifiOK ? ACCENT_GREEN : ACCENT_RED;
    Icons::drawWiFi(tft, 8, STATUSBAR_Y + 2, color);
}

void TFTStatusBar::drawWeekday(int wday) {
    // 已在 update 中处理
}
