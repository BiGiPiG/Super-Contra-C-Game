#pragma once
#include "raylib.h"
#include <vector>
#include <cmath>

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
    float yShift = 38.0f;

    bool changeFrame = false;
    bool isHidden = false;
    bool isJumping = true; // Флаг прыжка
    bool isActive = true;
    bool lookRight;

public:
    Alien(float startX, float startY, bool flag) {
        position = { startX, startY };
        hitBox = { 25.0f + startX, yShift + startY, 50.0f, 100.0f }; // Размер хитбокса
        textureRec = { 0, 0, 100.0f, 138.0f }; 
        alienTexture = LoadTexture("resources/AlienRunner.png"); // Загрузка текстуры инопланетянина
        lookRight = flag;
    }

    bool isLookRight() {
        return lookRight;
    }

    bool isAlive() {
        return isActive;
    }

    bool getIsHidden() {
        return isHidden;
    }

    ~Alien() {
        UnloadTexture(alienTexture); // Освобождение текстуры при уничтожении объекта
    }

    Rectangle& getHitBox() {
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
        //DrawRectangleRec(hitBox, GREEN);
    }

    int isOnGround(std::vector<std::shared_ptr<MapObject>> &mapObjects) {
        for (auto obj: mapObjects) {
             if (auto platform = dynamic_cast<Platform*>(obj.get()))  {
                
                if (platform->isCollision(hitBox)) {
                    return platform->getRectangle().y;
                }
            } 
        }
        return 0;
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

    void runLeft(std::vector<std::shared_ptr<MapObject>> &mapObjects) {
        if (isOnGround(mapObjects) && !isOnLadder(mapObjects)) {
            
            position.x -= velocity.x;
            hitBox.x = 25 + position.x;
            
            if (isOnGround(mapObjects)) {
                position.y = isOnGround(mapObjects) - alienTexture.height / countAnimations;
            } else {
                isJumping = true;
            }
            hitBox.y = yShift + position.y;
           
        } else if (isOnLadder(mapObjects)) {

            position.x -= velocity.x;
            hitBox.x = 25 + position.x;
            position.y += 3.12; // Смещение по Y при нахождении на лестнице
            hitBox.y = yShift + position.y; // Обновляем хитбокс
        } else if (isJumping) {
            position.x -= velocity.x;
            hitBox.x = 25 + position.x;
            position.y += gravity;
            hitBox.y = yShift + position.y;
        } else {
            position.y += gravity;
            hitBox.y = yShift + position.y;
        }

        if (textureRec.width > 0) {
            textureRec.width = -textureRec.width;
        }
    }

    void runRight(std::vector<std::shared_ptr<MapObject>> &mapObjects) {
        if (isOnGround(mapObjects) && !isOnLadder(mapObjects)) {
            position.x += velocity.x;
            hitBox.x = 25 + position.x;
            
            if (isOnGround(mapObjects)) {
                position.y = isOnGround(mapObjects) - alienTexture.height / countAnimations;
            } else {
                isJumping = true;
            }
        
            hitBox.y = yShift + position.y;

        } else if (isOnLadder(mapObjects)) {

            position.x += velocity.x;
            hitBox.x = 25 + position.x;
            position.y -= 3.12; // Смещение по Y при нахождении на лестнице
            hitBox.y = yShift + position.y; // Обновляем хитбокс

        } else if (isJumping) {

            position.x += velocity.x;
            hitBox.x = 25 + position.x;
            position.y += gravity;
            hitBox.y = yShift + position.y;

        } else {

            position.y += gravity;
            hitBox.y = yShift + position.y;

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

    void updatePhysics(std::vector<std::shared_ptr<MapObject>> &mapObjects) {
        
        if (isJumping) {

            velocity.y += gravity; // Применяем гравитацию к вертикальной скорости
            position.y += velocity.y; // Обновляем позицию по Y
            hitBox.y = yShift + position.y;

            if (isOnGround(mapObjects)) { // Проверка на приземление

                position.y = isOnGround(mapObjects) - alienTexture.height / countAnimations; // Сброс позиции на землю
                isJumping = false; // Завершение прыжка
                velocity.y = 0.0f; // Сброс скорости прыжка
                hitBox.y = yShift + position.y;

            } else if (isOnLadder(mapObjects)) {

                position.y -= 2.5; // Поднимаем немного выше, чтобы он не проваливался сквозь лестницу
                hitBox.y = yShift + position.y; // Обновляем хитбокс
                isJumping = false; 
                velocity.y = 0.0f;

            } 
        } else if (!isOnGround(mapObjects)) {

            if (!isOnLadder(mapObjects)) {
                velocity.y += gravity; // Если не на земле, применяем гравитацию
                position.y += velocity.y;
                hitBox.y = yShift + position.y;
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
            velocity.y = 0.0f; // Если на земле, сбрасываем вертикальную скорость
        }
        
        hitBox.x = position.x + 25; // Обновление хитбокса по X
    }   

    void updateAnimation() {
        if (isHidden) {
            if (changeFrame) {
                currentDieFrame = (currentDieFrame + 1) % 6;
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
            textureRec.y = (float)alienTexture.height / countAnimations; 
        } else {
            textureRec.x = (float)alienTexture.width / countFrameRun * currentRunFrame;
            textureRec.y = (float)alienTexture.height / countAnimations * 0; 
        }
    }

    void checkAlien(std::vector<std::shared_ptr<MapObject>> &mapObjects, Vector2 &playerPos, std::shared_ptr<Bullet> bullet, int &score) {

        if (isHidden) {
            return;
        }

        if (position.x < 0 || abs(position.x - playerPos.x) >= 1000.0f) {
            isActive = false;
        }
        if (CheckCollisionRecs(hitBox, bullet->hitBox)) {
            bullet->isHidden = true;
            isHidden = true;
            velocity = {0, 0};
            score += 500;
        }
        
    }

};