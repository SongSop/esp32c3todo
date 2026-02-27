#include "weather.h"
#include <Ed25519Util.h>

// ====== 私有配置 ======
String LocationID = "101240508";           // 江西省吉安市新干县
String LocationName = "新干县";            // 城市名称显示
char PrivateKey[] = "MC4CAQAwBQYDK2VwBCIEIO3GcvsOCq+JSRGEYaBMmy0KuY1dAYD1JXs4zpJyL4k4";
char PublicKey[] = "MCowBQYDK2VwAyEA+6mMaw3JmfaDGQcm0Z5Me/3yUUeO+4uZk1gEHuEMgyY=";
String KeyID = "KKWED6JPP7";
String ProjectID = "362H8HXGCB";
String ApiHost = "m73md8fcdf.re.qweatherapi.com";
String airURL = "/airquality/v1/current/";
String futureURL = "/v7/weather/7d";
int queryTimeout = 5000;

// ====== 全局变量 ======
NowWeather nowWeather;
TomorrowWeather tomorrowWeather;
bool queryNowWeatherSuccess = false;
bool queryAirSuccess = false;
bool queryFutureWeatherSuccess = false;
String data = "";
uint8_t *outbuf;
HTTPClient httpClient;

// ====== 获取今日天气 ======
void getWeather() {
  String jwt = generateJWT(PrivateKey, PublicKey, KeyID, ProjectID);
  String url = "https://" + String(ApiHost) + "/v7/weather/now?location=" + LocationID;
  
  httpClient.setConnectTimeout(queryTimeout);
  httpClient.begin(url);
  httpClient.addHeader("Authorization", "Bearer " + jwt);
  
  data = "";
  queryNowWeatherSuccess = false;
  
  int httpCode = httpClient.GET();
  Serial.println("正在获取今日天气数据");
  
  if (httpCode == HTTP_CODE_OK) {
    int len = httpClient.getSize();
    uint8_t buff[2048] = { 0 };
    WiFiClient *stream = httpClient.getStreamPtr();
    
    while (httpClient.connected() && (len > 0 || len == -1)) {
      size_t size = stream->available();
      if (size) {
        size_t realsize = ((size > sizeof(buff)) ? sizeof(buff) : size);
        size_t readBytesSize = stream->readBytes(buff, realsize);
        if (len > 0) len -= readBytesSize;
        
        outbuf = (uint8_t *)malloc(sizeof(uint8_t) * 5120);
        uint32_t outprintsize = 0;
        ArduinoZlib::libmpq__decompress_zlib(buff, readBytesSize, outbuf, 5120, outprintsize);
        
        for (int i = 0; i < outprintsize; i++) {
          data += (char)outbuf[i];
        }
        free(outbuf);
      }
      delay(1);
    }
    
    StaticJsonDocument<2048> doc;
    DeserializationError error = deserializeJson(doc, data);
    
    if (!error) {
      String code = doc["code"].as<const char*>();
      if (code.equals("200")) {
        queryNowWeatherSuccess = true;
        nowWeather.city = LocationName;
        nowWeather.text = doc["now"]["text"].as<const char*>();
        nowWeather.icon = doc["now"]["icon"].as<int>();
        nowWeather.temp = doc["now"]["temp"].as<int>();
        nowWeather.feelsLike = doc["now"]["feelsLike"].as<const char*>();
        String windDir = doc["now"]["windDir"].as<const char*>();
        String windScale = doc["now"]["windScale"].as<const char*>();
        nowWeather.win = windDir + windScale + "级";
        nowWeather.humidity = doc["now"]["humidity"].as<int>();
        nowWeather.vis = doc["now"]["vis"].as<const char*>();
        Serial.println("今日天气获取成功");
      }
    }
  }
  
  if (!queryNowWeatherSuccess) {
    Serial.print("请求今日天气错误：");
    Serial.println(httpCode);
  }
  httpClient.end();
}

