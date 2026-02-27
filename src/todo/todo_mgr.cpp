#include "todo_mgr.h"



TodoManager::TodoManager() {
    _nextId = 1;
}

void TodoManager::begin(WebServer* server) {
    _server = server;
    _server->on("/", std::bind(&TodoManager::handleRoot, this));
    _server->on("/add", std::bind(&TodoManager::handleAdd, this));
    _server->on("/toggle", std::bind(&TodoManager::handleToggle, this));
    _server->on("/delete", std::bind(&TodoManager::handleDelete, this));
    _server->begin();

    // 加载已保存的待办事项
    loadTodos();
}

void TodoManager::handleRequests() {
    _server->handleClient();
}

// 页面显示 TODO 列表
void TodoManager::handleRoot() {
    String html = "<!DOCTYPE html><html><head>";
    html += "<meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "<title>ESP32 TODO</title>";
    html += "<style>";
    html += "* { margin: 0; padding: 0; box-sizing: border-box; }";
    html += "body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; padding: 20px; background: #f5f5f5; }";
    html += ".container { max-width: 600px; margin: 0 auto; background: white; border-radius: 12px; padding: 20px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }";
    html += "h1 { color: #007DFF; margin-bottom: 20px; font-size: 24px; text-align: center; }";
    html += "ul { list-style: none; margin-bottom: 20px; }";
    html += "li { display: flex; align-items: center; justify-content: space-between; padding: 12px 0; border-bottom: 1px solid #eee; }";
    html += "li:last-child { border-bottom: none; }";
    html += ".todo-text { flex: 1; font-size: 16px; color: #333; }";
    html += "li.completed .todo-text { text-decoration: line-through; color: #999; }";
    html += ".btn { padding: 6px 12px; margin-left: 8px; border: none; border-radius: 6px; cursor: pointer; font-size: 14px; text-decoration: none; }";
    html += ".btn-toggle { background: #007DFF; color: white; }";
    html += ".btn-delete { background: #D32F2F; color: white; }";
    html += "form { display: flex; gap: 10px; }";
    html += "input[type='text'] { flex: 1; padding: 12px; border: 2px solid #ddd; border-radius: 8px; font-size: 16px; }";
    html += "input[type='text']:focus { outline: none; border-color: #007DFF; }";
    html += "input[type='submit'] { padding: 12px 24px; background: #007DFF; color: white; border: none; border-radius: 8px; font-size: 16px; cursor: pointer; font-weight: bold; }";
    html += "input[type='submit']:active { background: #0056b3; }";
    html += "@media (max-width: 480px) { body { padding: 10px; } .container { padding: 15px; } h1 { font-size: 20px; } }";
    html += "</style></head><body>";
    html += "<div class='container'>";
    html += "<h1>📝 ESP32 待办事项</h1>";
    html += "<ul>";
    for (auto &item : _todos) {
        html += "<li class='" + String(item.done ? "completed" : "") + "'>";
        html += "<span class='todo-text'>" + item.text + "</span>";
        html += "<div>";
        html += "<a href='/toggle?id=" + String(item.id) + "' class='btn btn-toggle'>切换</a>";
        html += "<a href='/delete?id=" + String(item.id) + "' class='btn btn-delete'>删除</a>";
        html += "</div>";
        html += "</li>";
    }
    html += "</ul>";
    html += "<form action='/add' method='get'>";
    html += "<input type='text' name='text' placeholder='输入新任务...' autocomplete='off'>";
    html += "<input type='submit' value='添加'>";
    html += "</form>";
    html += "</div></body></html>";
    _server->send(200, "text/html", html);
}

// 添加 TODO
void TodoManager::handleAdd() {
    if (_server->hasArg("text") && _server->arg("text") != "") {
        TodoItem item;
        item.text = _server->arg("text");
        item.done = false;
        item.id = _nextId++;
        _todos.push_back(item);
        _hasUpdates = true;  // 标记需要更新显示
        saveTodos();  // 保存到 NV 存储
    }
    _server->sendHeader("Location", "/");
    _server->send(302, "text/plain", ""); // 重定向回主页
}

// 切换完成状态
void TodoManager::handleToggle() {
    if (_server->hasArg("id")) {
        int id = _server->arg("id").toInt();
        for (auto &item : _todos) {
            if (item.id == id) {
                item.done = !item.done;
                _hasUpdates = true;  // 标记需要更新显示
                saveTodos();  // 保存到 NV 存储
                break;
            }
        }
    }
    _server->sendHeader("Location", "/");
    _server->send(302, "text/plain", "");
}

// 删除 TODO
void TodoManager::handleDelete() {
    if (_server->hasArg("id")) {
        int id = _server->arg("id").toInt();
        for (size_t i = 0; i < _todos.size(); i++) {
            if (_todos[i].id == id) {
                _todos.erase(_todos.begin() + i);
                _hasUpdates = true;  // 标记需要更新显示
                saveTodos();  // 保存到 NV 存储
                break;
            }
        }
    }
    _server->sendHeader("Location", "/");
    _server->send(302, "text/plain", "");
}

// ====== 存储相关 ======

// 保存待办到 NV 存储
void TodoManager::saveTodos() {
    _prefs.begin("todos", false);
    _prefs.putInt("count", _todos.size());
    _prefs.putInt("nextId", _nextId);

    for (size_t i = 0; i < _todos.size(); i++) {
        String prefix = "todo_" + String(i);
        _prefs.putString((prefix + "_text").c_str(), _todos[i].text);
        _prefs.putInt((prefix + "_id").c_str(), _todos[i].id);
        _prefs.putBool((prefix + "_done").c_str(), _todos[i].done);
    }

    _prefs.end();
    Serial.println("Todos saved to NV storage");
}

// 从 NV 存储加载待办
void TodoManager::loadTodos() {
    _prefs.begin("todos", true);
    int count = _prefs.getInt("count", 0);
    _nextId = _prefs.getInt("nextId", 1);

    _todos.clear();
    for (int i = 0; i < count; i++) {
        TodoItem item;
        String prefix = "todo_" + String(i);
        item.text = _prefs.getString((prefix + "_text").c_str(), "");
        item.id = _prefs.getInt((prefix + "_id").c_str(), i + 1);
        item.done = _prefs.getBool((prefix + "_done").c_str(), false);

        if (item.text.length() > 0) {
            _todos.push_back(item);
        }
    }

    _prefs.end();
    Serial.printf("Loaded %d todos from NV storage\n", _todos.size());
}

// 清空存储
void TodoManager::clearStorage() {
    _prefs.begin("todos", false);
    _prefs.clear();
    _prefs.end();
    Serial.println("Todo storage cleared");
}
