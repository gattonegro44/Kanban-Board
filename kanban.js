// kanban.js
const fs = require('fs').promises;
const readline = require('readline');

const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout
});

const question = (prompt) => new Promise(resolve => rl.question(prompt, resolve));

class Task {
    constructor(id, title, description, status = 'To Do', priority = 'Средний', createdAt = new Date().toISOString()) {
        this.id = id;
        this.title = title;
        this.description = description;
        this.status = status;
        this.priority = priority;
        this.createdAt = createdAt;
    }
}

class KanbanBoard {
    constructor() {
        this.tasks = [];
        this.nextId = 1;
    }

    addTask(title, description, priority) {
        const task = new Task(this.nextId, title, description, 'To Do', priority);
        this.tasks.push(task);
        this.nextId++;
        return task;
    }

    findTask(id) {
        return this.tasks.find(t => t.id === id);
    }

    moveTask(id, newStatus) {
        const task = this.findTask(id);
        if (!task) return false;
        task.status = newStatus;
        return true;
    }

    deleteTask(id) {
        const index = this.tasks.findIndex(t => t.id === id);
        if (index === -1) return false;
        this.tasks.splice(index, 1);
        return true;
    }

    async saveToFile(filename = 'kanban_data.json') {
        await fs.writeFile(filename, JSON.stringify(this.tasks, null, 2));
    }

    async loadFromFile(filename = 'kanban_data.json') {
        try {
            const data = await fs.readFile(filename, 'utf8');
            const loaded = JSON.parse(data);
            this.tasks = loaded.map(t => Object.assign(new Task(0), t));
            this.nextId = this.tasks.reduce((max, t) => Math.max(max, t.id), 0) + 1;
        } catch (err) {
            if (err.code !== 'ENOENT') throw err;
        }
    }
}

function printTask(task) {
    const emoji = { 'To Do': '📌', 'In Progress': '🔄', 'Done': '✅' };
    console.log(`${emoji[task.status] || '❓'} #${task.id} - ${task.title} (${task.status})`);
    console.log(`   Описание: ${task.description}`);
    console.log(`   Приоритет: ${task.priority}, Создана: ${task.createdAt}`);
}

async function main() {
    const board = new KanbanBoard();
    await board.loadFromFile();

    while (true) {
        console.log('\n===== КАНБАН-ДОСКА (JavaScript) =====');
        console.log('1. Показать все задачи');
        console.log('2. Добавить задачу');
        console.log('3. Переместить задачу (изменить статус)');
        console.log('4. Удалить задачу');
        console.log('5. Сохранить в файл');
        console.log('6. Загрузить из файла');
        console.log('0. Выход');
        const choice = await question('Выберите действие: ');

        if (choice === '0') break;

        switch (choice) {
            case '1':
                if (board.tasks.length === 0) {
                    console.log('Задач нет.');
                } else {
                    board.tasks.forEach(printTask);
                }
                break;
            case '2': {
                const title = await question('Введите название: ');
                if (!title.trim()) {
                    console.log('Название не может быть пустым.');
                    continue;
                }
                const desc = await question('Введите описание: ');
                console.log('Выберите приоритет: 1-Низкий, 2-Средний, 3-Высокий');
                const prio = await question('> ');
                let priority;
                if (prio === '1') priority = 'Низкий';
                else if (prio === '3') priority = 'Высокий';
                else priority = 'Средний';
                const task = board.addTask(title, desc, priority);
                console.log(`Задача добавлена с ID ${task.id}`);
                break;
            }
            case '3': {
                const id = parseInt(await question('Введите ID задачи: '));
                const task = board.findTask(id);
                if (!task) {
                    console.log('Задача не найдена.');
                    continue;
                }
                console.log(`Текущий статус: ${task.status}`);
                console.log('Выберите новый статус: 1-To Do, 2-In Progress, 3-Done');
                const newChoice = await question('> ');
                let newStatus;
                if (newChoice === '1') newStatus = 'To Do';
                else if (newChoice === '2') newStatus = 'In Progress';
                else if (newChoice === '3') newStatus = 'Done';
                else {
                    console.log('Неверный выбор.');
                    continue;
                }
                if (board.moveTask(id, newStatus)) {
                    console.log('Статус обновлён.');
                } else {
                    console.log('Ошибка.');
                }
                break;
            }
            case '4': {
                const id = parseInt(await question('Введите ID задачи для удаления: '));
                if (board.deleteTask(id)) {
                    console.log('Задача удалена.');
                } else {
                    console.log('Задача не найдена.');
                }
                break;
            }
            case '5':
                try {
                    await board.saveToFile();
                    console.log('Сохранено.');
                } catch (err) {
                    console.log('Ошибка сохранения:', err.message);
                }
                break;
            case '6':
                try {
                    await board.loadFromFile();
                    console.log('Загружено.');
                } catch (err) {
                    console.log('Ошибка загрузки:', err.message);
                }
                break;
            default:
                console.log('Неизвестная команда.');
        }
    }
    rl.close();
}

main().catch(console.error);
