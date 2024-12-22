#pragma once
#include "raylib.h"
#include "LedderBullet.hpp"
#include "TurretBullet.hpp"

#include <variant>
#include <memory>
#include <vector>
#include <algorithm>

using BulletVariant = std::variant<std::shared_ptr<LedderBullet>, std::shared_ptr<TurretBullet>, std::shared_ptr<Granate>>;

class Ledder {
private:

    Vector2 position;
    Rectangle hitBox;
    Rectangle textureRec;
    Texture2D ledder;

    bool lookRight = true;
    bool lookLeft = false;
    bool isDying = false;
    bool isLifting = false;
    bool isAlive = true;

    float shotTimer = 0.0f;
    float queueTimer = 0.0f;
    float liftSpeed = 5.0f; // Скорость подъема при смерти
    float liftTime = 0.2f;
    float liftTimer = 0.0f;
    float queueInterval = 3.0f;
    float shotInterval = 0.2f;
    float frameTimer = 0.0f;
    float frameSpeed = 0.2f;

    int currentShotCount = 0;
    int shotQueueCount = 2;
    int currentFrame = 0;
    int deathFrames = 3;

    void die() { 
        isDying = true;
        currentFrame = 0; 
        isLifting = true;
    }

    void resetShooting() {
        currentShotCount = 0; // Сбрасываем счетчик гранат в очереди после завершения бросков
        shotTimer = 0.0f; // Сбрасываем таймер
        queueTimer = 0.0f; // Сбрасываем таймер
    }

    void updateDeathAnimation(float deltaTime) {

        liftTimer += deltaTime;
        frameTimer += deltaTime;
        if (!isLifting) { 

            if (frameTimer >= frameSpeed) { 
                currentFrame++;
                frameTimer = 0.0f;

                if (currentFrame >= deathFrames) { 
                    currentFrame = deathFrames - 1; // Останавливаемся на последнем кадре анимации смерти
                    isAlive = false; // Устанавливаем флаг мертвого состояния после достижения определенной высоты
                }
                
                textureRec.x = ledder.width / deathFrames * currentFrame;

            }
        } else { 
            position.y -= liftSpeed; // Поднимаем метателя вверх при смерти
            
            if (liftTimer >= liftTime) { 
                isLifting = false;
                position.x += 15.0f;
                position.y += 70.0f;
                textureRec.x = 0;
                textureRec.y = 128; 
                textureRec.width = 64;
                textureRec.height = 64;
            }
        }
    }

    void drawDeathAnimation() const {
        DrawTextureRec(ledder, textureRec, position, WHITE);
    }

public:

    bool getAlive() {
        return isAlive;
    }

    Vector2 getPosition() {
        return position;
    }

    Ledder(float x, float y) {
        position = Vector2 {x, y};
        hitBox = { 25.0f + x, y + 28.0f, 50.0f, 100.0f }; // Размер хитбокса
        textureRec = { 0, 0, 101.0f, 128.0f }; 
        ledder = LoadTexture("resources/Ledder.png");
    }


     void shoot(Vector2 &target, std::vector<BulletVariant> &bullets) {
        float bulletPosX = position.x + (lookRight ? 65 : 20);
        float bulletPosY = position.y + 70;
        bullets.push_back(std::make_shared<LedderBullet>(bulletPosX, bulletPosY, target.x + 45, target.y + 60, lookRight));
    }

    void update(Vector2 target, std::vector<std::shared_ptr<MapObject>> &mapObjects, 
            float deltaTime, std::vector<BulletVariant> &bullets) {
    
        if (isDying) {
            // Update death animation if the shooter is dead
            updateDeathAnimation(deltaTime);
            return; // Stop execution if the shooter is dead
        }

        shotTimer += deltaTime; // Increment shot timer

        // Flip texture based on direction
        if ((textureRec.width < 0 && target.x > position.x) || 
            (textureRec.width > 0 && target.x <= position.x)) {
            textureRec.width = -textureRec.width; // Flip texture
            std::swap(lookLeft, lookRight); // Swap look directions
        }

        // Handle shooting logic
        if (currentShotCount < shotQueueCount) {
            if (shotTimer >= shotInterval) {
                shoot(target, bullets); // Execute shooting
                shotTimer = 0.0f; // Reset shot timer
                currentShotCount++; // Increment shot count
            }
        } else {
            queueTimer += deltaTime; // Increment queue timer for reloading
        }
        
        // Reset shooting after queue interval
        if (queueTimer >= queueInterval) {
            resetShooting(); // Reset shooting state
            queueTimer = 0.0f; // Reset queue timer for next cycle
        }

    }

    void draw() {
        if (!isDying) {
            DrawTextureRec(ledder, textureRec, position, WHITE);
            //DrawRectangleRec(hitBox, GREEN);
        } else {
            drawDeathAnimation(); // Отрисовываем анимацию смерти
        }
    }

    void checkDie(std::vector<std::shared_ptr<Bullet>> &bullets, int &score) {
        if (isDying) {
            return;
        }

        if (!bullets.empty()) {
            for (const auto &bullet : bullets) {
                if (CheckCollisionRecs(hitBox, bullet->hitBox)) {
                    score += 1000;
                    die();
                    return;
                }
            }
        }
    }

};
