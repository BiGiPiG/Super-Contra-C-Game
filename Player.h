#pragma once
#include "raylib.h"
#include "raymath.h"
#include "Bullet.h"
#include "Alien.h"

class Player {
private:

    std::vector<std::shared_ptr<Bullet>> bullets; // Вектор для хранения пуль

    int countFrameRun = 6;
    int countFrameJump = 4;
    int countFrame = 6;
    int countAnimations = 7;
    int currentShootFrame = 0;
    int currentRunUpFrame = 0;
    int currentRunDownFrame = 0;
    int currentDieFrame = 0;
    int countLifes = 3;


    float xVelocity = 5;
    float yVelocity = 0;
    float gravity = 0.5;
    float jumpHeight = 15;
    float bulletSpeed = 20.0f; // Скорость пулиg

    Vector2 velocity = {5, 0};

    Rectangle hitBox;
    Rectangle textureRec;

    bool isJumping = false;
    bool isMoving = false;
    bool isShooting = false;
    bool leftRun = false;
    bool rightRun = false;
    bool changeFrame = false;
    bool lookUp = false;
    bool lookDown = false;
    bool changeStatShoot = false;
    bool isAlive = true;
    bool isDying = false;


    Texture2D playerTexture;
    Vector2 position;



public:
    Player(float startX, float startY, const char* texturePath) {
        position.x = startX;
        position.y = startY;
        hitBox = { 25.0f + startX, startY, 50.0f, 128.0f }; 
        textureRec = { 0, 0, 101.0f, 128.0f };
        playerTexture = LoadTexture(texturePath); // Загрузка текстуры игрока
    }

    std::vector<std::shared_ptr<Bullet>> getBullets() {
        return bullets;
    }

    void setChangeStatShoot(bool flag) {
        changeStatShoot = flag;
    }

    void setLookDown(bool flag) {
        lookDown = flag;
    }

    void setLookUp(bool flag) {
        lookUp = flag;
    }

    void setChangeFrame(bool flag) {
        changeFrame = flag;
    }

    void setShooting(bool flag) {
        isShooting = flag;
    }

    void addShootingFrame() {
        currentShootFrame++;
    }

