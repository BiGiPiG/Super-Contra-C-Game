#pragma once

#include "raylib.h"
#include "Granate.hpp"
#include "TurretBullet.hpp"
#include "LedderBullet.hpp"

#include <vector>
#include <memory>
#include <variant>

using BulletVariant = std::variant<std::shared_ptr<LedderBullet>, std::shared_ptr<TurretBullet>, std::shared_ptr<Granate>>;

class GranateThrower {
private:

    int countFrames = 2;
    int currentFrame = 0;
    int deathFrames = 3;

    float frameTimer = 0.0f; // Таймер для управления временем между кадрами
    float frameSpeed = 0.2f; // Скорость анимации (время между кадрами)

    Vector2 position;
    Rectangle hitBox;
    Rectangle textureRec;
    Texture2D granateThrower;
    
    bool isLifting = false; // Флаг, указывающий на подъем
    bool isAlive = true;
    bool changeFrame = false;
    bool isDying = false;

    int granateQueueCount = 3; // Количество гранат в очереди
    int currentGranateCount = 0; // Текущий счетчик гранат в очереди
    float throwInterval = 0.5f; // Интервал между бросками (в секундах)
    float throwTimer = 0.0f; // Таймер для отслеживания времени
    float queueInterval = 1.5f; // Промежуток между очередями (в секундах)
    float queueTimer = 0.0f; // Таймер для отслеживания времени между очередями
    float liftSpeed = 5.0f; // Скорость подъема при смерти
    float liftTime = 0.2f;
    float liftTimer = 0.0f;

    void resetThrowing() {
        currentGranateCount = 0; // Сбрасываем счетчик гранат в очереди после завершения бросков
        throwTimer = 0.0f; // Сбрасываем таймер
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
                
                textureRec.x = granateThrower.width / deathFrames * currentFrame;

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
        DrawTextureRec(granateThrower, textureRec, position, WHITE);
    }

    void die() { 
        isDying = true;
        currentFrame = 0; 
        changeFrame = true;
        isLifting = true;
    }

public:

    bool getAlive() {
        return isAlive;
    }

    Vector2 getPosition() {
        return position;
    }

    GranateThrower(float startX, float startY) {
        position = { startX, startY };
        hitBox = { 25.0f + startX, startY + 28.0f, 50.0f, 100.0f }; // Размер хитбокса
        textureRec = { 0, 0, 101.0f, 128.0f }; 
        granateThrower = LoadTexture("resources/GranateThrower.png");
    }

    ~GranateThrower() {
        UnloadTexture(granateThrower);
    }

    //бросить гранату
    void throwGranate(std::vector<BulletVariant> &bullets) {
        if (currentGranateCount < granateQueueCount) {
            bullets.push_back(std::make_shared<Granate>(position.x + 15, position.y, "resources/Granate.png"));
            currentGranateCount++;
            changeFrame = true;
        }
    }

    void update(float deltaTime, std::vector<BulletVariant> &bullets, Player& player) {
        if (isDying) {
            updateDeathAnimation(deltaTime); // Обновляем анимацию смерти если метатель мертв
            return; // Прекращаем выполнение метода, если метатель мертв
        }

        throwTimer += deltaTime; // Увеличиваем таймер

        if (player.getPosition().x >= position.x - 500 && currentGranateCount < granateQueueCount) {
            if (throwTimer >= throwInterval) {
                throwGranate(bullets); // Бросаем гранату
                throwTimer = 0.0f; // Сбрасываем таймер
            }
        } else {
            queueTimer += deltaTime;
        }
        
        if (queueTimer >= queueInterval) {
            resetThrowing(); // Если все гранаты были брошены и время ожидания прошло, сбрасываем счетчик
        }
        
        updateAnimation(deltaTime); // Обновляем анимацию метателя
    }

    //отрисовка метателя
    void draw() {
        if (!isDying) {
            DrawTextureRec(granateThrower, textureRec, position, WHITE);
            //DrawRectangleRec(hitBox, GREEN);
        } else {
            drawDeathAnimation(); // Отрисовываем анимацию смерти
        }
    }

    //обновление анимации
    void updateAnimation(float deltaTime) {
        if (changeFrame) {
            frameTimer += deltaTime; // Увеличиваем таймер на время прошедшее с последнего обновления

            if (frameTimer >= frameSpeed) { 
                currentFrame = (currentFrame + 1) % countFrames; // Переход к следующему кадру
                textureRec.x = granateThrower.width / countFrames * currentFrame; // Обновление текстурной области
                frameTimer = 0.0f; // Сбрасываем таймер

                if (currentFrame == 0) {
                    changeFrame = false; // Сбрасываем флаг изменения кадра после полного цикла
                }
            }
        }
    }

    //проверка состояния метателя
    void checkDie(std::vector<std::shared_ptr<Bullet>> bullets, int &score) {

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