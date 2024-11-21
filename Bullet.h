#pragma once
#include "raylib.h"

class Bullet {
public:
    Vector2 position;
    Vector2 velocity;
    Rectangle hitBox;
    bool isActive;

    Bullet(float startX, float startY, float speedX, float speedY) {
        position = { startX, startY };
        velocity = { speedX, speedY };
        hitBox = { position.x, position.y, 5.0f, 5.0f }; // Размеры пули
        isActive = true;
    }

    void update() {
        if (isActive) {
            position.x += velocity.x;
            position.y += velocity.y;
            hitBox.x = position.x; // Обновляем хитбокс
            hitBox.y = position.y;

            // Деактивируем пулю, если она выходит за пределы экрана
            if (position.x < 0 || position.x > GetScreenWidth() || 
                position.y < 0 || position.y > GetScreenHeight()) {
                isActive = false;
            }
        }
    }

    void draw() const {
        if (isActive) {
            DrawRectangleRec(hitBox, RED); // Отрисовка пули
        }
    }
};