    int getShootFrame() {
        return currentShootFrame;
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

    bool getJumping() {
        return isJumping;
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

        leftRun = true;
        rightRun = false;

        if (isOnGround(mapObjects) && !isOnLadder(mapObjects)) {
            
            position.x -= velocity.x;
            hitBox.x = 25 + position.x;
            
            if (isOnGround(mapObjects)) {
                position.y = isOnGround(mapObjects) - playerTexture.height / countAnimations;
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

    void runRight(const std::vector<std::shared_ptr<MapObject>> mapObjects) {

        rightRun = true;
        leftRun = false;
        
        if (isOnGround(mapObjects) && !isOnLadder(mapObjects)) {
            position.x += velocity.x;
            hitBox.x = 25 + position.x;
            
            if (isOnGround(mapObjects)) {
                position.y = isOnGround(mapObjects) - playerTexture.height / countAnimations;
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

    void jump(const std::vector<std::shared_ptr<MapObject>> mapObjects) {
        if (!isJumping && (isOnGround(mapObjects) || isOnLadder(mapObjects))) { // Проверка, может ли игрок прыгнуть
            velocity.y = -jumpHeight; // Устанавливаем вертикальную скорость для прыжка
            isJumping = true;   // Устанавливаем флаг прыжка
        }
    }

    void updateAnimation(int &currentFrame) {
        if (isDying) {
            if (changeFrame) {
                currentDieFrame = (currentDieFrame + 1) % 4;
                if (currentDieFrame) {
                    position.x -= 10;
                    position.y -= 10;
                    textureRec.x = (float)playerTexture.width / countFrameRun * currentDieFrame;
                    textureRec.y = (float)playerTexture.height / countAnimations * 5;
                    changeFrame = false;
                } else {
                    isAlive = false;
                    isDying = false;
                    changeFrame = false;
                }
            }
        } else if (isJumping) {
            textureRec.x = (float)playerTexture.width / countFrameRun * currentFrame;
            textureRec.y = (float)playerTexture.height / countAnimations * 2; // Кадр прыжка
        } else if (lookUp && isMoving) {
            textureRec.y = (float)playerTexture.height / countAnimations * 3; // Кадр RunUp
            textureRec.x = (float)playerTexture.width / countFrameRun * currentRunUpFrame;

            // Обновляем кадр анимации стрельбы
            if (changeFrame) {
                currentRunUpFrame++;
                changeFrame = false;
            }

            if (currentShootFrame >= countFrameRun) { // Если достигли конца анимации
                //isShooting = false; // Завершаем стрельбу
                currentShootFrame = 0; // Сбрасываем кадры стрельбы
            }
        } else if (lookDown && isMoving) {
            std::cout << "RunDown" << std::endl;
            textureRec.y = (float)playerTexture.height / countAnimations * 4; // Кадр RunUp
            textureRec.x = (float)playerTexture.width / countFrameRun * currentRunDownFrame;

            // Обновляем кадр анимации стрельбы
            if (changeFrame) {
                currentRunDownFrame++;
                changeFrame = false;
            }

            if (currentShootFrame >= countFrameRun) { // Если достигли конца анимации
                //isShooting = false; // Завершаем стрельбу
                currentShootFrame = 0; // Сбрасываем кадры стрельбы
            }
        } else if (isShooting && isMoving) {
            //std::cout << "shooting" << std::endl;
            textureRec.y = (float)playerTexture.height / countAnimations; // Кадр стрельбы
            textureRec.x = (float)playerTexture.width / countFrameRun * currentShootFrame;

            // Обновляем кадр анимации стрельбы
            if (changeFrame) {
                currentShootFrame = (currentShootFrame + 1) % countFrameRun;
                changeFrame = false;
            }
        } else if (isMoving) {
            textureRec.x = (float)playerTexture.width / countFrameRun * currentFrame;
            textureRec.y = isShooting ? (float)playerTexture.height / countAnimations : 0; // Кадр стрельбы или бега
        } else if (isShooting) {
        textureRec.y = (float)playerTexture.height / countAnimations * 6; // Кадр стрельбы
        textureRec.x = (float)playerTexture.width / countFrameRun;
        } else {
        isShooting = false;
        currentFrame = 0; 
        textureRec.x = 0; 
        textureRec.y = 0; 
        }
    }

    void updatePhysics(const std::vector<std::shared_ptr<MapObject>> &mapObjects) {

        //std::cout << velocity.y << " " << position.y << std::endl;

        if (isJumping) {

            velocity.y += gravity; // Применяем гравитацию к вертикальной скорости
            position.y += velocity.y; // Обновляем позицию по Y
            hitBox.y = position.y;

            if (isOnGround(mapObjects)) { // Проверка на приземление

                position.y = isOnGround(mapObjects) - playerTexture.height / countAnimations; // Сброс позиции на землю
                isJumping = false; // Завершение прыжка
                isShooting = false; // Завершение стрельбы
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
                //std::cout << "falling" << std::endl;
                velocity.y += gravity; // Если не на земле, применяем гравитацию
                position.y += velocity.y;
                //std::cout << velocity.y << " " << position.y << std::endl;
                hitBox.y = position.y;
            }

            if (isOnGround(mapObjects)) { 
                //std::cout << "Notfalling" << std::endl;
                position.y = isOnGround(mapObjects) - playerTexture.height / countAnimations;
                isJumping = false; 
                velocity.y = 0.0f;
                //std::cout << velocity.y << " " << position.y << std::endl;
            } else if (isOnLadder(mapObjects)) {
                isJumping = false; 
                velocity.y = 0.0f;
            } 
        } else {
            position.y = isOnGround(mapObjects) - playerTexture.height / countAnimations;
            velocity.y = 0.0f; // Если игрок на земле, сбрасываем вертикальную скорость
        }
        
        hitBox.x = position.x + 25; // Обновление хитбокса по X
    }

    void draw() const {
        DrawTextureRec(playerTexture, textureRec, position, WHITE); // Отрисовка текстуры игрока на экране
        //DrawRectangleRec(hitBox, GREEN);
        drawBullets();
    }

    void shoot() {
        float bulletDirectionX = rightRun ? bulletSpeed : -bulletSpeed; // Направление в зависимости от движения игрока
        float bulletDirectionY = lookUp ? -bulletSpeed : lookDown ? bulletSpeed : 0;
        bullets.push_back(std::make_shared<Bullet>(position.x + (rightRun ? getWidth() + 15 : 15), position.y + getHeight() / countAnimations - 5, bulletDirectionX, bulletDirectionY));
    
    }

    void updateBullets(const std::vector<std::shared_ptr<MapObject>>& mapObjects) {
        for (auto it = bullets.begin(); it != bullets.end();) {
            (*it)->update(mapObjects, position);
            if (!(*it)->isActive) {
                it = bullets.erase(it); // Удаляем неактивные пули
            } else {
                ++it;
            }
        }
    }

    void drawBullets() const {
        for (const auto& bullet : bullets) {
            if (bullet->isHidden) {
                bullet->currentFrameShoot = (bullet->currentFrameShoot + 1) % bullet->countFrame;
            }
            bullet->draw();
        }
    }

    void update(const std::vector<std::shared_ptr<MapObject>>& mapObjects) {
        updatePhysics(mapObjects); // Обновляем физику (включая прыжки)
        updateBullets(mapObjects); // Обновляем состояние пуль
    }   

    void isPlayerAlive(std::vector<std::shared_ptr<Alien>>& aliens) {
        
        if (!aliens.empty()) {
            for (const auto &alien : aliens) {
                if (CheckCollisionRecs(hitBox, alien->getHitBox())) {
                    isDying = true;
                }
            }
        }
    }
};