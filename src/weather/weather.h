#ifndef WEATHER_H
#define WEATHER_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "ArduinoZlib.h"

// ====== 今日天气结构体 ======
struct NowWeather {
  String city;          // 城市名称
  String text;          // 天气状况
  int icon;             // 天气图标代码
  int temp;             // 当前温度
  String feelsLike;     // 体感温度
  String win;           // 风向风力
  int humidity;         // 湿度
  String vis;           // 能见度
  
  // 空气质量
  int air;              // AQI指数
  String pm2p5;         // PM2.5
  String pm10;          // PM10
};

// ====== 明日天气结构体 ======
struct TomorrowWeather {
  String text;          // 天气状况
  int icon;             // 天气图标代码
  int tempMax;          // 最高温度
  int tempMin;          // 最低温度
};

// ====== 函数声明 ======
void getWeather();      // 获取今日天气
void getAir();          // 获取空气质量
void getFutureWeather(); // 获取未来天气

// ====== 全局变量 ======
extern NowWeather nowWeather;
extern TomorrowWeather tomorrowWeather;
extern bool queryNowWeatherSuccess;
extern bool queryAirSuccess;
extern bool queryFutureWeatherSuccess;

#endif
