#pragma once

#include "raylib.h"

#include "Bullet.hpp"

#include <memory>
#include <cmath>

class Bonus {
private:

    //текстуры
    Vector2 position; // Позиция бонуса
    Texture2D texture; // Текстура бонуса
    Rectangle hitBox; // Хитбокс для столкновения

    //движение
    bool isActive; // Флаг активности бонуса
    float amplitude; // Амплитуда синусоиды
    float frequency; // Частота синусоиды
    float elapsedTime; // Время, прошедшее с начала движения

    void die() {
        isHidden = true;
        
    }

public:
    Bonus(float x, float y, const char* texturePath) 
        : position({x, y}), isActive(true) {
        texture = LoadTexture(texturePath); // Загрузка текстуры бонуса
        hitBox = { position.x, position.y, (float)texture.width, (float)texture.height }; // Инициализация хитбокса
    }

    ~Bonus() {
        UnloadTexture(texture); // Освобождение текстуры при уничтожении объекта
    }

    void update(float deltaTime) {
        if (isActive) {
            elapsedTime += deltaTime; // Увеличиваем время

            // Обновляем позицию по синусоиде
            position.y += sin(elapsedTime * frequency) * amplitude;

            hitBox.x = position.x; // Обновляем хитбокс по позиции
            hitBox.y = position.y;
        }
    }

    void draw() const {
        if (isActive) {
            DrawTexture(texture, (int)position.x, (int)position.y, WHITE); // Отрисовка бонуса
            // DrawRectangleRec(hitBox, GREEN); // Для отладки: отрисовка хитбокса
        }
    }

    void checkCollision(const std::vector<Bullet> bullets) {

        if (bullets.empty()) {
            return;
        }

        for (const auto &bullet : bullets) {
            if (CheckCollisionRecs(hitBox, bullet->hitBox)) {
                die();
                return;
            }
        }
    }

    bool isBonusActive() const {
        return isActive; // Возвращаем состояние активности бонуса
    }

    Vector2 getPosition() const {
        return position; // Возвращаем позицию бонуса
    }
};