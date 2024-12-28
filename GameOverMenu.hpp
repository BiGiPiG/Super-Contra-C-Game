#include "raylib.h"
#include <iostream>
#include <format>

class GameOverMenu {
private:

    int countFrames = 2;
    int currentFrame = 0;
    int hiScore;
    int curScore;

    Texture2D gameOverMenu;
    Music music;
    Rectangle textureRec = {0, 0, 900, 700};

    float musicTime = 6.5f;

public:
    GameOverMenu(int hiScore, int curScore) : hiScore(hiScore), curScore(curScore) {}

    ~GameOverMenu() {
        UnloadTexture(gameOverMenu);
    }

    void updateScore(int hiScore, int curScore) {
        this->hiScore = hiScore;
        this->curScore = curScore;
    }

    int show() {

        gameOverMenu = LoadTexture("resources/GameOverMenu.png");
        Font font = LoadFontEx("resources/Font.ttf", 20, NULL, 0);

        Music music = LoadMusicStream("resources/GameOver.ogg"); // Загрузка музыки
        PlayMusicStream(music); // Воспроизведение музыки

        while (true) {
            if (GetMusicTimePlayed(music) >= musicTime) {
                StopMusicStream(music); 
            } else {
                UpdateMusicStream(music); 
            }

            BeginDrawing();
            
            ClearBackground(BLACK);
            
            DrawTextureRec(gameOverMenu, textureRec, Vector2 {0, 0}, WHITE);
            DrawTextEx(font, "1P SCORE", (Vector2){100, 195}, font.baseSize, 2, WHITE);
            DrawTextEx(font, std::format(" {:06}", curScore).c_str(), (Vector2){100, 250}, font.baseSize, 2, WHITE);

            DrawTextEx(font, "2P SCORE", (Vector2){590, 195}, font.baseSize, 2, WHITE);
            DrawTextEx(font, "  SOON  ", (Vector2){590, 250}, font.baseSize, 2, WHITE);

            DrawTextEx(font, "HI SCORE", (Vector2){345, 80}, font.baseSize, 2, WHITE);
            DrawTextEx(font, std::format(" {:06}", hiScore).c_str(), (Vector2){345, 135}, font.baseSize, 2, WHITE);
            
            EndDrawing();

            if (IsKeyPressed(KEY_ENTER)) {
                UnloadFont(font); 
                if (currentFrame == 0) {
                    UnloadMusicStream(music); // Освобождение памяти
                    return 2;
                } else {
                    UnloadMusicStream(music); // Освобождение памяти
                    return 1;
                }
            }

            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_DOWN)) {
                currentFrame = (currentFrame + 1) % countFrames;
                textureRec.x = gameOverMenu.width / countFrames * currentFrame;
            }
        }
        return -1;
    }
};