// ====== 获取空气质量 ======
void getAir() {
  String jwt = generateJWT(PrivateKey, PublicKey, KeyID, ProjectID);
  String lat = "27.75";
  String lon = "115.40";
  
  data = "";
  queryAirSuccess = false;
  String url = "https://" + ApiHost + airURL + lat + "/" + lon;
  
  httpClient.setConnectTimeout(queryTimeout);
  httpClient.begin(url);
  httpClient.addHeader("Authorization", "Bearer " + jwt);
  
  int httpCode = httpClient.GET();
  Serial.println("正在获取空气质量数据");
  
  if (httpCode == HTTP_CODE_OK) {
    int len = httpClient.getSize();
    uint8_t buff[2048] = { 0 };
    WiFiClient *stream = httpClient.getStreamPtr();
    
    while (httpClient.connected() && (len > 0 || len == -1)) {
      size_t size = stream->available();
      if (size) {
        size_t realsize = ((size > sizeof(buff)) ? sizeof(buff) : size);
        size_t readBytesSize = stream->readBytes(buff, realsize);
        if (len > 0) len -= readBytesSize;
        
        outbuf = (uint8_t *)malloc(sizeof(uint8_t) * 20480);
        uint32_t outprintsize = 0;
        ArduinoZlib::libmpq__decompress_zlib(buff, readBytesSize, outbuf, 20480, outprintsize);
        
        for (int i = 0; i < outprintsize; i++) {
          data += (char)outbuf[i];
        }
        free(outbuf);
      }
      delay(1);
    }
    
    Serial.println("空气质量API返回:");
    Serial.println(data);

    StaticJsonDocument<2048> doc;
    DeserializationError error = deserializeJson(doc, data);

    if (!error) {
      // 空气质量API没有code字段，直接检查indexes数组
      JsonArray indexes = doc["indexes"];
      if (indexes.size() > 0) {
        queryAirSuccess = true;
        nowWeather.air = indexes[0]["aqi"].as<int>();
        Serial.print("AQI: ");
        Serial.println(nowWeather.air);

        JsonArray pollutants = doc["pollutants"];
        for (JsonObject pollutant : pollutants) {
          String pcode = pollutant["code"].as<const char*>();
          float value = pollutant["concentration"]["value"].as<float>();

          if (pcode.equals("pm2p5")) nowWeather.pm2p5 = String((int)value);
          else if (pcode.equals("pm10")) nowWeather.pm10 = String((int)value);
        }
        Serial.println("空气质量获取成功");
      } else {
        Serial.println("indexes数组为空");
      }
    } else {
      Serial.println("JSON解析失败");
    }
  }
  
  if (!queryAirSuccess) {
    Serial.print("请求空气质量错误：");
    Serial.println(httpCode);
  }
  httpClient.end();
}

// ====== 获取未来天气(只需要明天) ======
void getFutureWeather() {
  String jwt = generateJWT(PrivateKey, PublicKey, KeyID, ProjectID);
  
  data = "";
  queryFutureWeatherSuccess = false;
  String url = "https://" + ApiHost + futureURL + "?location=" + LocationID;
  
  httpClient.setConnectTimeout(queryTimeout);
  httpClient.begin(url);
  httpClient.addHeader("Authorization", "Bearer " + jwt);
  
  int httpCode = httpClient.GET();
  Serial.println("正在获取未来天气数据");
  
  if (httpCode == HTTP_CODE_OK) {
    int len = httpClient.getSize();
    uint8_t buff[2048] = { 0 };
    WiFiClient *stream = httpClient.getStreamPtr();
    
    while (httpClient.connected() && (len > 0 || len == -1)) {
      size_t size = stream->available();
      if (size) {
        size_t realsize = ((size > sizeof(buff)) ? sizeof(buff) : size);
        size_t readBytesSize = stream->readBytes(buff, realsize);
        if (len > 0) len -= readBytesSize;
        
        outbuf = (uint8_t *)malloc(sizeof(uint8_t) * 5120);
        uint32_t outprintsize = 0;
        ArduinoZlib::libmpq__decompress_zlib(buff, readBytesSize, outbuf, 5120, outprintsize);
        
        for (int i = 0; i < outprintsize; i++) {
          data += (char)outbuf[i];
        }
        free(outbuf);
      }
      delay(1);
    }
    
    StaticJsonDocument<4096> doc;
    DeserializationError error = deserializeJson(doc, data);
    
    if (!error) {
      String code = doc["code"].as<const char*>();
      if (code.equals("200")) {
        queryFutureWeatherSuccess = true;
        // 只取明天的数据 (daily[1])
        tomorrowWeather.text = doc["daily"][1]["textDay"].as<const char*>();
        tomorrowWeather.icon = doc["daily"][1]["iconDay"].as<int>();
        tomorrowWeather.tempMax = doc["daily"][1]["tempMax"].as<int>();
        tomorrowWeather.tempMin = doc["daily"][1]["tempMin"].as<int>();
        Serial.println("未来天气获取成功");
      }
    }
  }
  
  if (!queryFutureWeatherSuccess) {
    Serial.print("请求未来天气错误：");
    Serial.println(httpCode);
  }
  httpClient.end();
}
