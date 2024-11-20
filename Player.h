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
    bool isJumping = true;
    bool isMoving = false;
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

    bool isOnGround(std::vector<std::shared_ptr<MapObject>> mapObjects) {
        for (auto obj: mapObjects) {
             if (auto platform = dynamic_cast<Platform*>(obj.get()))  {
                
                if (platform->isCollision(hitBox)) {
                    std::cout << "platform" << std::endl;
                    return true;
                }
            } 
        }
        return false;
    }

    bool isOnLadder(std::vector<std::shared_ptr<MapObject>> mapObjects) {
        for (auto obj: mapObjects) {
            if (auto ladder = dynamic_cast<Ladder*>(obj.get())) {
                Vector2 hitBoxCenter = { hitBox.x + hitBox.width, hitBox.y + hitBox.height};
                
                if (ladder->isCollision(hitBoxCenter)) {
                    std::cout << "ladder" << std::endl;
                    return true;
                }
            }
        }
        return false;
    }

    void runLeft(const std::vector<std::shared_ptr<MapObject>> mapObjects) {
        position.x -= velocity.x;
        hitBox.x = 25 + position.x;
        /*
        if (isOnLadder(mapObjects)) {
            isJumping = false;
            position.y += 0.77; // Смещение по Y при нахождении на лестнице
            hitBox.y = position.y; // Обновляем хитбокс
        }
        */
        if (textureRec.width > 0) {
            textureRec.width = -textureRec.width;
        }
    }

    void runRight(const std::vector<std::shared_ptr<MapObject>> mapObjects) {

        position.x += velocity.x;
        hitBox.x = 25 + position.x;
        /*
        if (isOnLadder(mapObjects)) {
            isJumping = false;
            position.y -= 0.77; // Смещение по Y при нахождении на лестнице
            hitBox.y = position.y; // Обновляем хитбокс
        }
        */
        if (textureRec.width < 0) {
            textureRec.width = -textureRec.width;
        }
    }

    void jump(const std::vector<std::shared_ptr<MapObject>> mapObjects) {
        if (!isJumping || isOnLadder(mapObjects)) { // Проверка, может ли игрок прыгнуть
            velocity.y = -jumpHeight; // Устанавливаем вертикальную скорость для прыжка
            isJumping = true;   // Устанавливаем флаг прыжка
        }
    }

    void update(const std::vector<std::shared_ptr<MapObject>>& mapObjects, int currentFrame) {
    // Проверяем, находится ли игрок на лестнице
    if (isOnLadder(mapObjects)) {
        isJumping = false; // Сбрасываем флаг прыжка при нахождении на лестнице
        
        // Автоматическое движение по лестнице
        if (IsKeyDown(KEY_RIGHT)) { // Движение вправо по лестнице
            position.y -= 1; // Смещение вверх по лестнице
        } 
        if (IsKeyDown(KEY_LEFT)) { // Движение влево по лестнице
            position.y += 1; // Смещение вниз по лестнице
        }

        hitBox.y = position.y; // Обновляем хитбокс

        // Проверка на прыжок даже на лестнице
        if (IsKeyPressed(KEY_SPACE)) { // Прыжок при нажатии пробела
            jump(mapObjects);
        }
    } else {
        // Если игрок не на лестнице, проверяем состояние прыжка
        if (isJumping) {
            velocity.y += gravity; // Применяем гравитацию к вертикальной скорости
            position.y += velocity.y;   // Обновляем позицию хитбокса
            hitBox.y = position.y;

            // Проверяем, приземлился ли игрок на платформу или находится на лестнице
            if (isOnGround(mapObjects)) {
                isJumping = false;      // Если да, сбрасываем флаг прыжка
                velocity.y = 0;          // Сбрасываем вертикальную скорость
                
                // Найти платформу, с которой произошла коллизия, и установить позицию Y
                for (auto obj : mapObjects) {
                    if (auto platform = dynamic_cast<Platform*>(obj.get())) {
                        if (platform->isCollision(hitBox)) {
                            position.y = platform->getY() - hitBox.height; // Устанавливаем Y до верхней границы платформы
                            hitBox.y = position.y; // Обновляем хитбокс
                            break; // Выходим из цикла после нахождения первой коллизии
                        }
                    }
                }
            } else if (isOnLadder(mapObjects)) { 
                isJumping = false; 
                position.y += 1;   // Устанавливаем позицию немного ниже, чтобы избежать "залипания"
                hitBox.y = position.y;
            }
        } else {
            if (!isOnGround(mapObjects)) {
                jump(mapObjects);          // Если не на земле и не в прыжке, начинаем падение
            } else {
                velocity.y = 0;               // Если на земле и не в прыжке
            }
        }
    }

    // Обновление текстуры в зависимости от состояния игрока
    textureRec.x = (float)playerTexture.width / countFrameRun * currentFrame; 
    textureRec.y = isJumping ? 128 : 0;   // Обновляем текстуру в зависимости от состояния прыжка
    }

    void draw() const {
        DrawTextureRec(playerTexture, textureRec, position, WHITE); // Отрисовка текстуры игрока на экране
        DrawRectangleRec(hitBox, GREEN);
        
        DrawPixel(static_cast<int>(hitBox.x + hitBox.width - 10), static_cast<int>(hitBox.y + hitBox.height - 30), RED);
    }
};