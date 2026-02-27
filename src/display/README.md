# ESP32 Todo 显示系统

## 📐 布局设计

```
┌────────────────────────────────┐
│  📶 🔋   2026-02-17 15:30:45  │  ← 状态栏 (40px)
├────────────────────────────────┤
│  Weather                       │
│  ─────────────────────────────│
│  Now:   晴  25°C              │
│  Feel:  26°C                  │  ← 天气区域 (100px)
│  Wind:  东北风 2级  65%       │
│  Vis:   10km                  │
├────────────────────────────────┤
│  Todo List                     │
│  ─────────────────────────────│
│  ☐ 1. 完成项目文档            │
│  ☑ 2. 修复显示Bug             │  ← Todo区域 (剩余高度)
│  ☐ 3. 测试WiFi连接            │
│  ☐ 4. 添加新功能              │
│                      -2 more   │
└────────────────────────────────┘
```

## 🎨 Gruvbox 配色方案

已实现完整的 Gruvbox 配色：

- **背景色**: `#282828` (深灰), `#1d2021` (更深)
- **前景色**: `#ebdbb2` (米黄), `#a89984` (暗淡)
- **高亮色**: 
  - 🟢 绿色 `#b8bb26` - 时间、WiFi连接
  - 🟡 黄色 `#fabd2f` - 标签、标题
  - 🔴 红色 `#fb4934` - 错误、断开
  - 🟠 橙色 `#fe8019` - 风向
  - 🔵 青色 `#8ec07c` - Todo标题
  - 🟣 紫色 `#d3869b` - 能见度

## 📦 模块结构

```
src/display/
├── display_config.h/cpp    # 全局配置和图标
├── display_manager.h/cpp   # 显示管理器
├── tft_statusbar/          # 状态栏模块
├── tft_weather/            # 天气显示模块
└── tft_todo/              # Todo列表模块
```

## 🚀 快速使用

### 1. 初始化显示

```cpp
#include "display/display_manager.h"

DisplayManager display;

void setup() {
    display.begin();  // 初始化显示系统
}
```

### 2. 更新显示

```cpp
void loop() {
    // 每秒更新一次
    static unsigned long lastUpdate = 0;
    if(millis() - lastUpdate > 1000) {
        time_t currentTime = getUnixTime();
        bool wifiOK = (WiFi.status() == WL_CONNECTED);
        int battery = 100;
        
        // 更新所有显示
        display.update(currentTime, wifiOK, battery, 
                      nowWeather, todo);
        lastUpdate = millis();
    }
}
```

### 3. 单独更新模块

```cpp
// 仅更新状态栏
display.updateStatus(currentTime, wifiOK, battery);

// 仅更新天气
display.updateWeather(nowWeather);

// 强制刷新天气
display.updateWeather(nowWeather, true);

// 仅更新Todo列表
display.updateTodo(todo);

// 强制刷新Todo
display.updateTodo(todo, true);
```

### 4. Todo 滚动

```cpp
// 滚动显示更多Todo项
display.todoScrollUp();
display.todoScrollDown();
```

## ⚙️ 配置调整

### 屏幕尺寸

在 `display_config.h` 中修改：

```cpp
namespace Layout {
    constexpr int SCREEN_W = 240;  // 你的屏幕宽度
    constexpr int SCREEN_H = 320;  // 你的屏幕高度
}
```

### 区域高度

```cpp
constexpr int STATUSBAR_H = 40;   // 状态栏高度
constexpr int WEATHER_H = 100;    // 天气区域高度
// Todo区域自动占据剩余空间
```

### 自定义颜色

```cpp
namespace Gruvbox {
    constexpr uint16_t MY_COLOR = 0xabcd;  // RGB565格式
}
```

## 🎯 优化特性

1. **智能刷新**: 只在数据变化时刷新，减少闪烁
2. **滚动显示**: Todo列表支持滚动查看更多项
3. **自适应布局**: 自动计算可显示的Todo数量
4. **主题色**: 每个信息使用不同颜色，清晰易读

## 🔧 下一步建议

1. **添加真实图标**: 
   - 替换 `display_config.cpp` 中的图标绘制函数
   - 使用字库或位图数据

2. **电池电量**: 
   - 连接电池ADC引脚
   - 实现 `getBatteryLevel()` 函数

3. **触摸控制**: 
   - 添加触摸屏支持
   - 实现点击Todo项切换状态

4. **动画效果**: 
   - 添加页面切换动画
   - 天气图标动画

## 📊 内存使用

- **ROM**: 约 15KB (包含字体)
- **RAM**: 约 2KB (显示缓冲区)

## 🐛 常见问题

**Q: 屏幕显示错位?**  
A: 检查 `SCREEN_W` 和 `SCREEN_H` 是否匹配你的硬件

**Q: 字体太小/太大?**  
A: 在各模块中修改 `setFreeFont()` 参数

**Q: 颜色显示不正确?**  
A: 检查TFT_eSPI配置，确保颜色顺序正确

## 📝 API 参考

详见各模块头文件：
- `display_manager.h` - 主接口
- `tft_statusbar.h` - 状态栏
- `tft_weather.h` - 天气显示
- `tft_todo.h` - Todo列表
