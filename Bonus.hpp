#pragma once

#include "raylib.h"

#include "Bullet.hpp"

#include <memory>
#include <cmath>

enum BonusType {TYPE_1, TYPE_2};

class Bonus {
private:

    //текстуры
    Vector2 position; // Позиция бонуса
    Texture2D bonus; // Текстура бонуса
    Rectangle hitBox; // Хитбокс для столкновения
    Vector2 velocity = {8.0f, 0.0f};
    Rectangle textureRec;
    BonusType type;

    int currentFrame = 0;
    int countFrames = 7;
    float gravity = 0.5;


    //движение
    bool isActive = false; // Флаг активности бонуса
    float amplitude = 100; // Амплитуда синусоиды
    float timeCounter = 0.0f;
    float elapsedTime = 0.0f;
    float delay = 0.1f;
    bool isDying = false;
    float jumpHeight = 12;
    bool isJumping = false;
    bool isAlive = true;
    bool check = false;
    int delta;

    void die() {
        isDying = true;
        velocity.x = 0;
        velocity.y = -jumpHeight; // Устанавливаем вертикальную скорость для прыжка
        isJumping = true;   // Устанавливаем флаг прыжка
    }

public:
    Bonus(float x, float y, const char* texturePath, BonusType bonusType, int delta) 
        : position({x, y}), delta(delta) {
        type = bonusType;
        bonus = LoadTexture(texturePath); // Загрузка текстуры бонуса
        hitBox = { position.x, position.y, (float)bonus.width / countFrames, (float)bonus.height }; // Инициализация хитбокса
        textureRec = { 0, 0, (float)bonus.width / countFrames, (float)bonus.height };
    }

    ~Bonus() {
        UnloadTexture(bonus); // Освобождение текстуры при уничтожении объекта
    }

    Rectangle getHitBox() {
        return hitBox;
    }

    BonusType getType() {
        return type;
    }

    void setAlive() {
        isAlive = false;
    }

    int isOnGround(std::vector<std::shared_ptr<MapObject>> mapObjects) {
        for (auto obj: mapObjects) {
             if (auto platform = dynamic_cast<Platform*>(obj.get()))  {
                
                if (platform->isCollision(hitBox)) {
                    return platform->getRectangle().y;
                }
            } 
        }
        return 0;
    }

    bool isOnLadder(std::vector<std::shared_ptr<MapObject>> mapObjects) {
        for (auto obj: mapObjects) {
            if (auto ladder = dynamic_cast<Ladder*>(obj.get())) {
                Vector2 hitBoxCenter = { hitBox.x + hitBox.width, hitBox.y + hitBox.height};
                
                if (ladder->isCollision(hitBoxCenter)) {
                    //std::cout << "ladder" << std::endl;
                    return true;
                }
            }
        }
        return false;
    }

    void updateAnimation(float deltaTime) {
        if (isDying) {
            timeCounter += deltaTime;
            if (timeCounter >= delay) {
                currentFrame = (currentFrame + 1) % (countFrames - 1) + 1;
                textureRec.x = bonus.width / countFrames * currentFrame;
                timeCounter = 0.0;
            }
        } else {
            textureRec.x = 0.0f;
        }
        
    }

    void update(float deltaTime, const std::vector<std::shared_ptr<MapObject>> &mapObjects) {
        if (isActive && !isDying) {
            // Двигаем бонус по горизонтали
            position.x += velocity.x;

            // Двигаем бонус по синусоиде с учетом времени
            elapsedTime += deltaTime; // Увеличиваем счётчик времени
            position.y = amplitude * sin(elapsedTime) + delta; // Синусоидальное движение

            // Обновляем хитбокс по новой позиции
            hitBox.x = position.x;
            hitBox.y = position.y;
        } else if (isActive && isJumping) {
            velocity.y += gravity; // Применяем гравитацию к вертикальной скорости
            position.y += velocity.y; // Обновляем позицию по Y
            hitBox.y = position.y;
            if (isOnGround(mapObjects)) { // Проверка на приземление

                position.y = isOnGround(mapObjects) - bonus.height; // Сброс позиции на землю
                isJumping = false; // Завершение прыжка
                velocity.y = 0.0f; // Сброс скорости прыжка
                hitBox.y = position.y;
            } else if (isOnLadder(mapObjects)) {
                position.y -= 2.5; // Поднимаем игрока немного выше, чтобы он не проваливался сквозь лестницу
                hitBox.y = 38.0f + position.y; // Обновляем хитбокс
                isJumping = false; 
                velocity.y = 0.0f;
            }
        }
    }

    void draw() const {
        if (isActive) {
            DrawTextureRec(bonus, textureRec, position, WHITE);
            //DrawRectangleRec(hitBox, GREEN); // Для отладки: отрисовка хитбокса
        }
    }

    void isBonusActive(Vector2 playerPos) {
        std::cout << playerPos.x - position.x << std::endl;
        if (playerPos.x - position.x > 450) {
            isActive = true;
            check = true;
        }
        if (position.x - playerPos.x >= 1500 && check) {
            isAlive = false;
        }
    }

    bool getAlive() {
        return isAlive;
    }

    Vector2 getPosition() const {
        return position; // Возвращаем позицию бонуса
    }

    void checkDie(std::vector<std::shared_ptr<Bullet>> bullets, int &score) {

        if (isDying) {
            return;
        }

        if (!bullets.empty()) {
            for (const auto &bullet : bullets) {
                if (CheckCollisionRecs(hitBox, bullet->hitBox)) {
                    score += 10000;
                    die();
                    return;
                }
            }
        }
    }

};