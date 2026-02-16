cmake_minimum_required(VERSION 3.10)
project(MyGame)

# --- 1. Настройка выходных папок ---
# Исполняемые файлы и DLL будут попадать в build/Debug или build/Release
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Release")

# --- 2. Списки исходных файлов ---
set(CORE_SRC
    src/core/graphics_sdl.c
    src/core/time_sdl.c
    src/core/input_sdl.c
    src/core/assets.c
    src/core/ecs.c
)

set(SYSTEMS_SRC
    src/systems/render.c
    src/systems/movement_platformer.c
    src/systems/camera.c
    src/systems/ai_control.c
)

set(MAIN_SRC
    src/main.c
    src/game.c
)

# --- 3. Подключение зависимостей ---
include_directories(include)

add_subdirectory(extensions/SDL)
add_subdirectory(extensions/SDL_image)

# --- 4. Создание проекта ---
add_executable(${PROJECT_NAME} WIN32 ${CORE_SRC} ${SYSTEMS_SRC} ${MAIN_SRC})

# Если вы хотите, чтобы консоль была в Debug, а в Release исчезала:
set_target_properties(${PROJECT_NAME} PROPERTIES 
    WIN32_EXECUTABLE $<NOT:$<CONFIG:Debug>>
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
)

# Определение пути к ассетам в коде
target_compile_definitions(${PROJECT_NAME} PRIVATE 
    $<$<CONFIG:Debug>:ASSET_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/assets/">
    $<$<CONFIG:Release>:ASSET_ROOT="assets/">
)

# --- 5. Пост-сборка: Копирование DLL и Ассетов ---

# Копируем DLL всегда (и в Debug, и в Release) в папку к .exe
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3>
        $<TARGET_FILE:SDL3_image::SDL3_image>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMENT "Copying runtime dependencies (DLLs)..."
)

# Этот блок сработает только если мы явно собираем НЕ Debug
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
                "${CMAKE_CURRENT_SOURCE_DIR}/assets" 
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
        COMMENT "Release: Copying assets to build folder..."
    )
endif()