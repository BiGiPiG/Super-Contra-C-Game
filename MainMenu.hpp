#include "raylib.h"
#include <iostream>

class MainMenu {
public:
    MainMenu() {
        // Конструктор может быть использован для инициализации переменных, если необходимо
    }

    void Show() {
        while (!WindowShouldClose()) {
            BeginDrawing();
            ClearBackground(RAYWHITE);
            
            DrawText("Главное Меню", 350, 200, 40, DARKGRAY);
            DrawText("Нажмите [ENTER] для начала игры", 250, 300, 20, DARKGRAY);
            DrawText("Нажмите [ESC] для выхода", 270, 350, 20, DARKGRAY);
            
            EndDrawing();

            if (IsKeyPressed(KEY_ENTER)) {
                break; // Выход из меню и начало игры
            }
            
        }
    }
};