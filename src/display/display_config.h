// display_config.h - 浅色主题配色与布局配置
#pragma once
#include <TFT_eSPI.h>

// ====== 浅色主题配色 ======
namespace HarmonyOS {
    // 背景色
    constexpr uint16_t BG_PRIMARY   = 0xFFFF;   // #FFFFFF - 主背景(白色)
    constexpr uint16_t BG_CARD      = 0xF7BE;   // #EFEFEF - 卡片背景(浅灰)
    
    // 文字色
    constexpr uint16_t TEXT_PRIMARY = 0x0000;   // #000000 - 主文字(黑色)
    constexpr uint16_t TEXT_SECONDARY = 0x7BEF; // #666666 - 次要文字
    constexpr uint16_t TEXT_DISABLED = 0xB5B6;  // #999999 - 禁用文字
    
    // 强调色
    constexpr uint16_t ACCENT_BLUE  = 0x055F;   // #007DFF - 蓝色
    constexpr uint16_t ACCENT_GREEN = 0x37E0;   // #00C853 - 绿色
    constexpr uint16_t ACCENT_ORANGE = 0xFD41;  // #FF6D00 - 橙色
    constexpr uint16_t ACCENT_RED   = 0xF143;   // #D32F2F - 红色
    constexpr uint16_t ACCENT_YELLOW = 0xFEA2;  // #FFC107 - 黄色
    constexpr uint16_t ACCENT_PURPLE = 0x911F;  // #8038A6 - 紫色
    
    // 分隔线
    constexpr uint16_t DIVIDER      = 0xD69A;   // #CCCCCC - 分隔线
}

// 兼容旧命名空间
namespace Gruvbox = HarmonyOS;

// ====== 布局配置 ======
namespace Layout {
    // 屏幕尺寸
    constexpr int SCREEN_W = 240;
    constexpr int SCREEN_H = 320;
    
    // 卡片圆角
    constexpr int CORNER_RADIUS = 8;
    
    // 边距
    constexpr int PADDING = 8;
    constexpr int CARD_MARGIN = 4;
    
    // 状态栏 (顶部)
    constexpr int STATUSBAR_HEIGHT = 52;
    constexpr int STATUSBAR_Y = 0;
    
    // 天气卡片 (中部) - 增高以容纳更多内容
    constexpr int WEATHER_HEIGHT = 130;
    constexpr int WEATHER_Y = STATUSBAR_Y + STATUSBAR_HEIGHT + CARD_MARGIN;
    
    // Todo卡片 (底部)
    constexpr int TODO_Y = WEATHER_Y + WEATHER_HEIGHT + CARD_MARGIN;
    constexpr int TODO_HEIGHT = SCREEN_H - TODO_Y - CARD_MARGIN;
}

// ====== 绘图辅助函数 ======
inline void drawCard(TFT_eSPI& tft, int x, int y, int w, int h, uint16_t bgColor) {
    tft.fillRoundRect(x, y, w, h, Layout::CORNER_RADIUS, bgColor);
}

// ====== 图标占位符 ======
namespace Icons {
    void drawWiFi(TFT_eSPI& tft, int x, int y, uint16_t color);
    void drawBattery(TFT_eSPI& tft, int x, int y, uint16_t color, int level = 100);
    void drawWeatherIcon(TFT_eSPI& tft, int x, int y, int code, uint16_t color);
}
