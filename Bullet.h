#pragma once
#include "raylib.h"

class Bullet {
public:

    float bulletSize = 15.0f;

    Vector2 position;
    Vector2 velocity;
    Rectangle hitBox;
    Texture2D bullet = LoadTexture("resources/MainBullet.png");
    Rectangle textureRec = {0, 0, bulletSize, bulletSize};
    bool isActive;

    Bullet(float startX, float startY, float speedX, float speedY) {
        position = { startX, startY };
        velocity = { speedX, speedY };
        hitBox = { position.x, position.y, bulletSize, bulletSize}; // Размеры пули
        isActive = true;
    }

    ~Bullet() {
        UnloadTexture(bullet);
    }

    void update() {
        if (isActive) {
            position.x += velocity.x;
            position.y += velocity.y;
            hitBox.x = position.x; // Обновляем хитбокс
            hitBox.y = position.y;

            // Деактивируем пулю, если она выходит за пределы экрана
            if (position.x < 0 || position.x > position.x + GetScreenWidth() || 
                position.y < 0 || position.y > position.y + GetScreenHeight()) {
                isActive = false;
            }
        }
    }

    void draw() const {
        if (isActive) {
            DrawTextureRec(bullet, textureRec, position, WHITE);
        }
    }
};