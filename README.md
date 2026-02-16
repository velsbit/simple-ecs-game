# Simple ECS Game

Простая реализация Entity-Component System (ECS) для 2D игр на C.

## Архитектура

Проект использует классический ECS подход с плоскими массивами компонентов для максимальной производительности:

- **Entity** - 32-битный идентификатор (до 256 сущностей)
- **Component** - структуры данных (Position, Velocity, Sprite, Stats и др.)
- **System** - игровая логика в главном цикле

## Компоненты

- `POSITION` - текущая и предыдущая позиция
- `VELOCITY` - текущая и целевая скорость
- `SPRITE` - размеры и текстура
- `STATS` - характеристики (скорость, здоровье, сила прыжка)
- `COLLIDER` - коллизии с тайлами
- `PLAYER_CONTROL` - управление игроком
- `AI_CONTROL` - ИИ поведение (патруль, преследование, случайное движение)

## Особенности реализации

- Компактное хранение компонентов в массивах (SOA)
- Битмаски для быстрой проверки наличия компонентов
- Поддержка коллизий с тайловой картой 20x15
- Камера для отслеживания игрока
- Гибкая система управления (игрок/ИИ)

## Требования

- Компилятор с поддержкой C17
- CMake 3.10 или выше
- Git

## Сборка и запуск

### Linux/Mac
```bash
# Клонирование репозитория
git clone https://github.com/velsbit/simple-ecs-game.git
cd simple-ecs-game

# Сборка проекта
mkdir build && cd build

# Debug сборка
cmake ..
# или Release сборка
cmake .. -DCMAKE_BUILD_TYPE=Release

# Компиляция
cmake --build .

# Запуск
./ECSCore
```

### Windows

```bash
# Клонирование репозитория
git clone https://github.com/velsbit/simple-ecs-game.git
cd simple-ecs-game

# Сборка проекта
mkdir build && cd build

# Генерация проекта для Visual Studio
cmake ..

# Компиляция (замените "Debug" на "Release" для release сборки)
cmake --build . --config Debug

# Запуск
.\Debug\ECSCore.exe
```

## Управление
- `W` - Прыжок
- `A` - Движение влево
- `S` - Движение вправо