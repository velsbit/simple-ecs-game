# Simple ECS Game

Легковесный 2D-движок на C11, реализующий паттерн Entity-Component System. Архитектура базируется на плоских массивах (SoA) и битмасках для работы с сущностями (до 256 ID). Включает системы физики, коллизий с тайлами, камеру и контроллер для игрока.

## Установка
Этот проект использует Git-сабмодули. Чтобы склонировать репозиторий вместе со всеми зависимостями, используйте одну из команд ниже:

### Быстрая загрузка
Если вам нужно просто собрать или запустить проект (скачивает только последние версии файлов без истории изменений, в 8 потоков):

```bash
git clone --recurse-submodules --shallow-submodules -j 8 https://github.com/velsbit/simple-ecs-game.git
```
### Полная загрузка (для разработки)
Если вы планируете вносить правки в проект и его сабмодули, сохраняя всю историю коммитов:

```bash
git clone --recursive https://github.com/velsbit/simple-ecs-game.git
```

## Сборка
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release

cmake --build build --config Release
```

## Запуск
После успешной сборки исполняемый файл будет находиться в папке `build`:

* **Linux:** `./build/Release/MyGame`
* **Windows:** `.\build\Release\MyGame.exe`