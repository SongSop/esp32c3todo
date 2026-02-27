// tft_todo.cpp - Todo 列表显示实现 (HarmonyOS 风格)
#include "tft_todo.h"
#include "../display_config.h"
#include "../../todo/todo_mgr.h"
#include "../../font/HarmonyOSSans18.h"
#include "../../font/HarmonyOS_Sans_SC_Regular16.h"

using namespace HarmonyOS;
using namespace Layout;

void TFTTodo::begin() {
    drawBackground();
    initialized = true;
}

void TFTTodo::drawBackground() {
    tft.fillRect(0, TODO_Y, SCREEN_W, TODO_HEIGHT, BG_PRIMARY);
    drawCard(tft, CARD_MARGIN, TODO_Y, SCREEN_W - CARD_MARGIN * 2, TODO_HEIGHT, BG_CARD);

    // 标题 "Todo"
    tft.loadFont(HarmonyOS_Sans_SC_Regular16);
    tft.setTextColor(ACCENT_BLUE, BG_CARD);
    tft.setCursor(CARD_MARGIN + PADDING, TODO_Y + 5);
    tft.print("待办");

    // 分隔线
    tft.drawFastHLine(CARD_MARGIN + PADDING, TODO_Y + 20,
                      SCREEN_W - CARD_MARGIN * 2 - PADDING * 2, DIVIDER);
    tft.unloadFont();
}

void TFTTodo::update(TodoManager& todoMgr, bool force) {
    if (!initialized) begin();

    int count = todoMgr.size();
    if (!force && count == lastCount && !todoMgr.hasUpdates()) return;
    lastCount = count;
    todoMgr.clearUpdates();  // 清除更新标记

    // 清空内容区域
    int contentY = TODO_Y + 26;
    int cardX = CARD_MARGIN + PADDING;
    tft.fillRect(cardX, contentY, SCREEN_W - CARD_MARGIN * 2 - PADDING * 2,
                 TODO_HEIGHT - 34, BG_CARD);

    if (count == 0) {
        tft.loadFont(HarmonyOS_Sans_SC_Regular16);
        tft.setTextColor(TEXT_SECONDARY, BG_CARD);
        tft.setCursor(cardX + 24, contentY + 10);
        tft.print("无任务");
        tft.unloadFont();
        return;
    }

    // 绘制每个 Todo 项
    int maxItems = getMaxVisibleItems();
    int y = contentY + 1;

    // 加载字体
    tft.loadFont(HarmonyOSSans18);

    for (int i = scrollOffset; i < count && i < scrollOffset + maxItems; i++) {
        const TodoItem& item = todoMgr.getItem(i);
        drawTodoItem(item, y, i);
        y += 22;  // 18pt字体行高更大
    }

    tft.unloadFont();

    // 如果有更多项
    if (count > maxItems) {
        tft.setFreeFont(&FreeMono9pt7b);
        tft.setTextColor(TEXT_SECONDARY, BG_CARD);
        tft.setCursor(SCREEN_W - 45, TODO_Y + TODO_HEIGHT - 8);
        tft.printf("+%d", count - maxItems);
    }
}

void TFTTodo::drawTodoItem(const TodoItem& item, int y, int index) {
    int startX = CARD_MARGIN + PADDING;

    // 方形复选框 - 向下调整与文本对齐
    int boxX = startX;
    int boxSize = 12;
    int boxY = y;  // 复选框左上角 Y（更靠下）

    // 绘制方框
    tft.drawRect(boxX, boxY, boxSize, boxSize, TEXT_SECONDARY);

    if (item.done) {
        tft.fillRect(boxX, boxY, boxSize, boxSize, ACCENT_BLUE);
        // 勾选标记
        tft.drawLine(boxX + 2, boxY + 6, boxX + 5, boxY + 9, TEXT_PRIMARY);
        tft.drawLine(boxX + 5, boxY + 9, boxX + 10, boxY + 3, TEXT_PRIMARY);
    }

    // 文本
    uint16_t textColor = item.done ? TEXT_SECONDARY : TEXT_PRIMARY;
    tft.setCursor(boxX + 16, y);
    tft.setTextColor(textColor, BG_CARD);

    String text = item.text;
    if (text.length() > 12) {
        text = text.substring(0, 10) + "...";
    }
    tft.print(text);
}

int TFTTodo::getMaxVisibleItems() {
    return (TODO_HEIGHT - 44) / 22;  // 适配 18pt 字体行高
}

void TFTTodo::scrollUp() {
    if (scrollOffset > 0) {
        scrollOffset--;
        lastCount = -1;
    }
}

void TFTTodo::scrollDown() {
    scrollOffset++;
    lastCount = -1;
}
