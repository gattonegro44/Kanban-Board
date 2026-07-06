// kanban.go
package main

import (
	"bufio"
	"encoding/json"
	"fmt"
	"os"
	"strconv"
	"strings"
	"time"
)

type Status string

const (
	TODO       Status = "To Do"
	IN_PROGRESS Status = "In Progress"
	DONE       Status = "Done"
)

type Priority string

const (
	LOW    Priority = "Низкий"
	MEDIUM Priority = "Средний"
	HIGH   Priority = "Высокий"
)

type Task struct {
	ID          int       `json:"id"`
	Title       string    `json:"title"`
	Description string    `json:"description"`
	Status      Status    `json:"status"`
	Priority    Priority  `json:"priority"`
	CreatedAt   time.Time `json:"created_at"`
}

type KanbanBoard struct {
	tasks   []Task
	nextID  int
}

func NewKanbanBoard() *KanbanBoard {
	return &KanbanBoard{
		tasks:  []Task{},
		nextID: 1,
	}
}

func (b *KanbanBoard) AddTask(title, desc string, priority Priority) Task {
	task := Task{
		ID:          b.nextID,
		Title:       title,
		Description: desc,
		Status:      TODO,
		Priority:    priority,
		CreatedAt:   time.Now(),
	}
	b.tasks = append(b.tasks, task)
	b.nextID++
	return task
}

func (b *KanbanBoard) FindTask(id int) *Task {
	for i := range b.tasks {
		if b.tasks[i].ID == id {
			return &b.tasks[i]
		}
	}
	return nil
}

func (b *KanbanBoard) MoveTask(id int, newStatus Status) bool {
	task := b.FindTask(id)
	if task == nil {
		return false
	}
	task.Status = newStatus
	return true
}

func (b *KanbanBoard) DeleteTask(id int) bool {
	for i, t := range b.tasks {
		if t.ID == id {
			b.tasks = append(b.tasks[:i], b.tasks[i+1:]...)
			return true
		}
	}
	return false
}

func (b *KanbanBoard) SaveToFile(filename string) error {
	data, err := json.MarshalIndent(b.tasks, "", "  ")
	if err != nil {
		return err
	}
	return os.WriteFile(filename, data, 0644)
}

func (b *KanbanBoard) LoadFromFile(filename string) error {
	data, err := os.ReadFile(filename)
	if err != nil {
		if os.IsNotExist(err) {
			return nil
		}
		return err
	}
	var loaded []Task
	if err := json.Unmarshal(data, &loaded); err != nil {
		return err
	}
	b.tasks = loaded
	for _, t := range loaded {
		if t.ID >= b.nextID {
			b.nextID = t.ID + 1
		}
	}
	return nil
}

func printTask(task Task) {
	emoji := map[Status]string{TODO: "📌", IN_PROGRESS: "🔄", DONE: "✅"}
	fmt.Printf("%s #%d - %s (%s)\n", emoji[task.Status], task.ID, task.Title, task.Status)
	fmt.Printf("   Описание: %s\n", task.Description)
	fmt.Printf("   Приоритет: %s, Создана: %s\n", task.Priority, task.CreatedAt.Format("2006-01-02 15:04:05"))
}

func readString(prompt string) string {
	fmt.Print(prompt)
	reader := bufio.NewReader(os.Stdin)
	input, _ := reader.ReadString('\n')
	return strings.TrimSpace(input)
}

func readInt(prompt string) int {
	for {
		input := readString(prompt)
		if val, err := strconv.Atoi(input); err == nil {
			return val
		}
		fmt.Println("Введите число.")
	}
}

func main() {
	board := NewKanbanBoard()
	if err := board.LoadFromFile("kanban_data.json"); err != nil {
		fmt.Println("Ошибка загрузки:", err)
	}

	for {
		fmt.Println("\n===== КАНБАН-ДОСКА (Go) =====")
		fmt.Println("1. Показать все задачи")
		fmt.Println("2. Добавить задачу")
		fmt.Println("3. Переместить задачу (изменить статус)")
		fmt.Println("4. Удалить задачу")
		fmt.Println("5. Сохранить в файл")
		fmt.Println("6. Загрузить из файла")
		fmt.Println("0. Выход")
		choice := readString("Выберите действие: ")

		switch choice {
		case "0":
			return
		case "1":
			if len(board.tasks) == 0 {
				fmt.Println("Задач нет.")
			} else {
				for _, t := range board.tasks {
					printTask(t)
				}
			}
		case "2":
			title := readString("Введите название: ")
			if title == "" {
				fmt.Println("Название не может быть пустым.")
				continue
			}
			desc := readString("Введите описание: ")
			fmt.Println("Выберите приоритет: 1-Низкий, 2-Средний, 3-Высокий")
			prio := readString("> ")
			var priority Priority
			switch prio {
			case "1":
				priority = LOW
			case "3":
				priority = HIGH
			default:
				priority = MEDIUM
			}
			task := board.AddTask(title, desc, priority)
			fmt.Printf("Задача добавлена с ID %d\n", task.ID)
		case "3":
			id := readInt("Введите ID задачи: ")
			task := board.FindTask(id)
			if task == nil {
				fmt.Println("Задача не найдена.")
				continue
			}
			fmt.Printf("Текущий статус: %s\n", task.Status)
			fmt.Println("Выберите новый статус: 1-To Do, 2-In Progress, 3-Done")
			newChoice := readString("> ")
			var newStatus Status
			switch newChoice {
			case "1":
				newStatus = TODO
			case "2":
				newStatus = IN_PROGRESS
			case "3":
				newStatus = DONE
			default:
				fmt.Println("Неверный выбор.")
				continue
			}
			if board.MoveTask(id, newStatus) {
				fmt.Println("Статус обновлён.")
			} else {
				fmt.Println("Ошибка.")
			}
		case "4":
			id := readInt("Введите ID задачи для удаления: ")
			if board.DeleteTask(id) {
				fmt.Println("Задача удалена.")
			} else {
				fmt.Println("Задача не найдена.")
			}
		case "5":
			if err := board.SaveToFile("kanban_data.json"); err != nil {
				fmt.Println("Ошибка сохранения:", err)
			} else {
				fmt.Println("Сохранено.")
			}
		case "6":
			if err := board.LoadFromFile("kanban_data.json"); err != nil {
				fmt.Println("Ошибка загрузки:", err)
			} else {
				fmt.Println("Загружено.")
			}
		default:
			fmt.Println("Неизвестная команда.")
		}
	}
}
