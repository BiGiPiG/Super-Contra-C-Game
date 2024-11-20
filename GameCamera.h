#include "raylib.h"

class GameCamera {
public:
    Camera2D camera;

    GameCamera(int screenWidth, int screenHeight) {
        camera.target = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f }; // Центрирование на игроке
        camera.offset = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f}; // Смещение для центрирования
        camera.rotation = 0.0f; // Угол поворота
        camera.zoom = 1.0f;
    }

    void setCameraTarget(int x) {
        camera.target.x = x;
    }
};