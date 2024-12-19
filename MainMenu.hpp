#include "raylib.h"
#include <iostream>

class MainMenu {
private:
    int countFrames = 2;
    int currentFrame = 0;
    Texture2D mainMenu;
    Rectangle textureRec = {0, 0, 900, 700};

public:
    MainMenu() {
        // Конструктор может быть использован для инициализации переменных, если необходимо
        mainMenu = LoadTexture("resources/MainMenu.png");
    }

    ~MainMenu() {
        UnloadTexture(mainMenu);
    }

    int show() {
        while (!WindowShouldClose()) {
            BeginDrawing();
            ClearBackground(RAYWHITE);
            
            DrawTextureRec(mainMenu, textureRec, Vector2 {0, 0}, WHITE);
            
            EndDrawing();

            if (IsKeyPressed(KEY_ENTER)) {
                break; // Выход из меню и начало игры
            }

            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_LEFT)) {
                currentFrame = (currentFrame + 1) % countFrames;
                textureRec.x = mainMenu.width / countFrames * currentFrame;
            }

            if (WindowShouldClose()) {
                CloseWindow();
                return 0;
            }
        }
        return 1;
    }
};