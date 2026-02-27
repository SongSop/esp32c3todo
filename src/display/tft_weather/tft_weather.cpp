// tft_weather.cpp - 天气显示实现 (左右分栏布局)
#include "tft_weather.h"
#include "../display_config.h"
#include "../../weather/weather.h"
#include "../../font/HarmonyOS_Sans_SC_Regular14.h"

// Meteocons 天气图标 (64x64 RGB565, 透明背景)
#include "../../image/meteocons/clear_day.h"
#include "../../image/meteocons/clear_night.h"
#include "../../image/meteocons/partly_cloudy_day.h"
#include "../../image/meteocons/partly_cloudy_night.h"
#include "../../image/meteocons/cloudy.h"
#include "../../image/meteocons/overcast.h"
#include "../../image/meteocons/rain.h"
#include "../../image/meteocons/drizzle.h"
#include "../../image/meteocons/thunderstorms.h"
#include "../../image/meteocons/thunderstorms_day_rain.h"
#include "../../image/meteocons/snow.h"
#include "../../image/meteocons/sleet.h"
#include "../../image/meteocons/fog.h"
#include "../../image/meteocons/mist.h"
#include "../../image/meteocons/haze.h"
#include "../../image/meteocons/hail.h"
#include "../../image/meteocons/dust.h"
#include "../../image/meteocons/dust_wind.h"
#include "../../image/meteocons/extreme_rain.h"
#include "../../image/meteocons/not_available.h"

using namespace HarmonyOS;
using namespace Layout;

// 图标尺寸
constexpr int ICON_SIZE = 64;

// 天气代码到图标映射 (和风天气代码 -> Meteocons)
const uint16_t* getWeatherIcon(int code) {
    // 晴天
    if (code == 100) return clear_day;          // 晴 (白天)
    if (code == 150) return clear_night;        // 晴 (夜间)

    // 多云/少云
    if (code == 101 || code == 102 || code == 103) return partly_cloudy_day;
    if (code == 151 || code == 152 || code == 153) return partly_cloudy_night;

    // 阴天
    if (code >= 200 && code <= 204) {
        Serial.printf("Weather code %d -> overcast (阴天)\n", code);
        return overcast;
    }
    
    // 阵雨/雷阵雨
    if (code == 300 || code == 301) return rain;
    if (code == 302 || code == 303) return thunderstorms;
    if (code == 304) return thunderstorms_day_rain;
    
    // 雨 (小/中/大/暴雨)
    if (code >= 305 && code <= 309) return drizzle;    // 小雨/毛毛雨
    if (code >= 310 && code <= 318) return rain;        // 大雨/暴雨
    if (code == 399) return rain;
    
    // 雪
    if (code >= 400 && code <= 403) return snow;        // 小/中/大/暴雪
    if (code == 407 || code == 408 || code == 409 || code == 410) return snow;
    if (code == 404 || code == 405 || code == 406) return sleet; // 雨夹雪
    if (code == 313) return sleet;                      // 冻雨
    if (code == 499) return snow;
    
    // 雾/霾
    if (code == 500 || code == 513) return mist;        // 薄雾/轻雾
    if (code >= 501 && code <= 504) return fog;          // 雾
    if (code >= 511 && code <= 512) return fog;          // 浓雾
    if (code >= 507 && code <= 510) return haze;         // 霾
    if (code == 514 || code == 515) return fog;
    
    // 冰雹
    if (code >= 516 && code <= 519) return hail;
    
    // 沙尘
    if (code == 800 || code == 805) return dust;         // 浮尘
    if (code >= 801 && code <= 804) return dust_wind;    // 扬沙/沙尘暴
    if (code >= 806 && code <= 899) return dust_wind;

    // 添加更多可能的阴天代码范围
    if (code == 104) return cloudy;
    if (code == 154) return cloudy;
    if (code >= 205 && code <= 299) return overcast;

    Serial.printf("Unknown weather code: %d\n", code);
    return not_available;  // 未知天气
}

// AQI等级颜色
uint16_t getAQIColor(int aqi) {
    if (aqi <= 50) return ACCENT_GREEN;       // 优
    else if (aqi <= 100) return ACCENT_YELLOW; // 良
    else if (aqi <= 150) return ACCENT_ORANGE; // 轻度污染
    else if (aqi <= 200) return ACCENT_RED;    // 中度污染
    else return 0xA145;                        // 重度污染
}

void TFTWeather::begin() {
    drawBackground();
    initialized = true;
}

void TFTWeather::drawBackground() {
    tft.fillRect(0, WEATHER_Y, SCREEN_W, WEATHER_HEIGHT, BG_PRIMARY);
    drawCard(tft, CARD_MARGIN, WEATHER_Y, SCREEN_W - CARD_MARGIN * 2, WEATHER_HEIGHT, BG_CARD);
}

