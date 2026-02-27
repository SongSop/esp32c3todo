#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "ArduinoZlib.h"
#include <Ed25519Util.h>
#include "wifi/wifi_mgr.h"
#include "time/time_mgr.h"
#include "weather/weather.h"
#include "todo/todo_mgr.h"
#include "display/display_manager.h"

WebServer server(80);
TodoManager todo;
DisplayManager display;

// 天气数据刷新定时器
unsigned long lastWeatherUpdate = 0;
const unsigned long WEATHER_INTERVAL = 60 * 60 * 1000; // 60分钟刷新一次

// WiFi状态检查定时器
unsigned long lastWiFiCheck = 0;
const unsigned long WIFI_CHECK_INTERVAL = 30 * 1000; // 30秒检查一次网络状态

// NTP对时定时器
unsigned long lastTimeSync = 0;
const unsigned long TIME_SYNC_INTERVAL = 58 * 60 * 1000; // 58分钟进行一次NTP对时

// ======== setup & loop ========
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // 初始化显示系统
  display.begin();
  Serial.println("Display initialized");

  wifiConnect();
  todo.begin(&server);
  syncTime();
  
  // 获取天气数据
  getWeather();
  getAir();
  getFutureWeather();
  
  lastWeatherUpdate = millis();
}

// ================= 串口控制台 =================

String cmdBuffer = "";

void handleCommand(String cmd) {
  cmd.trim();

  if (cmd == "help") {
    Serial.println("====== Command List ======");
    Serial.println("help    - show commands");
    Serial.println("wifi    - check wifi");
    Serial.println("time    - show ntp time");
    Serial.println("weather - get weather");
    Serial.println("air     - get air quality");
    Serial.println("future  - get future weather");
    Serial.println("reboot  - restart");
    Serial.println("==========================");
  }

  else if (cmd == "wifi") {
    Serial.print("WiFi Status: ");
    Serial.println(WiFi.status() == WL_CONNECTED ? "OK" : "DISCONNECTED");
    Serial.println(WiFi.localIP());
  }

  else if (cmd == "time") {
    time_t now = time(nullptr);
    Serial.print("UNIX Time: ");
    Serial.println(now);
  }

  else if (cmd == "weather") {
    Serial.println("Getting weather...");
    getWeather();
  }

  else if (cmd == "air") {
    Serial.println("Getting air quality...");
    getAir();
  }

  else if (cmd == "future") {
    Serial.println("Getting future weather...");
    getFutureWeather();
  }

  else if (cmd == "reboot") {
    Serial.println("Rebooting...");
    ESP.restart();
  }

  else {
    Serial.println("Unknown command. Type: help");
  }
}


void loop() {
  server.handleClient();
  todo.handleRequests();

  // 检查WiFi状态
  if (millis() - lastWiFiCheck > WIFI_CHECK_INTERVAL) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi disconnected, reconnecting...");
      wifiConnect();
    }
    lastWiFiCheck = millis();
  }

  // NTP对时
  if (millis() - lastTimeSync > TIME_SYNC_INTERVAL) {
    syncTime();
    Serial.println("Time synced via NTP");
    lastTimeSync = millis();
  }

  // 定时刷新天气数据
  if (millis() - lastWeatherUpdate > WEATHER_INTERVAL) {
    getWeather();
    getAir();
    getFutureWeather();
    lastWeatherUpdate = millis();
  }
  
  // 每隔一秒刷新显示
  static unsigned long lastUpdate = 0;
  if(millis() - lastUpdate > 1000) {
    time_t currentTime = getUnixTime();
    bool wifiOK = (WiFi.status() == WL_CONNECTED);
    
    // 更新所有显示内容
    display.update(currentTime, wifiOK, nowWeather, tomorrowWeather, todo);
    lastUpdate = millis();
  }

  // 串口命令处理
  while (Serial.available()) {
    char c = Serial.read();
    
    if (c == '\n' || c == '\r') {
      if (cmdBuffer.length() > 0) {
        handleCommand(cmdBuffer);
        cmdBuffer = "";
      }
    } else {
      cmdBuffer += c;
    }
  }

  delay(5);
}
