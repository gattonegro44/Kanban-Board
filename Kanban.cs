// Kanban.cs
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.Json;
using System.Text.Json.Serialization;

public enum Status
{
    [JsonPropertyName("To Do")]
    Todo,
    [JsonPropertyName("In Progress")]
    InProgress,
    [JsonPropertyName("Done")]
    Done
}

public enum Priority
{
    [JsonPropertyName("Низкий")]
    Low,
    [JsonPropertyName("Средний")]
    Medium,
    [JsonPropertyName("Высокий")]
    High
}

public record Task(
    int Id,
    string Title,
    string Description,
    Status Status,
    Priority Priority,
    string CreatedAt
);

public class KanbanBoard
{
    private List<Task> tasks = new();
    private int nextId = 1;

    public IReadOnlyList<Task> Tasks => tasks.AsReadOnly();

    public Task AddTask(string title, string desc, Priority priority)
    {
        var task = new Task(
            nextId,
            title,
            desc,
            Status.Todo,
            priority,
            DateTime.Now.ToString("o")
        );
        tasks.Add(task);
        nextId++;
        return task;
    }

    public Task? FindTask(int id) => tasks.FirstOrDefault(t => t.Id == id);

    public bool MoveTask(int id, Status newStatus)
    {
        var task = FindTask(id);
        if (task == null) return false;
        tasks.Remove(task);
        var updated = task with { Status = newStatus };
        tasks.Add(updated);
        return true;
    }

    public bool DeleteTask(int id) => tasks.RemoveAll(t => t.Id == id) > 0;

    public void SaveToFile(string filename)
    {
        var options = new JsonSerializerOptions { WriteIndented = true };
        string json = JsonSerializer.Serialize(tasks, options);
        File.WriteAllText(filename, json);
    }

    public void LoadFromFile(string filename)
    {
        if (!File.Exists(filename)) return;
        string json = File.ReadAllText(filename);
        var loaded = JsonSerializer.Deserialize<List<Task>>(json);
        if (loaded != null)
        {
            tasks = loaded;
            nextId = tasks.Any() ? tasks.Max(t => t.Id) + 1 : 1;
        }
    }
}

public static class Program
{
    private static string ReadString(string prompt)
    {
        Console.Write(prompt);
        return Console.ReadLine()?.Trim() ?? "";
    }

    private static int ReadInt(string prompt)
    {
        while (true)
        {
            Console.Write(prompt);
            if (int.TryParse(Console.ReadLine(), out int result))
                return result;
            Console.WriteLine("Введите число.");
        }
    }

    private static void PrintTask(Task task)
    {
        string emoji = task.Status switch
        {
            Status.Todo => "📌",
            Status.InProgress => "🔄",
            Status.Done => "✅",
            _ => "❓"
        };
        Console.WriteLine($"{emoji} #{task.Id} - {task.Title} ({task.Status})");
        Console.WriteLine($"   Описание: {task.Description}");
        Console.WriteLine($"   Приоритет: {task.Priority}, Создана: {task.CreatedAt}");
    }

    public static void Main()
    {
        var board = new KanbanBoard();
        try { board.LoadFromFile("kanban_data.json"); }
        catch { Console.WriteLine("Не удалось загрузить данные."); }

        while (true)
        {
            Console.WriteLine("\n===== КАНБАН-ДОСКА (C#) =====");
            Console.WriteLine("1. Показать все задачи");
            Console.WriteLine("2. Добавить задачу");
            Console.WriteLine("3. Переместить задачу (изменить статус)");
            Console.WriteLine("4. Удалить задачу");
            Console.WriteLine("5. Сохранить в файл");
            Console.WriteLine("6. Загрузить из файла");
            Console.WriteLine("0. Выход");
            string choice = ReadString("Выберите действие: ");

            switch (choice)
            {
                case "0": return;
                case "1":
                    if (!board.Tasks.Any()) Console.WriteLine("Задач нет.");
                    else foreach (var t in board.Tasks) PrintTask(t);
                    break;
                case "2":
                    string title = ReadString("Введите название: ");
                    if (string.IsNullOrWhiteSpace(title))
                    {
                        Console.WriteLine("Название не может быть пустым.");
                        continue;
                    }
                    string desc = ReadString("Введите описание: ");
                    Console.WriteLine("Выберите приоритет: 1-Низкий, 2-Средний, 3-Высокий");
                    string prio = ReadString("> ");
                    Priority priority = prio switch
                    {
                        "1" => Priority.Low,
                        "3" => Priority.High,
                        _ => Priority.Medium
                    };
                    var task = board.AddTask(title, desc, priority);
                    Console.WriteLine($"Задача добавлена с ID {task.Id}");
                    break;
                case "3":
                    int id = ReadInt("Введите ID задачи: ");
                    var found = board.FindTask(id);
                    if (found == null)
                    {
                        Console.WriteLine("Задача не найдена.");
                        continue;
                    }
                    Console.WriteLine($"Текущий статус: {found.Status}");
                    Console.WriteLine("Выберите новый статус: 1-To Do, 2-In Progress, 3-Done");
                    string newChoice = ReadString("> ");
                    Status newStatus = newChoice switch
                    {
                        "1" => Status.Todo,
                        "2" => Status.InProgress,
                        "3" => Status.Done,
                        _ => Status.Todo // fallback
                    };
                    if (board.MoveTask(id, newStatus))
                        Console.WriteLine("Статус обновлён.");
                    else
                        Console.WriteLine("Ошибка.");
                    break;
                case "4":
                    int delId = ReadInt("Введите ID задачи для удаления: ");
                    if (board.DeleteTask(delId)) Console.WriteLine("Задача удалена.");
                    else Console.WriteLine("Задача не найдена.");
                    break;
                case "5":
                    try { board.SaveToFile("kanban_data.json"); Console.WriteLine("Сохранено."); }
                    catch (Exception ex) { Console.WriteLine($"Ошибка: {ex.Message}"); }
                    break;
                case "6":
                    try { board.LoadFromFile("kanban_data.json"); Console.WriteLine("Загружено."); }
                    catch (Exception ex) { Console.WriteLine($"Ошибка: {ex.Message}"); }
                    break;
                default: Console.WriteLine("Неизвестная команда."); break;
            }
        }
    }
}
