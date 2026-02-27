#pragma once
#include <Arduino.h>
#include <vector>
#include <WebServer.h>
#include <Preferences.h>

struct TodoItem {
    String text;
    bool done;
    int id; // 唯一标识
};

class TodoManager {
public:
    TodoManager();
    void begin(WebServer* server); // 绑定 WebServer
    void handleRequests();          // 在 loop 中调用
    // ===== 新增接口 =====
    inline int size() const { return _todos.size(); }
    inline const TodoItem& getItem(int index) const { return _todos.at(index); }
    inline bool hasUpdates() const { return _hasUpdates; }
    inline void clearUpdates() { _hasUpdates = false; }
private:
    WebServer* _server;
    std::vector<TodoItem> _todos;
    int _nextId;
    bool _hasUpdates = false;  // 标记是否有待显示的更新
    Preferences _prefs;       // NV存储

    void handleRoot();
    void handleAdd();
    void handleToggle();
    void handleDelete();

    // ===== 存储相关 =====
    void saveTodos();     // 保存待办到 NV 存储
    void loadTodos();     // 从 NV 存储加载待办
    void clearStorage();  // 清空存储
};
