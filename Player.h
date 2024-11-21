#pragma once
#include "raylib.h"
#include "raymath.h"

class Player {
private:
    int countFrameRun = 6;
    int countFrameJump = 4;
    int countFrame = 6;

    float xVelocity = 5;
    float yVelocity = 0;
    float gravity = 0.5;
    float jumpHeight = 15;

    Vector2 velocity = {5, 0};

    int x;
    int y;
    Rectangle hitBox;
    Rectangle textureRec;
    bool isJumping = false;
    bool isMoving = false;
    bool leftRun = false;
    bool rightRun = false;
    bool fromLadder = false;
    bool fromGravity = false;
    Texture2D playerTexture;
    Vector2 position;



public:
    Player(int startX, int startY, const char* texturePath) : x(startX), y(startY) {
        position.x = startX;
        position.y = startY;
        hitBox = { 25 + startX, startY, 50.0f, 128.0f }; 
        textureRec = { 0, 0, 101.0f, 128.0f };
        playerTexture = LoadTexture(texturePath); // Загрузка текстуры игрока
    }

    void setLeftRun(bool flag) {
        leftRun = flag;
    }

    void setRightRun(bool flag) {
        rightRun = flag;
    }

    int getWidth() {
        return hitBox.width;
    }

    int getHeight() {
        return hitBox.height;
    }

    Vector2 getPosition() {
        return position;
    }

    void setYtextureRecForJump() {
        textureRec.y = 128;
    }

    bool getJumping() {
        return isJumping;
    }

    void setXVelocity(int velocity) {
        xVelocity = velocity;
    }

    void setMoving(bool flag) {
        isMoving = flag;
    }

    bool getMoving() {
        return isMoving;
    }

    ~Player() {
        UnloadTexture(playerTexture);
    }

    int isOnGround(std::vector<std::shared_ptr<MapObject>> mapObjects) {
        for (auto obj: mapObjects) {
             if (auto platform = dynamic_cast<Platform*>(obj.get()))  {
                
                if (platform->isCollision(hitBox)) {
                    //std::cout << "platform" << std::endl;
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

    void runLeft(const std::vector<std::shared_ptr<MapObject>> mapObjects) {

        std::cout << "runl" << std::endl;

        leftRun = true;
        rightRun = false;

        if (isOnGround(mapObjects) && !isOnLadder(mapObjects)) {
            
            position.x -= velocity.x;
            hitBox.x = 25 + position.x;
            
            position.y = isOnGround(mapObjects) - playerTexture.height / 2;
            hitBox.y = position.y;
           
        } else if (isOnLadder(mapObjects)) {

            position.x -= velocity.x;
            hitBox.x = 25 + position.x;
            position.y += 2.5; // Смещение по Y при нахождении на лестнице
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

    void runRight(const std::vector<std::shared_ptr<MapObject>> mapObjects) {

        std::cout << "runr" << std::endl;

        rightRun = true;
        leftRun = false;
        
        if (isOnGround(mapObjects) && !isOnLadder(mapObjects)) {
            position.x += velocity.x;
            hitBox.x = 25 + position.x;

            position.y = isOnGround(mapObjects) - playerTexture.height / 2;
            hitBox.y = position.y;

        } else if (isOnLadder(mapObjects)) {

            position.x += velocity.x;
            hitBox.x = 25 + position.x;
            position.y -= 2.5; // Смещение по Y при нахождении на лестнице
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

    void jump(const std::vector<std::shared_ptr<MapObject>> mapObjects) {
        if (!isJumping && (isOnGround(mapObjects) || isOnLadder(mapObjects))) { // Проверка, может ли игрок прыгнуть
            std::cout << "jump" << std::endl;
            velocity.y = -jumpHeight; // Устанавливаем вертикальную скорость для прыжка
            isJumping = true;   // Устанавливаем флаг прыжка
        }
    }

    void updateAnimation(int &currentFrame) {

        if (isJumping) {
            textureRec.x = (float)playerTexture.width / countFrameRun * currentFrame;
            textureRec.y = (float)playerTexture.height / 2; // Кадр прыжка
        } else if (isMoving) {
            textureRec.x = (float)playerTexture.width / countFrameRun * currentFrame;
            textureRec.y = 0; // Кадр бега
        } else {
            currentFrame = 0; 
            textureRec.x = 0; // Возвращаемся к первому кадру
            textureRec.y = 0; 
        }
    }

    void updatePhysics(const std::vector<std::shared_ptr<MapObject>> &mapObjects) {
        
        if (isJumping) {
            if (isMoving) {
                std::cout << "runExtra" << std::endl;
                /*
                if (leftRun) {
                    position.x -= velocity.x; // Move left
                }
                if (rightRun) {
                    position.x += velocity.x; // Move right
                }
                */
            }
            std::cout << "jumpJump" << std::endl;
            velocity.y += gravity; // Применяем гравитацию к вертикальной скорости
            position.y += velocity.y; // Обновляем позицию по Y
            hitBox.y = position.y;

            if (isOnGround(mapObjects)) { // Проверка на приземление
                std::cout << "notJumpJump" << std::endl;
                position.y = isOnGround(mapObjects) - playerTexture.height / 2; // Сброс позиции на землю
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
                position.y = isOnGround(mapObjects) - playerTexture.height / 2;
                isJumping = false; 
                velocity.y = 0.0f;
            } else if (isOnLadder(mapObjects)) {
                isJumping = false; 
                velocity.y = 0.0f;
            }
            
        } else {
            position.y = isOnGround(mapObjects) - playerTexture.height / 2;
            velocity.y = 0.0f; // Если игрок на земле, сбрасываем вертикальную скорость
        }
        
        hitBox.x = position.x + 25; // Обновление хитбокса по X
    }

    void draw() const {
        DrawTextureRec(playerTexture, textureRec, position, WHITE); // Отрисовка текстуры игрока на экране
        DrawRectangleRec(hitBox, GREEN);
        
        DrawPixel(static_cast<int>(hitBox.x + hitBox.width - 10), static_cast<int>(hitBox.y + hitBox.height - 30), RED);
    }
};