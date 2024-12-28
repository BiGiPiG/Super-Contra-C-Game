#pragma once

#include "raylib.h"

#include "Platform.hpp"
#include "Ladder.hpp"

class Bullet {
public:

    float bulletSize = 32.0f;
    int currentFrameShoot = 0;
    int countFrame = 4;
    int damage = 1;

    Vector2 position;
    Vector2 velocity;
    Rectangle hitBox;
    Texture2D bullet = LoadTexture("resources/MainBullet.png");
    Rectangle textureRec = {0, 0, bulletSize, bulletSize};

    bool isActive = true;
    bool isHidden = false;

    int isGround(std::vector<std::shared_ptr<MapObject>> &mapObjects) {
        for (auto obj: mapObjects) {
             if (auto platform = dynamic_cast<Platform*>(obj.get()))  {
                if (platform->isCollision(hitBox)) {
                    return platform->getRectangle().y;
                }
            } 
        }
        return 0;
    }

    bool isLadder(std::vector<std::shared_ptr<MapObject>> &mapObjects) {
        for (auto obj: mapObjects) {
            if (auto ladder = dynamic_cast<Ladder*>(obj.get())) {
                Vector2 hitBoxCenter = { hitBox.x + hitBox.width, hitBox.y + hitBox.height};
                if (ladder->isCollision(hitBoxCenter)) {
                    return true;
                }
            }
        }
        return false;
    }

    Bullet(float startX, float startY, float speedX, float speedY,int damage) : damage(damage), isActive(true) {
        position = { startX, startY };
        velocity = { speedX, speedY };
        hitBox = { position.x, position.y, bulletSize, bulletSize}; // Размеры пули
    }

    ~Bullet() {
        BeginDrawing();
        draw();
        EndDrawing();
        UnloadTexture(bullet);
    }

    void update(std::vector<std::shared_ptr<MapObject>> &mapObjects, Vector2 &playerPos) {
        if (isActive) {
            position.x += velocity.x;
            position.y += velocity.y;
            hitBox.x = position.x; // Обновляем хитбокс
            hitBox.y = position.y;

            // Деактивируем пулю, если она выходит за пределы экрана
            if (position.x < 0 || position.x > playerPos.x + GetScreenWidth() 
            || position.y > playerPos.y + GetScreenHeight()) {
                isHidden = true;
            }
            if ((isGround(mapObjects) || isLadder(mapObjects)) && velocity.y >= 0) {
                isHidden = true;
            }
        }
    }

    void draw() {
        if (isActive && !isHidden) {       
            DrawTextureRec(bullet, textureRec, position, WHITE);
            //DrawRectangleRec(hitBox, GREEN);
        } else if (isHidden) {
            velocity = {0, 0};
            if (currentFrameShoot != 0) {
                textureRec.x = bullet.width / countFrame * currentFrameShoot;
                DrawTextureRec(bullet, textureRec, position, WHITE);
                //DrawRectangleRec(hitBox, GREEN);
            } else {
                isActive = false;
            }
        }
    }
};