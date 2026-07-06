// Kanban.java
import java.io.*;
import java.nio.file.*;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.*;
import java.util.stream.Collectors;

enum Status {
    TODO("To Do"),
    IN_PROGRESS("In Progress"),
    DONE("Done");

    private final String display;
    Status(String display) { this.display = display; }
    public String getDisplay() { return display; }

    public static Status fromDisplay(String display) {
        for (Status s : values()) {
            if (s.display.equals(display)) return s;
        }
        throw new IllegalArgumentException("Неизвестный статус");
    }
}

enum Priority {
    LOW("Низкий"),
    MEDIUM("Средний"),
    HIGH("Высокий");

    private final String display;
    Priority(String display) { this.display = display; }
    public String getDisplay() { return display; }

    public static Priority fromDisplay(String display) {
        for (Priority p : values()) {
            if (p.display.equals(display)) return p;
        }
        return MEDIUM;
    }
}

record Task(int id, String title, String description, Status status, Priority priority, String createdAt) implements Serializable {
    public Task {
        Objects.requireNonNull(title);
        Objects.requireNonNull(description);
        Objects.requireNonNull(status);
        Objects.requireNonNull(priority);
    }
}

class KanbanBoard implements Serializable {
    private static final long serialVersionUID = 1L;
    private List<Task> tasks = new ArrayList<>();
    private int nextId = 1;

    public Task addTask(String title, String desc, Priority priority) {
        Task task = new Task(nextId, title, desc, Status.TODO, priority,
                LocalDateTime.now().format(DateTimeFormatter.ISO_LOCAL_DATE_TIME));
        tasks.add(task);
        nextId++;
        return task;
    }

    public Optional<Task> findTask(int id) {
        return tasks.stream().filter(t -> t.id() == id).findFirst();
    }

    public boolean moveTask(int id, Status newStatus) {
        return findTask(id).map(t -> {
            tasks.remove(t);
            Task updated = new Task(t.id(), t.title(), t.description(), newStatus, t.priority(), t.createdAt());
            tasks.add(updated);
            return true;
        }).orElse(false);
    }

    public boolean deleteTask(int id) {
        return tasks.removeIf(t -> t.id() == id);
    }

    public List<Task> filterByStatus(Status status) {
        return tasks.stream().filter(t -> t.status() == status).collect(Collectors.toList());
    }

    public void saveToFile(String filename) throws IOException {
        try (ObjectOutputStream oos = new ObjectOutputStream(Files.newOutputStream(Paths.get(filename)))) {
            oos.writeObject(this);
        }
    }

    public void loadFromFile(String filename) throws IOException, ClassNotFoundException {
        try (ObjectInputStream ois = new ObjectInputStream(Files.newInputStream(Paths.get(filename)))) {
            KanbanBoard loaded = (KanbanBoard) ois.readObject();
            this.tasks = loaded.tasks;
            this.nextId = loaded.nextId;
        }
    }

    public List<Task> getTasks() { return Collections.unmodifiableList(tasks); }
}

public class Kanban {
    private static final Scanner scanner = new Scanner(System.in);

    private static String readString(String prompt) {
        System.out.print(prompt);
        return scanner.nextLine().trim();
    }

    private static int readInt(String prompt) {
        while (true) {
            try {
                System.out.print(prompt);
                return Integer.parseInt(scanner.nextLine().trim());
            } catch (NumberFormatException e) {
                System.out.println("Введите число.");
            }
        }
    }

    private static void printTask(Task task) {
        String emoji = switch (task.status()) {
            case TODO -> "📌";
            case IN_PROGRESS -> "🔄";
            case DONE -> "✅";
        };
        System.out.printf("%s #%d - %s (%s)%n", emoji, task.id(), task.title(), task.status().getDisplay());
        System.out.printf("   Описание: %s%n", task.description());
        System.out.printf("   Приоритет: %s, Создана: %s%n", task.priority().getDisplay(), task.createdAt());
    }

    public static void main(String[] args) {
        KanbanBoard board = new KanbanBoard();
        try {
            board.loadFromFile("kanban_data.ser");
        } catch (IOException | ClassNotFoundException e) {
            System.out.println("Не удалось загрузить данные, начинаем с пустой доски.");
        }

        while (true) {
            System.out.println("\n===== КАНБАН-ДОСКА (Java) =====");
            System.out.println("1. Показать все задачи");
            System.out.println("2. Добавить задачу");
            System.out.println("3. Переместить задачу (изменить статус)");
            System.out.println("4. Удалить задачу");
            System.out.println("5. Сохранить в файл");
            System.out.println("6. Загрузить из файла");
            System.out.println("0. Выход");
            String choice = readString("Выберите действие: ");

            switch (choice) {
                case "0" -> { return; }
                case "1" -> {
                    if (board.getTasks().isEmpty()) {
                        System.out.println("Задач нет.");
                    } else {
                        board.getTasks().forEach(Kanban::printTask);
                    }
                }
                case "2" -> {
                    String title = readString("Введите название: ");
                    if (title.isBlank()) {
                        System.out.println("Название не может быть пустым.");
                        continue;
                    }
                    String desc = readString("Введите описание: ");
                    System.out.println("Выберите приоритет: 1-Низкий, 2-Средний, 3-Высокий");
                    String prio = readString("> ");
                    Priority priority = switch (prio) {
                        case "1" -> Priority.LOW;
                        case "3" -> Priority.HIGH;
                        default -> Priority.MEDIUM;
                    };
                    Task task = board.addTask(title, desc, priority);
                    System.out.printf("Задача добавлена с ID %d%n", task.id());
                }
                case "3" -> {
                    int id = readInt("Введите ID задачи: ");
                    var opt = board.findTask(id);
                    if (opt.isEmpty()) {
                        System.out.println("Задача не найдена.");
                        continue;
                    }
                    Task task = opt.get();
                    System.out.printf("Текущий статус: %s%n", task.status().getDisplay());
                    System.out.println("Выберите новый статус: 1-To Do, 2-In Progress, 3-Done");
                    String newChoice = readString("> ");
                    Status newStatus = switch (newChoice) {
                        case "1" -> Status.TODO;
                        case "2" -> Status.IN_PROGRESS;
                        case "3" -> Status.DONE;
                        default -> null;
                    };
                    if (newStatus == null) {
                        System.out.println("Неверный выбор.");
                        continue;
                    }
                    if (board.moveTask(id, newStatus)) {
                        System.out.println("Статус обновлён.");
                    } else {
                        System.out.println("Ошибка.");
                    }
                }
                case "4" -> {
                    int id = readInt("Введите ID задачи для удаления: ");
                    if (board.deleteTask(id)) {
                        System.out.println("Задача удалена.");
                    } else {
                        System.out.println("Задача не найдена.");
                    }
                }
                case "5" -> {
                    try {
                        board.saveToFile("kanban_data.ser");
                        System.out.println("Сохранено.");
                    } catch (IOException e) {
                        System.out.println("Ошибка сохранения: " + e.getMessage());
                    }
                }
                case "6" -> {
                    try {
                        board.loadFromFile("kanban_data.ser");
                        System.out.println("Загружено.");
                    } catch (IOException | ClassNotFoundException e) {
                        System.out.println("Ошибка загрузки: " + e.getMessage());
                    }
                }
                default -> System.out.println("Неизвестная команда.");
            }
        }
    }
}
