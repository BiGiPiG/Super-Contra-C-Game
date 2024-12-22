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
        Font font = LoadFontEx("resources/Font.ttf", 20, NULL, 0);

        Music music = LoadMusicStream("resources/MainMenu.ogg"); // Загрузка музыки
        PlayMusicStream(music); // Воспроизведение музыки

        while (!WindowShouldClose()) {
            if (GetMusicTimePlayed(music) >= 5.5f) {
                StopMusicStream(music); // Stop the music when it finishes
            } else {
                UpdateMusicStream(music); // Always update the music stream
            }


            BeginDrawing();
            ClearBackground(RAYWHITE);
            
            DrawTextureRec(mainMenu, textureRec, Vector2 {0, 0}, WHITE);

            DrawTextEx(font, "PLAY", (Vector2){170, 500}, font.baseSize, 2, WHITE);
            DrawTextEx(font, "EXIT", (Vector2){510, 500}, font.baseSize, 2, WHITE);
            
            EndDrawing();

            if (IsKeyPressed(KEY_ENTER) && currentFrame == 0) {
                UnloadMusicStream(music); // Освобождение памяти
                break; // Выход из меню и начало игры
            } else if (IsKeyPressed(KEY_ENTER) && currentFrame == 1) {
                UnloadMusicStream(music); // Освобождение памяти
                CloseWindow();
                return 0;
            }

            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_LEFT)) {
                currentFrame = (currentFrame + 1) % countFrames;
                textureRec.x = mainMenu.width / countFrames * currentFrame;
            }

            if (WindowShouldClose()) {
                UnloadMusicStream(music); // Освобождение памяти
                CloseWindow();
                return 0;
            }
        }
        return 1;
    }
};