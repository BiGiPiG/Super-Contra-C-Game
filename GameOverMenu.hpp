#include "raylib.h"
#include <iostream>

class GameOverMenu {
private:
    int countFrames = 2;
    int currentFrame = 0;
    Texture2D gameOverMenu;
    Rectangle textureRec = {0, 0, 900, 700};

public:
    GameOverMenu() {
        // Конструктор может быть использован для инициализации переменных, если необходимо
        gameOverMenu = LoadTexture("resources/GameOverMenu.png");
    }

    ~GameOverMenu() {
        UnloadTexture(gameOverMenu);
    }

    int show() {
        while (!WindowShouldClose()) {
            BeginDrawing();
            ClearBackground(RAYWHITE);
            
            DrawTextureRec(gameOverMenu, textureRec, Vector2 {0, 0}, WHITE);
            
            EndDrawing();

            if (IsKeyPressed(KEY_ENTER)) {
                if (currentFrame == 0) {
                    return 1;
                } else {
                    return 0;
                }
            }

            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_DOWN)) {
                currentFrame = (currentFrame + 1) % countFrames;
                textureRec.x = gameOverMenu.width / countFrames * currentFrame;
            }
        }
    }
};