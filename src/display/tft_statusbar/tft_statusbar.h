// tft_statusbar.h - 顶部状态栏
#pragma once
#include <TFT_eSPI.h>
#include <time.h>

class TFTStatusBar {
public:
    TFTStatusBar(TFT_eSPI& tft) : tft(tft) {}
    
    // 初始化状态栏
    void begin();
    
    // 刷新状态栏
    void update(time_t currentTime, bool wifiOK);

private:
    TFT_eSPI& tft;
    
    int lastMin = -1;  // 分钟变化才重绘
    
    void drawBackground();
    void drawTime(time_t currentTime);
    void drawWiFi(bool wifiOK);
    void drawWeekday(int wday);
};
