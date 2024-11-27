#pragma once
#include "raylib.h"
#include <vector>

class Alien {
private:
    int currentRunFrame = 0;
    int countAnimations = 2;
    int countFrameRun = 6;
    int currentDieFrame = 0;

    Vector2 position; // Позиция инопланетянина
    Rectangle hitBox; // Хитбокс для коллизий
    Rectangle textureRec;
    Texture2D alienTexture; // Текстура инопланетянина
    Vector2 velocity = {6.0f, 0}; // Объединенная скорость (x, y)
    float gravity = 0.5f; // Гравитация

    bool changeFrame = false;
    bool isHidden = false;
    bool isJumping = true; // Флаг прыжка
    bool isActive = true;

public:
    Alien(float startX, float startY, const char* texturePath) {
        position = { startX, startY };
        hitBox = { 25.0f + startX, startY, 50.0f, 128.0f }; // Размер хитбокса
        textureRec = { 0, 0, 101.0f, 128.0f }; 
        alienTexture = LoadTexture(texturePath); // Загрузка текстуры инопланетянина
    }

    bool isAlive() {
        return isActive;
    }

    ~Alien() {
        UnloadTexture(alienTexture); // Освобождение текстуры при уничтожении объекта
    }

    Rectangle getHitBox() {
        return hitBox;
    }

    void setChangeFrame() {
        changeFrame = true;
    }

    void updateCurrentRunFrame() {
        currentRunFrame = (currentRunFrame + 1) % countFrameRun;
    }

    void draw() {
        DrawTextureRec(alienTexture, textureRec, position, WHITE);
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

    void runLeft(const std::vector<std::shared_ptr<MapObject>> &mapObjects) {
        if (isOnGround(mapObjects) && !isOnLadder(mapObjects)) {
            
            position.x -= velocity.x;
            hitBox.x = 25 + position.x;
            
            if (isOnGround(mapObjects)) {
                position.y = isOnGround(mapObjects) - alienTexture.height / countAnimations;
            } else {
                isJumping = true;
            }
            hitBox.y = position.y;
           
        } else if (isOnLadder(mapObjects)) {

            position.x -= velocity.x;
            hitBox.x = 25 + position.x;
            position.y += 2.6; // Смещение по Y при нахождении на лестнице
            hitBox.y = position.y; // Обновляем хитбокс
        } else if (isJumping) {
            position.x -= velocity.x;
            hitBox.x = 25 + position.x;
            position.y += gravity;
            hitBox.y = position.y;
        } else {
            position.y += gravity;
            hitBox.y = position.y;
        }

        if (textureRec.width > 0) {
            textureRec.width = -textureRec.width;
        }
    }

    void runRight(const std::vector<std::shared_ptr<MapObject>> &mapObjects) {
        if (isOnGround(mapObjects) && !isOnLadder(mapObjects)) {
            position.x += velocity.x;
            hitBox.x = 25 + position.x;
            
            if (isOnGround(mapObjects)) {
                position.y = isOnGround(mapObjects) - alienTexture.height / countAnimations;
            } else {
                isJumping = true;
            }
        
            hitBox.y = position.y;

        } else if (isOnLadder(mapObjects)) {

            position.x += velocity.x;
            hitBox.x = 25 + position.x;
            position.y -= 2.6; // Смещение по Y при нахождении на лестнице
            hitBox.y = position.y; // Обновляем хитбокс

        } else if (isJumping) {

            position.x += velocity.x;
            hitBox.x = 25 + position.x;
            position.y += gravity;
            hitBox.y = position.y;

        } else {

            position.y += gravity;
            hitBox.y = position.y;

        }

        if (textureRec.width < 0) {
            textureRec.width = -textureRec.width;
        }
    }

    void jump() {
        if (!isJumping) { // Проверка на возможность прыжка
            velocity.y = -10.0f; // Установка вертикальной скорости для прыжка
            isJumping = true; // Установка флага прыжка
        }
    }

    void updatePhysics(const std::vector<std::shared_ptr<MapObject>> &mapObjects) {
        
        if (isJumping) {

            velocity.y += gravity; // Применяем гравитацию к вертикальной скорости
            position.y += velocity.y; // Обновляем позицию по Y
            hitBox.y = position.y;

            if (isOnGround(mapObjects)) { // Проверка на приземление

                //std::cout << "notJump" << std::endl;
                position.y = isOnGround(mapObjects) - alienTexture.height / countAnimations; // Сброс позиции на землю
                isJumping = false; // Завершение прыжка
                velocity.y = 0.0f; // Сброс скорости прыжка
                hitBox.y = position.y;

            } else if (isOnLadder(mapObjects)) {

                // Если игрок на лестнице, корректируем его позицию
                position.y -= 2.5; // Поднимаем игрока немного выше, чтобы он не проваливался сквозь лестницу
                hitBox.y = position.y; // Обновляем хитбокс
                isJumping = false; 
                velocity.y = 0.0f;

            } 
        } else if (!isOnGround(mapObjects)) {

            if (!isOnLadder(mapObjects)) {
                velocity.y += gravity; // Если не на земле, применяем гравитацию
                position.y += velocity.y;
                hitBox.y = position.y;
            }

            if (isOnGround(mapObjects)) { 

                position.y = isOnGround(mapObjects) - alienTexture.height / countAnimations;
                isJumping = false; 
                velocity.y = 0.0f;

            } else if (isOnLadder(mapObjects)) {

                isJumping = false; 
                velocity.y = 0.0f;

            } 
        } else {
            position.y = isOnGround(mapObjects) - alienTexture.height / countAnimations;
            velocity.y = 0.0f; // Если игрок на земле, сбрасываем вертикальную скорость
        }
        
        hitBox.x = position.x + 25; // Обновление хитбокса по X
    }   

    void updateAnimation() {
        if (isHidden) {
            if (changeFrame) {
                currentDieFrame = (currentDieFrame + 1) % 5;
                if (currentDieFrame) {
                    textureRec.x = (float)alienTexture.width / countFrameRun * currentDieFrame;
                    textureRec.y = (float)alienTexture.height / countAnimations;
                    changeFrame = false;
                } else {
                    isActive = false;
                }
            }
        } else if (isJumping) {
            textureRec.x = 0;
            textureRec.y = (float)alienTexture.height / countAnimations; // Кадр прыжка
        } else {
            textureRec.x = (float)alienTexture.width / countFrameRun * currentRunFrame;
            textureRec.y = (float)alienTexture.height / countAnimations * 0; // Кадр стрельбы или бега
        }
    }

    void checkAlien(std::vector<std::shared_ptr<MapObject>> mapObjects, Vector2 playerPos, const Rectangle &bullet) {

        // Деактивируем пулю, если она выходит за пределы экрана
        if (position.x < 0 || position.x > playerPos.x + GetScreenWidth() || 
            position.y > playerPos.y + GetScreenHeight()) {
            std::cout << "diePos" << std::endl;
            isActive = false;
        }
        if (CheckCollisionRecs(hitBox, bullet)) {
            isHidden = true;
            velocity = {0, 0};
        }
        
    }
};