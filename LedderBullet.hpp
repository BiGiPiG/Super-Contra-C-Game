#pragma once
#include "raylib.h"
#include "Platform.h"
#include "Ladder.h"
#include <cmath>

class LedderBullet {
public:

    float bulletSize = 16.0f;
    int currentFrameShoot = 0;
    int countFrame = 5;
    float frameTimer = 0.0f;
    float frameSpeed = 0.1f;

    Vector2 position;
    Vector2 velocity;
    Rectangle hitBox;
    Texture2D bullet = LoadTexture("resources/LedderBullet.png");
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

    LedderBullet(float startX, float startY, float targetX, float targetY, bool flag) {
        position = { startX, startY };

        std::cout << flag << std::endl;
        // Calculate direction vector from start to target
        float dirX = abs(targetX - startX);
        float dirY = abs(targetY - startY);

        // Calculate distance
        float distance = sqrt(dirX * dirX + dirY * dirY);

        // Normalize the direction vector and scale by desired speed (e.g., 20.0f)
        if (distance != 0) { // Prevent division by zero
            velocity = { (flag ? 1.0f : -1.0f) * (dirX / distance) * 5,
                          (dirY / distance) * 5 };
        } else {
            velocity = { 0.0f, 0.0f }; // If no distance, set velocity to zero
        }

        hitBox = { position.x, position.y, bulletSize, bulletSize}; // Размеры пули
        isActive = true;
    }

    ~LedderBullet() {
        UnloadTexture(bullet);
    }

    void update(std::vector<std::shared_ptr<MapObject>> mapObjects, Vector2 playerPos) {
        if (isActive) {
            position.x += velocity.x;
            position.y += velocity.y;
            hitBox.x = position.x; // Обновляем хитбокс
            hitBox.y = position.y;

            // Деактивируем пулю, если она выходит за пределы экрана
            if (position.x < 0 || abs(playerPos.x - position.x) >= 500) {
                isActive = false;
            }
            if (isGround(mapObjects) || isLadder(mapObjects)) {
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