void TFTWeather::update(const NowWeather& today, const TomorrowWeather& tomorrow, bool force) {
    if (!initialized) begin();
    if (!force && today.text == lastText) return;
    lastText = today.text;
    
    // 清空卡片内容
    int cardX = CARD_MARGIN + PADDING;
    int cardY = WEATHER_Y + PADDING;
    int cardW = SCREEN_W - CARD_MARGIN * 2 - PADDING * 2;
    int cardH = WEATHER_HEIGHT - PADDING * 2;
    tft.fillRect(cardX, cardY, cardW, cardH, BG_CARD);
    
    // 中间分隔线 - 图标高度 (64) + 标题行 (16) + 间距 = 82
    int midX = cardX + cardW / 2;
    int dividerEndY = cardY + 82;
    tft.drawFastVLine(midX, cardY + 14, dividerEndY - cardY - 14, DIVIDER);
    
    // 绘制今日天气 (左半边)
    drawTodayWeather(today, cardX, cardY);
    
    // 绘制明日天气 (右半边)
    drawTomorrowWeather(tomorrow, midX + PADDING, cardY);
    
    // 绘制底部栏 (风向 + 空气质量)
    drawBottomBar(today, cardY + 82);
}

void TFTWeather::drawTodayWeather(const NowWeather& weather, int x, int y) {
    tft.loadFont(HarmonyOS_Sans_SC_Regular14);

    // 标题: "今日 :城市"
    tft.setTextColor(TEXT_SECONDARY, BG_CARD);
    tft.setCursor(x, y);
    tft.print("今日 ");
    tft.setTextColor(ACCENT_BLUE, BG_CARD);
    tft.print(":");
    tft.print(weather.city);

    // 天气图标 (标题下方, 64x64)
    int iconY = y + 18;
    tft.setSwapBytes(true);
    tft.pushImage(x, iconY, ICON_SIZE, ICON_SIZE, getWeatherIcon(weather.icon));
    tft.setSwapBytes(false);

    // 图标右侧: 温度 + 天气状况
    int textX = x + ICON_SIZE + 6;
    tft.setTextColor(TEXT_PRIMARY, BG_CARD);
    tft.setCursor(textX, iconY + 14);  // 向下对齐图标中心
    tft.print(weather.temp);
    tft.print("℃");

    tft.setTextColor(TEXT_SECONDARY, BG_CARD);
    tft.setCursor(textX, iconY + 32);
    tft.print(weather.text);

    tft.unloadFont();
}

void TFTWeather::drawTomorrowWeather(const TomorrowWeather& weather, int x, int y) {
    tft.loadFont(HarmonyOS_Sans_SC_Regular14);

    // 标题 "明日" (顶部)
    tft.setTextColor(TEXT_SECONDARY, BG_CARD);
    tft.setCursor(x, y);
    tft.print("明日");

    // 天气图标 (标题下方, 64x64)
    int iconY = y + 18;
    tft.setSwapBytes(true);
    tft.pushImage(x, iconY, ICON_SIZE, ICON_SIZE, getWeatherIcon(weather.icon));
    tft.setSwapBytes(false);

    // 图标右侧: 温度范围 + 天气状况
    int textX = x + ICON_SIZE + 6;
    tft.setTextColor(TEXT_PRIMARY, BG_CARD);
    tft.setCursor(textX, iconY + 14);  // 向下对齐图标中心
    tft.print(weather.tempMax);
    tft.print("/");
    tft.setTextColor(TEXT_SECONDARY, BG_CARD);
    tft.print(weather.tempMin);
    tft.print("℃");

    tft.setCursor(textX, iconY + 32);
    tft.print(weather.text);

    tft.unloadFont();
}

void TFTWeather::drawBottomBar(const NowWeather& weather, int y) {
    // 分隔线
    int cardW = SCREEN_W - CARD_MARGIN * 2 - PADDING * 2;
    int startX = CARD_MARGIN + PADDING;
    tft.drawFastHLine(startX, y, cardW, DIVIDER);

    tft.loadFont(HarmonyOS_Sans_SC_Regular14);

    int x = startX;
    int textY = y + 16;

    // 左边: 体感温度
    tft.setTextColor(TEXT_SECONDARY, BG_CARD);
    tft.setCursor(x, textY);
    tft.print("体感");
    tft.setTextColor(TEXT_PRIMARY, BG_CARD);
    tft.print(weather.feelsLike);
    tft.print("℃");

    // 中间分隔符
    x += 58;
    tft.setTextColor(DIVIDER, BG_CARD);
    tft.setCursor(x, textY);
    tft.print("|");

    // 中间: 风向
    x += 10;
    tft.setTextColor(ACCENT_BLUE, BG_CARD);
    tft.setCursor(x, textY);
    tft.print(weather.win);

    // 中间分隔符
    x += 70;
    tft.setTextColor(DIVIDER, BG_CARD);
    tft.setCursor(x, textY);
    tft.print("|");

    // 右边: 空气质量
    x += 10;
    tft.setTextColor(TEXT_SECONDARY, BG_CARD);
    tft.setCursor(x, textY);
    tft.print("空气");
    tft.setTextColor(getAQIColor(weather.air), BG_CARD);
    if (weather.air > 0) {
        tft.print(weather.air);
        if (weather.air <= 50) tft.print("优");
        else if (weather.air <= 100) tft.print("良");
        else if (weather.air <= 150) tft.print("轻");
        else if (weather.air <= 200) tft.print("中");
        else tft.print("重");
    } else {
        tft.print("--");
    }

    tft.unloadFont();
}
