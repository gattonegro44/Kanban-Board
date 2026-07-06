// kanban.cpp
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <memory>
#include <variant>

using namespace std;

enum class Status { TODO, IN_PROGRESS, DONE };
enum class Priority { LOW, MEDIUM, HIGH };

string statusToString(Status s) {
    switch (s) {
        case Status::TODO: return "To Do";
        case Status::IN_PROGRESS: return "In Progress";
        case Status::DONE: return "Done";
        default: return "";
    }
}

Status stringToStatus(const string& str) {
    if (str == "To Do") return Status::TODO;
    if (str == "In Progress") return Status::IN_PROGRESS;
    if (str == "Done") return Status::DONE;
    return Status::TODO;
}

string priorityToString(Priority p) {
    switch (p) {
        case Priority::LOW: return "Низкий";
        case Priority::MEDIUM: return "Средний";
        case Priority::HIGH: return "Высокий";
        default: return "";
    }
}

Priority stringToPriority(const string& str) {
    if (str == "Низкий") return Priority::LOW;
    if (str == "Средний") return Priority::MEDIUM;
    if (str == "Высокий") return Priority::HIGH;
    return Priority::MEDIUM;
}

struct Task {
    int id;
    string title;
    string description;
    Status status;
    Priority priority;
    time_t createdAt;

    Task(int id, const string& title, const string& desc, Status status, Priority priority, time_t created)
        : id(id), title(title), description(desc), status(status), priority(priority), createdAt(created) {}

    string formatTime() const {
        char buf[20];
        tm* tm_info = localtime(&createdAt);
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_info);
        return string(buf);
    }
};

class KanbanBoard {
private:
    vector<Task> tasks;
    int nextId = 1;

public:
    Task& addTask(const string& title, const string& desc, Priority priority) {
        Task task(nextId, title, desc, Status::TODO, priority, time(nullptr));
        tasks.push_back(task);
        nextId++;
        return tasks.back();
    }

    Task* findTask(int id) {
        auto it = find_if(tasks.begin(), tasks.end(), [id](const Task& t) { return t.id == id; });
        return it != tasks.end() ? &(*it) : nullptr;
    }

    bool moveTask(int id, Status newStatus) {
        Task* task = findTask(id);
        if (!task) return false;
        task->status = newStatus;
        return true;
    }

    bool deleteTask(int id) {
        auto it = find_if(tasks.begin(), tasks.end(), [id](const Task& t) { return t.id == id; });
        if (it == tasks.end()) return false;
        tasks.erase(it);
        return true;
    }

    const vector<Task>& getTasks() const { return tasks; }

    void saveToFile(const string& filename = "kanban_data.txt") {
        ofstream out(filename);
        if (!out) return;
        for (const auto& t : tasks) {
            out << t.id << '|'
                << t.title << '|'
                << t.description << '|'
                << statusToString(t.status) << '|'
                << priorityToString(t.priority) << '|'
                << t.createdAt << '\n';
        }
    }

    void loadFromFile(const string& filename = "kanban_data.txt") {
        ifstream in(filename);
        if (!in) return;
        tasks.clear();
        string line;
        while (getline(in, line)) {
            stringstream ss(line);
            string idStr, title, desc, statusStr, priorityStr, timeStr;
            getline(ss, idStr, '|');
            getline(ss, title, '|');
            getline(ss, desc, '|');
            getline(ss, statusStr, '|');
            getline(ss, priorityStr, '|');
            getline(ss, timeStr, '|');
            int id = stoi(idStr);
            Status status = stringToStatus(statusStr);
            Priority priority = stringToPriority(priorityStr);
            time_t created = stoll(timeStr);
            tasks.emplace_back(id, title, desc, status, priority, created);
            if (id >= nextId) nextId = id + 1;
        }
    }
};

string readString(const string& prompt) {
    cout << prompt;
    string input;
    getline(cin, input);
    return input;
}

int readInt(const string& prompt) {
    while (true) {
        cout << prompt;
        string input;
        getline(cin, input);
        try {
            return stoi(input);
        } catch (...) {
            cout << "Введите число.\n";
        }
    }
}

void printTask(const Task& task) {
    string emoji;
    switch (task.status) {
        case Status::TODO: emoji = "📌"; break;
        case Status::IN_PROGRESS: emoji = "🔄"; break;
        case Status::DONE: emoji = "✅"; break;
    }
    cout << emoji << " #" << task.id << " - " << task.title << " (" << statusToString(task.status) << ")\n";
    cout << "   Описание: " << task.description << "\n";
    cout << "   Приоритет: " << priorityToString(task.priority) << ", Создана: " << task.formatTime() << "\n";
}

int main() {
    KanbanBoard board;
    board.loadFromFile();

    while (true) {
        cout << "\n===== КАНБАН-ДОСКА (C++) =====" << endl;
        cout << "1. Показать все задачи\n";
        cout << "2. Добавить задачу\n";
        cout << "3. Переместить задачу (изменить статус)\n";
        cout << "4. Удалить задачу\n";
        cout << "5. Сохранить в файл\n";
        cout << "6. Загрузить из файла\n";
        cout << "0. Выход\n";
        string choice = readString("Выберите действие: ");

        if (choice == "0") break;

        if (choice == "1") {
            if (board.getTasks().empty()) {
                cout << "Задач нет.\n";
            } else {
                for (const auto& t : board.getTasks()) {
                    printTask(t);
                }
            }
        } else if (choice == "2") {
            string title = readString("Введите название: ");
            if (title.empty()) {
                cout << "Название не может быть пустым.\n";
                continue;
            }
            string desc = readString("Введите описание: ");
            cout << "Выберите приоритет: 1-Низкий, 2-Средний, 3-Высокий\n";
            string prio = readString("> ");
            Priority priority;
            if (prio == "1") priority = Priority::LOW;
            else if (prio == "3") priority = Priority::HIGH;
            else priority = Priority::MEDIUM;
            Task& task = board.addTask(title, desc, priority);
            cout << "Задача добавлена с ID " << task.id << "\n";
        } else if (choice == "3") {
            int id = readInt("Введите ID задачи: ");
            Task* task = board.findTask(id);
            if (!task) {
                cout << "Задача не найдена.\n";
                continue;
            }
            cout << "Текущий статус: " << statusToString(task->status) << "\n";
            cout << "Выберите новый статус: 1-To Do, 2-In Progress, 3-Done\n";
            string newChoice = readString("> ");
            Status newStatus;
            if (newChoice == "1") newStatus = Status::TODO;
            else if (newChoice == "2") newStatus = Status::IN_PROGRESS;
            else if (newChoice == "3") newStatus = Status::DONE;
            else {
                cout << "Неверный выбор.\n";
                continue;
            }
            if (board.moveTask(id, newStatus)) {
                cout << "Статус обновлён.\n";
            } else {
                cout << "Ошибка.\n";
            }
        } else if (choice == "4") {
            int id = readInt("Введите ID задачи для удаления: ");
            if (board.deleteTask(id)) {
                cout << "Задача удалена.\n";
            } else {
                cout << "Задача не найдена.\n";
            }
        } else if (choice == "5") {
            board.saveToFile();
            cout << "Сохранено.\n";
        } else if (choice == "6") {
            board.loadFromFile();
            cout << "Загружено.\n";
        } else {
            cout << "Неизвестная команда.\n";
        }
    }
    return 0;
}
