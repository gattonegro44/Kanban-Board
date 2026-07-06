# kanban.rb
require 'json'
require 'date'

class Status
  TODO = "To Do"
  IN_PROGRESS = "In Progress"
  DONE = "Done"
end

class Priority
  LOW = "Низкий"
  MEDIUM = "Средний"
  HIGH = "Высокий"
end

class Task
  attr_accessor :id, :title, :description, :status, :priority, :created_at

  def initialize(id, title, description, status = Status::TODO, priority = Priority::MEDIUM, created_at = DateTime.now.iso8601)
    @id = id
    @title = title
    @description = description
    @status = status
    @priority = priority
    @created_at = created_at
  end

  def to_h
    {
      id: @id,
      title: @title,
      description: @description,
      status: @status,
      priority: @priority,
      created_at: @created_at
    }
  end

  def self.from_h(hash)
    Task.new(hash[:id], hash[:title], hash[:description], hash[:status], hash[:priority], hash[:created_at])
  end
end

class KanbanBoard
  attr_reader :tasks

  def initialize
    @tasks = []
    @next_id = 1
  end

  def add_task(title, description, priority = Priority::MEDIUM)
    task = Task.new(@next_id, title, description, Status::TODO, priority)
    @tasks << task
    @next_id += 1
    task
  end

  def find_task(id)
    @tasks.find { |t| t.id == id }
  end

  def move_task(id, new_status)
    task = find_task(id)
    return false unless task
    task.status = new_status
    true
  end

  def delete_task(id)
    task = find_task(id)
    return false unless task
    @tasks.delete(task)
    true
  end

  def filter_by_status(status)
    @tasks.select { |t| t.status == status }
  end

  def filter_by_priority(priority)
    @tasks.select { |t| t.priority == priority }
  end

  def save_to_file(filename = "kanban_data.json")
    File.write(filename, JSON.pretty_generate(@tasks.map(&:to_h)))
  end

  def load_from_file(filename = "kanban_data.json")
    return unless File.exist?(filename)
    data = JSON.parse(File.read(filename), symbolize_names: true)
    @tasks.clear
    data.each do |item|
      task = Task.from_h(item)
      @tasks << task
      @next_id = task.id + 1 if task.id >= @next_id
    end
  rescue JSON::ParserError
    puts "Ошибка чтения файла."
  end
end

def display_task(task)
  emoji = { "To Do" => "📌", "In Progress" => "🔄", "Done" => "✅" }
  puts "#{emoji[task.status]} ##{task.id} - #{task.title} (#{task.status})"
  puts "   Описание: #{task.description}"
  puts "   Приоритет: #{task.priority}, Создана: #{task.created_at}"
end

def main
  board = KanbanBoard.new
  board.load_from_file

  loop do
    puts "\n===== КАНБАН-ДОСКА (Ruby) ====="
    puts "1. Показать все задачи"
    puts "2. Добавить задачу"
    puts "3. Переместить задачу (изменить статус)"
    puts "4. Удалить задачу"
    puts "5. Сохранить в файл"
    puts "6. Загрузить из файла"
    puts "0. Выход"
    print "Выберите действие: "
    choice = gets.chomp

    case choice
    when "0"
      break
    when "1"
      if board.tasks.empty?
        puts "Задач нет."
      else
        board.tasks.each { |t| display_task(t) }
      end
    when "2"
      print "Введите название: "
      title = gets.chomp
      next if title.empty?
      print "Введите описание: "
      desc = gets.chomp
      puts "Выберите приоритет: 1-Низкий, 2-Средний, 3-Высокий"
      prio = gets.chomp
      priority = case prio
                 when "1" then Priority::LOW
                 when "3" then Priority::HIGH
                 else Priority::MEDIUM
                 end
      task = board.add_task(title, desc, priority)
      puts "Задача добавлена с ID #{task.id}"
    when "3"
      print "Введите ID задачи: "
      id = gets.chomp.to_i
      task = board.find_task(id)
      unless task
        puts "Задача не найдена."
        next
      end
      puts "Текущий статус: #{task.status}"
      puts "Выберите новый статус: 1-To Do, 2-In Progress, 3-Done"
      new_choice = gets.chomp
      new_status = case new_choice
                   when "1" then Status::TODO
                   when "2" then Status::IN_PROGRESS
                   when "3" then Status::DONE
                   else nil
                   end
      unless new_status
        puts "Неверный выбор."
        next
      end
      if board.move_task(id, new_status)
        puts "Статус обновлён."
      else
        puts "Ошибка."
      end
    when "4"
      print "Введите ID задачи для удаления: "
      id = gets.chomp.to_i
      if board.delete_task(id)
        puts "Задача удалена."
      else
        puts "Задача не найдена."
      end
    when "5"
      board.save_to_file
      puts "Сохранено."
    when "6"
      board.load_from_file
      puts "Загружено."
    else
      puts "Неизвестная команда."
    end
  end
end

main if __FILE__ == $0
