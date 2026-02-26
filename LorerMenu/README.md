# Lorer - Standoff 2 Cheat Framework

Полный фреймворк для модификации игры Standoff 2 (v0.37.1) на эмуляторе BlueStacks 5.

## Структура проекта

```
LorerMenu/
├── Loader/
│   └── Loader.cpp        # Загрузчик (Injector)
├── DllMain.cpp           # Точка входа DLL
├── MemoryManager.cpp/h   # Работа с памятью
├── CheatFeatures.cpp/h   # Функции чита
├── LorerMenu.cpp/h       # ImGui меню
├── Offsets.h             # Офсеты игры
├── CMakeLists.txt        # Конфигурация сборки
├── build.bat             # Скрипт сборки
└── external/             # Внешние библиотеки
    ├── imgui/            # ImGui (нужно скачать)
    └── minhook/          # MinHook (нужно скачать)
```

## Результат сборки

| Файл | Описание |
|------|----------|
| **Loader.exe** | Загрузчик для внедрения DLL |
| **Lorer.dll** | Основной модуль с функциями |

## Быстрая сборка

### 1. Скачайте зависимости

**ImGui:**
```
https://github.com/ocornut/imgui
-> Извлечь в external/imgui/
```

**MinHook:**
```
https://github.com/TsudaKageyu/minhook/releases
-> Извлечь в external/minhook/
```

### 2. Соберите проект

```batch
# Windows
build.bat

# Или вручную через CMake
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

### 3. Результат

После сборки в корне появятся:
- `Loader.exe` - загрузчик
- `Lorer.dll` - DLL с читом

## Использование

1. Запустите **BlueStacks 5**
2. Запустите **Standoff 2**
3. Запустите **Loader.exe** от имени администратора
4. Нажмите **INSERT** для открытия меню

## Функции

### Aimbot
- Включение/выключение
- Настройка FOV (0-360°)
- Плавность прицеливания
- Выбор кости (Head, Neck, Chest, Stomach)
- Проверка видимости
- Silent Aim
- Отрисовка FOV круга

### Visuals (ESP)
- Box ESP
- Health Bar
- Name ESP
- Distance ESP
- Skeleton ESP
- Snaplines
- Head Dot
- Weapon ESP
- Настройка цветов

### Misc
- Bunny Hop
- No Flash
- FOV Changer
- Fake Lag
- Auto Strafe

## Офсеты (Standoff 2 v0.37.1)

```cpp
// GameManager
GameManager_StaticInstance = 0x18

// PlayerManager
PlayerManager_StaticInstance = 0x18
PlayerManager_LocalPlayer = 0x68
PlayerManager_PlayerList = 0x38

// PlayerController
PlayerController_Health = 0x150
PlayerController_Team = 0x154
PlayerController_MovementController = 0x98
PlayerController_IsVisible = 0x1A0

// MovementController
MovementController_TranslationData = 0xB0

// TranslationData
TranslationData_Position = 0x1C
```

## Технические детали

- **Графика:** DirectX 11
- **Хуки:** MinHook (Present hook)
- **UI:** ImGui
- **Платформа:** Windows x64
- **Цель:** HD-Player.exe (BlueStacks 5)

## Системные требования

- Windows 10/11
- Visual Studio 2019/2022
- CMake 3.16+
- DirectX 11 SDK

---

**Версия:** 1.0.0  
**Цель:** Standoff 2 v0.37.1  
**Платформа:** BlueStacks 5 (Windows)
