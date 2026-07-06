📋 Kanban Board – Multi‑Language Implementation
Тестовый репозиторий с реализацией консольной канбан‑доски на 7 языках программирования.
Каждая версия поддерживает одинаковый набор действий: добавление, перемещение, удаление задач, просмотр доски, а также сохранение / загрузку состояния в файл.

🧩 Особенности
Интерактивное меню – выбор действий по номеру.

Задача содержит: id, название, описание, статус (To Do / In Progress / Done), приоритет (низкий / средний / высокий).

Автоинкремент id при создании.

Фильтрация задач по статусу и приоритету.

Цветной вывод (где поддерживается) для наглядности.

Сохранение состояния в файл kanban_data.json (или эквивалент) и загрузка из него.

Обработка ошибок ввода и файловых операций.

🗂 Структура репозитория
Файл	Язык	Описание
README.md	–	Этот файл
kanban.py	Python 3	Реализация на Python
kanban.rb	Ruby	Реализация на Ruby
kanban.go	Go	Реализация на Go
Kanban.java	Java 17+	Реализация на Java
Kanban.cs	C# 10 (.NET 6+)	Реализация на C#
kanban.js	Node.js	Реализация на JavaScript (ES2020)
kanban.cpp	C++17	Реализация на C++
🚀 Запуск
Python
bash
python3 kanban.py
Ruby
bash
ruby kanban.rb
Go
bash
go run kanban.go
Java
bash
javac Kanban.java && java Kanban
C#
bash
dotnet run --project .   # (или csc Kanban.cs && mono Kanban.exe)
JavaScript
bash
node kanban.js
C++
bash
g++ -std=c++17 -o kanban kanban.cpp && ./kanban
📸 Пример работы
text
===== КАНБАН-ДОСКА =====
1. Показать все задачи
2. Добавить задачу
3. Переместить задачу (изменить статус)
4. Удалить задачу
5. Сохранить в файл
6. Загрузить из файла
0. Выход
Выберите действие: 2
Введите название: Изучить Go
Введите описание: Прочитать книгу по Go
Выберите приоритет (1-низкий, 2-средний, 3-высокий): 2
Задача добавлена с ID 1

... (дальнейшие действия)
📦 Формат файла сохранения (JSON)
json
[
  {
    "id": 1,
    "title": "Изучить Go",
    "description": "Прочитать книгу по Go",
    "status": "To Do",
    "priority": "Средний"
  }
]
🧠 Продвинутые возможности в каждой версии
Язык	Фичи
Python	dataclass, Enum, type hints, контекстный менеджер, f-строки
Ruby	блоки, JSON модуль, символы, метод inject для генерации ID
Go	структуры с тегами, json.Unmarshal, bufio.Scanner, defer
Java	record (Java 17), Stream API, Enum, Serializable
C#	record struct, LINQ, System.Text.Json, switch expressions
JavaScript	классы, async/await, readline/promises, деструктуризация, try/catch
C++	std::variant для статуса, лямбды, std::filesystem, умные указатели
📄 Лицензия
Проект предоставлен для ознакомления и тестирования.
Распространяется по лицензии MIT.

