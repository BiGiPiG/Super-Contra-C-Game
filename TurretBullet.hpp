#pragma once
#include "raylib.h"
#include "Platform.hpp"
#include "Ladder.hpp"
#include <cmath>
   
class TurretBullet {
public:

    float bulletSize = 16.0f;
    int currentFrameShoot = 0;
    int countFrame = 3;
    float frameTimer = 0.0f;
    float frameSpeed = 0.5f;

    Vector2 position;
    Vector2 velocity = {-5.0f, 0.0f};
    Rectangle hitBox;
    Texture2D bullet = LoadTexture("resources/TurretBullet.png");
    Rectangle textureRec = {0, 0, bulletSize, bulletSize};

    bool isActive = true;

    bool getAlive() {
        return isActive;
    }

    int isGround(std::vector<std::shared_ptr<MapObject>> mapObjects) {
        for (auto obj: mapObjects) {
             if (auto platform = dynamic_cast<Platform*>(obj.get()))  {
                if (platform->isCollision(hitBox)) {
                    return platform->getRectangle().y;
                }
            } 
        }
        return 0;
    }

    bool isLadder(std::vector<std::shared_ptr<MapObject>> mapObjects) {
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

    TurretBullet(float startX, float startY) {
        position = { startX, startY };

        hitBox = { position.x, position.y, bulletSize, bulletSize}; // Размеры пули
        isActive = true;
    }

    ~TurretBullet() {
        UnloadTexture(bullet);
    }

    void update(std::vector<std::shared_ptr<MapObject>> mapObjects, Vector2 playerPos) {
        if (isActive) {
            position.x += velocity.x;
            position.y += velocity.y;
            hitBox.x = position.x; // Обновляем хитбокс
            hitBox.y = position.y;

            // Деактивируем пулю, если она выходит за пределы экрана
            if (position.x < 0 || position.x > playerPos.x + GetScreenWidth() 
            || position.y > playerPos.y + GetScreenHeight()) {
                isActive = false;
            }
            if (isGround(mapObjects) || isLadder(mapObjects) || playerPos.x - position.x >= 600) {
                isActive = false;
            }
        }
    }

    void updateAnimation(float deltaTime) {
        frameTimer += deltaTime;
        if (frameTimer >= frameSpeed) {
            currentFrameShoot = (currentFrameShoot + 1) % countFrame;
            textureRec.x = currentFrameShoot * bulletSize; // Update texture rectangle for animation
            frameTimer = 0.0f;
        }
        
    }

    void draw() {
        
        if (isActive) {       
            DrawTextureRec(bullet, textureRec, position, WHITE);
            //DrawRectangleRec(hitBox, GREEN);
        }
    }
};