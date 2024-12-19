#include "raylib.h"

class GameCamera {
public:
    Camera2D camera;
    float leftBorder = 0.0f;
    float rightBorder = 10470.0f;

    GameCamera(int screenWidth, int screenHeight) {
        camera.target = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f }; // Центрирование на игроке
        camera.offset = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f}; // Смещение для центрирования
        camera.rotation = 0.0f; // Угол поворота
        camera.zoom = 1.0f;
    }

    void setCameraTarget(int x, int y) {
        if (x > camera.target.x && x < rightBorder - 410) {
            camera.target.x = x;
            leftBorder = x - 450;
        }
        
        camera.target.y = y;        
    }
};