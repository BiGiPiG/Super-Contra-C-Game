#pragma once
#include "raylib.h"

class Granate {

private:

    Vector2 position;
    
    Rectangle textureRec;
    Texture2D granateTexture;
    
    Vector2 velocity = {-2.0f, -7.0f};
    
    float gravity = 0.3f;
    int countFrame = 6;
    int currentFrame = 0;
    int granateSize = 32;
    int explosionSize = 64;
    int explosionFrames = 3;
    float explosionTimer = 0.0f; // Таймер для управления временем взрыва
    float explosionDuration = 0.5f; // Длительность анимации взрыва

    bool isActive = true;
    bool isLanding = false;
    bool changeFrane = false;
    bool isExploding = false; // Флаг для отслеживания состояния взрыва

public:

    Rectangle hitBox;

    bool getActive() {
        return isActive;
    }

    int isOnGround(std::vector<std::shared_ptr<MapObject>> &mapObjects) {
        for (auto obj: mapObjects) {
             if (auto platform = dynamic_cast<Platform*>(obj.get()))  {
                if (platform->isCollision(hitBox)) {
                    return true;
                }
            } 
        }
        return false;
    }

    bool isOnLadder(std::vector<std::shared_ptr<MapObject>> &mapObjects) {
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

    Granate(float startX, float startY, const char* texturePath) {
        position = { startX, startY };
        hitBox = { startX, startY, 32.0f, 32.0f }; // Размер хитбокса
        textureRec = { 0, 0, 32.0f, 32.0f }; 
        granateTexture = LoadTexture(texturePath);
    }

    ~Granate() {
        UnloadTexture(granateTexture);
    }

    void move() {
        if (!isLanding) {
            velocity.y += gravity; // Применяем гравитацию к вертикальной скорости
            position.x += velocity.x;
            position.y += velocity.y; // Обновляем позицию по Y

            hitBox.x = position.x;
            hitBox.y = position.y;
        } else {
            velocity = {0, 0};
        }
    }

    void updateGranate(std::vector<std::shared_ptr<MapObject>> &mapObjects) {
        if (!isExploding) {
            isLanding = isOnGround(mapObjects); // Проверяем на приземление
            if (isLanding) {
                isExploding = true; // Начинаем анимацию взрыва
                currentFrame = 0; // Сбрасываем кадр для анимации взрыва
                position.x -= 32;
                position.y -= 32;
            }
        }
        move(); // Двигаем гранату
    }

    void updateAnimation(float deltaTime) {
        if (isActive && !isExploding) {
            currentFrame = (currentFrame + 1) % countFrame; // Обновляем кадр гранаты
            textureRec.x = granateTexture.width / countFrame * currentFrame; 
        } else if (isActive && isExploding) {
            explosionTimer += deltaTime; // Увеличиваем таймер для анимации взрыва
            
            if (explosionTimer >= explosionDuration / explosionFrames) { 
                currentFrame++;
                explosionTimer = 0.0f; // Сбрасываем таймер

                if (currentFrame >= explosionFrames - 1) {
                    isActive = false; // Деактивируем гранату после завершения анимации
                }
            }

            textureRec.x = (granateTexture.width / explosionFrames * currentFrame); 
            textureRec.y = granateSize; // Изменяем Y для отображения текстуры взрыва
            textureRec.width = explosionSize; 
            textureRec.height = explosionSize;
            
            hitBox.width = explosionSize; 
            hitBox.height = explosionSize;
        }
    }

    void draw() {
        DrawTextureRec(granateTexture, textureRec, position, WHITE);
        //DrawRectangleRec(hitBox, GREEN);
    }

    bool checkCollision(Rectangle hitBox) {
        return CheckCollisionRecs(hitBox, this->hitBox);
    }
};