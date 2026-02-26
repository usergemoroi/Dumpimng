# Lorer - Готовый проект для Visual Studio

## Быстрая сборка (3 шага)

### 1. Скачайте зависимости
```
Дважды кликните: download_deps.bat
```
Это автоматически скачает ImGui и MinHook.

### 2. Откройте проект
```
Дважды кликните: Lorer.sln
```
Откроется в Visual Studio 2019/2022.

### 3. Соберите
```
В меню: Build -> Build Solution (Ctrl+Shift+B)
```

## Результат

После сборки в папке `Output/`:
- **Loader.exe** - Загрузчик (инжектор)
- **Lorer.dll** - Основной модуль чита

## Использование

1. Запустите **BlueStacks 5**
2. Запустите **Standoff 2**
3. Запустите **Loader.exe** от имени администратора
4. Нажмите **INSERT** для открытия меню

## Файлы проекта

```
LorerMenu/
├── Lorer.sln           ← Откройте это в Visual Studio
├── Loader.vcxproj      ← Проект Loader.exe
├── Lorer.vcxproj       ← Проект Lorer.dll
├── download_deps.bat   ← Скачает ImGui и MinHook
│
├── Loader/
│   └── Loader.cpp      ← Код загрузчика
│
├── DllMain.cpp         ← Точка входа DLL
├── LorerMenu.cpp/h     ← Меню ImGui
├── MemoryManager.cpp/h ← Работа с памятью
├── CheatFeatures.cpp/h ← Функции чита
├── Offsets.h           ← Офсеты игры
│
└── external/
    ├── imgui/          ← ImGui (скачается автоматически)
    └── minhook/        ← MinHook (скачается автоматически)
```

## Если MinHook.lib не скачался

1. Откройте: https://github.com/TsudaKageyu/minhook/releases
2. Скачайте последнюю версию
3. Извлеките `libMinHook.x64.lib` в `external/minhook/lib/`

## Требования

- Windows 10/11
- Visual Studio 2019 или 2022
- Windows SDK 10

---

**Версия:** 1.0.0  
**Игра:** Standoff 2 v0.37.1  
**Платформа:** BlueStacks 5
