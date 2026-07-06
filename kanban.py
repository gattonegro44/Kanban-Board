// kanban.py
import json
import sys
from dataclasses import dataclass, asdict
from enum import Enum, auto
from typing import List, Optional
from datetime import datetime

class Status(Enum):
    TODO = "To Do"
    IN_PROGRESS = "In Progress"
    DONE = "Done"

class Priority(Enum):
    LOW = "Низкий"
    MEDIUM = "Средний"
    HIGH = "Высокий"

@dataclass
class Task:
    id: int
    title: str
    description: str
    status: Status
    priority: Priority
    created_at: str

class KanbanBoard:
    def __init__(self):
        self.tasks: List[Task] = []
        self.next_id = 1

    def add_task(self, title: str, desc: str, priority: Priority) -> Task:
        task = Task(
            id=self.next_id,
            title=title,
            description=desc,
            status=Status.TODO,
            priority=priority,
            created_at=datetime.now().isoformat()
        )
        self.tasks.append(task)
        self.next_id += 1
        return task

    def get_task(self, task_id: int) -> Optional[Task]:
        return next((t for t in self.tasks if t.id == task_id), None)

    def move_task(self, task_id: int, new_status: Status) -> bool:
        task = self.get_task(task_id)
        if task:
            task.status = new_status
            return True
        return False

    def delete_task(self, task_id: int) -> bool:
        task = self.get_task(task_id)
        if task:
            self.tasks.remove(task)
            return True
        return False

    def filter_by_status(self, status: Status) -> List[Task]:
        return [t for t in self.tasks if t.status == status]

    def filter_by_priority(self, priority: Priority) -> List[Task]:
        return [t for t in self.tasks if t.priority == priority]

    def save_to_file(self, filename: str = "kanban_data.json") -> None:
        with open(filename, "w", encoding="utf-8") as f:
            json.dump([asdict(t) for t in self.tasks], f, ensure_ascii=False, indent=2)

    def load_from_file(self, filename: str = "kanban_data.json") -> None:
        try:
            with open(filename, "r", encoding="utf-8") as f:
                data = json.load(f)
                self.tasks.clear()
                for item in data:
                    task = Task(
                        id=item["id"],
                        title=item["title"],
                        description=item["description"],
                        status=Status(item["status"]),
                        priority=Priority(item["priority"]),
                        created_at=item["created_at"]
                    )
                    self.tasks.append(task)
                    if task.id >= self.next_id:
                        self.next_id = task.id + 1
        except FileNotFoundError:
            print("Файл не найден, создана пустая доска.")

def print_task(task: Task, color: bool = True) -> None:
    status_emoji = {"To Do": "📌", "In Progress": "🔄", "Done": "✅"}
    print(f"{status_emoji[task.status.value]} #{task.id} - {task.title} ({task.status.value})")
    print(f"   Описание: {task.description}")
    print(f"   Приоритет: {task.priority.value}, Создана: {task.created_at}")

def main():
    board = KanbanBoard()
    board.load_from_file()

    while True:
        print("\n===== КАНБАН-ДОСКА (Python) =====")
        print("1. Показать все задачи")
        print("2. Добавить задачу")
        print("3. Переместить задачу (изменить статус)")
        print("4. Удалить задачу")
        print("5. Сохранить в файл")
        print("6. Загрузить из файла")
        print("0. Выход")
        choice = input("Выберите действие: ").strip()

        if choice == "0":
            break
        elif choice == "1":
            if not board.tasks:
                print("Задач нет.")
                continue
            for t in board.tasks:
                print_task(t)
        elif choice == "2":
            title = input("Введите название: ").strip()
            if not title:
                print("Название не может быть пустым.")
                continue
            desc = input("Введите описание: ").strip()
            print("Выберите приоритет: 1-Низкий, 2-Средний, 3-Высокий")
            prio_choice = input("> ").strip()
            if prio_choice == "1":
                priority = Priority.LOW
            elif prio_choice == "2":
                priority = Priority.MEDIUM
            elif prio_choice == "3":
                priority = Priority.HIGH
            else:
                priority = Priority.MEDIUM
            task = board.add_task(title, desc, priority)
            print(f"Задача добавлена с ID {task.id}")
        elif choice == "3":
            try:
                task_id = int(input("Введите ID задачи: ").strip())
            except ValueError:
                print("Некорректный ID.")
                continue
            task = board.get_task(task_id)
            if not task:
                print("Задача не найдена.")
                continue
            print(f"Текущий статус: {task.status.value}")
            print("Выберите новый статус: 1-To Do, 2-In Progress, 3-Done")
            new_choice = input("> ").strip()
            if new_choice == "1":
                new_status = Status.TODO
            elif new_choice == "2":
                new_status = Status.IN_PROGRESS
            elif new_choice == "3":
                new_status = Status.DONE
            else:
                print("Неверный выбор.")
                continue
            if board.move_task(task_id, new_status):
                print("Статус обновлён.")
            else:
                print("Ошибка.")
        elif choice == "4":
            try:
                task_id = int(input("Введите ID задачи для удаления: ").strip())
            except ValueError:
                print("Некорректный ID.")
                continue
            if board.delete_task(task_id):
                print("Задача удалена.")
            else:
                print("Задача не найдена.")
        elif choice == "5":
            board.save_to_file()
            print("Сохранено.")
        elif choice == "6":
            board.load_from_file()
            print("Загружено.")
        else:
            print("Неизвестная команда.")

if __name__ == "__main__":
    main()
