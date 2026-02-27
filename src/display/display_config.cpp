// display_config.cpp - 图标绘制实现
#include "display_config.h"

void Icons::drawWiFi(TFT_eSPI& tft, int x, int y, uint16_t color) {
    // 3条递增的信号线
    int heights[] = {6, 10, 14};
    for (int i = 0; i < 3; i++) {
        tft.fillRect(x + i * 6, y + (16 - heights[i]), 4, heights[i], color);
    }
}

void Icons::drawBattery(TFT_eSPI& tft, int x, int y, uint16_t color, int level) {
    // 电池外框
    tft.drawRect(x, y, 24, 12, color);
    tft.fillRect(x + 24, y + 3, 3, 6, color); // 电池头
    
    // 电量填充
    if (level > 20) {
        int fillWidth = map(level, 0, 100, 0, 20);
        tft.fillRect(x + 2, y + 2, fillWidth, 8, color);
    }
}

void Icons::drawWeatherIcon(TFT_eSPI& tft, int x, int y, int code, uint16_t color) {
    // 简单占位符：圆圈 + 编号
    tft.drawCircle(x + 15, y + 15, 15, color);
    tft.setCursor(x + 10, y + 10);
    tft.setTextColor(color);
    tft.setTextSize(1);
    tft.print(code % 10); // 显示图标编号个位数
}
