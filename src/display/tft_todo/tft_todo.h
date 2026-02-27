// tft_todo.h - Todo 列表显示模块
#pragma once
#include <TFT_eSPI.h>

// 前向声明
class TodoManager;
struct TodoItem;

class TFTTodo {
public:
    TFTTodo(TFT_eSPI& tft) : tft(tft) {}
    
    // 初始化 Todo 区域
    void begin();
    
    // 更新 Todo 列表显示
    void update(TodoManager& todoMgr, bool force = false);
    
    // 滚动显示
    void scrollUp();
    void scrollDown();
    
private:
    TFT_eSPI& tft;
    int scrollOffset = 0;      // 滚动偏移
    int lastCount = -1;        // 上次Todo数量
    bool initialized = false;
    
    void drawBackground();
    void drawTodoItem(const TodoItem& item, int y, int index);
    int getMaxVisibleItems();
};
