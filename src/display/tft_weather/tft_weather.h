// tft_weather.h - 天气显示模块
#pragma once
#include <TFT_eSPI.h>

// 前向声明
struct NowWeather;
struct TomorrowWeather;

class TFTWeather {
public:
    TFTWeather(TFT_eSPI& tft) : tft(tft) {}
    
    // 初始化天气区域
    void begin();
    
    // 更新天气显示 (今日+明日)
    void update(const NowWeather& today, const TomorrowWeather& tomorrow, bool force = false);
    
private:
    TFT_eSPI& tft;
    bool initialized = false;
    String lastText = "";
    
    void drawBackground();
    void drawTodayWeather(const NowWeather& weather, int x, int y);
    void drawTomorrowWeather(const TomorrowWeather& weather, int x, int y);
    void drawBottomBar(const NowWeather& weather, int y);
};
