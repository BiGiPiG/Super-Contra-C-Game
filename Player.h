#pragma once
#include "raylib.h"
#include "raymath.h"
#include "Bullet.h"
#include "Alien.h"
#include "LedderBullet.hpp"

class Player {
private:

    std::vector<std::shared_ptr<Bullet>> bullets; // Вектор для хранения пуль

    int countFrameRun = 7;
    
    int countFrame = 7;  
    int currentShootFrame = 0;
    int currentRunUpFrame = 0;
    int currentRunDownFrame = 0;
    int currentDieFrame = 0;
    int countLifes = 3;
    int deathFrames = 4;


    int currentFrame = 0;
    int countFrames = 7;
    int countAnimations = 7;
    int countRunFrames = 6;
    int countFrameJump = 4;
    int countStatShoot = 2;

    enum AnimationState { IDLE, RUNNING, JUMPING, DYING };
    AnimationState currentAnimationState = IDLE; // Текущее состояние анимации
    AnimationState previousAnimationState = IDLE; // Предыдущее состояние анимации

    float xVelocity = 5;
    float yVelocity = 0;
    float gravity = 0.5;
    float jumpHeight = 15;
    float bulletSpeed = 20.0f; // Скорость пулиg
    float frameTimer = 0.0f;
    float frameSpeed = 0.1f;

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

    void updateDeathAnimation() {

        if (frameTimer >= frameSpeed) {
            currentDieFrame++;
            frameTimer = 0.0f;

            if (currentDieFrame < deathFrames) {
                position.x -= 10; // Сдвиг позиции
                position.y -= 10; // Сдвиг позиции
                textureRec.x = (float)playerTexture.width / countFrameRun * currentDieFrame;
                textureRec.y = (float)playerTexture.height / countAnimations * 5; // Кадр смерти
            } else {
                isAlive = false; // Устанавливаем флаг мертвого состояния
                isDying = false; // Завершаем анимацию смерти
                currentDieFrame = deathFrames - 1; // Останавливаемся на последнем кадре
            }
        }
    }

    void updateJumpAnimation() {
        currentFrame %= countFrameJump; // Обновляем кадр
        textureRec.x = (float)playerTexture.width / countFrames * currentFrame;
        textureRec.y = (float)playerTexture.height / countAnimations * 2; // Кадр прыжка
    }

    void updateMovementAnimation() {

        currentFrame %= countRunFrames; // Обновляем кадр

        if (lookUp) {
            textureRec.x = (float)playerTexture.width / countFrames * currentFrame;
            textureRec.y = (float)playerTexture.height / countAnimations * 3; // Кадр runUp
        } else if (lookDown) {
            textureRec.x = (float)playerTexture.width / countFrames * currentFrame;
            textureRec.y = (float)playerTexture.height / countAnimations * 4; // Кадр runDown
        } else {
            if (isShooting) {
                textureRec.x = (float)playerTexture.width / countFrames * currentFrame;
                textureRec.y = (float)playerTexture.height / countAnimations; // Кадр runShoot
            } else {
                textureRec.x = (float)playerTexture.width / countFrames * currentFrame;
                textureRec.y = 0; // Кадр run
            }
        }    
    }

    void updateIdleAnimation() {
        if (lookUp) {
            textureRec.x = (float)playerTexture.width / countFrames * 2; // Кадр ожидания
            textureRec.y = (float)playerTexture.height / countAnimations * 6;
        } else if (lookDown) {
            textureRec.x = (float)playerTexture.width / countFrames * 4; // Кадр ожидания
            textureRec.y = (float)playerTexture.height / countAnimations * 6;
        } else {
            textureRec.x = 0; // Кадр ожидания
            textureRec.y = (float)playerTexture.height / countAnimations * 6;
        }
    }

public:

    Player(float startX, float startY, const char* texturePath) {
        position.x = startX;
        position.y = startY;
        hitBox = { 25.0f + startX, 38.0f + startY, 50.0f, 100.0f }; 
        textureRec = { 0, 0, 100.0f, 138.0f };
        playerTexture = LoadTexture(texturePath); // Загрузка текстуры игрока
    }

    std::vector<std::shared_ptr<Bullet>> getBullets() {
        return bullets;
    }

    void addCurFrame() {
        currentFrame++;
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
            
            hitBox.y = 38.0f + position.y;
           
        } else if (isOnLadder(mapObjects)) {

            position.x -= velocity.x;
            hitBox.x = 25 + position.x;
            position.y += 2.6; // Смещение по Y при нахождении на лестнице
            hitBox.y = 38.0f + position.y; // Обновляем хитбокс
        } else if (isJumping) {
            position.x -= velocity.x;
            hitBox.x = 25 + position.x;
            position.y += gravity;
            hitBox.y = 38.0f + position.y;
        } else {
            position.y += gravity;
            hitBox.y = 38.0f + position.y;
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
        
            hitBox.y = 38.0f + position.y;

        } else if (isOnLadder(mapObjects)) {

            position.x += velocity.x;
            hitBox.x = 25 + position.x;
            position.y -= 2.6; // Смещение по Y при нахождении на лестнице
            hitBox.y = 38.0f + position.y; // Обновляем хитбокс

        } else if (isJumping) {

            position.x += velocity.x;
            hitBox.x = 25 + position.x;
            position.y += gravity;
            hitBox.y = 38.0f + position.y;

        } else {

            position.y += gravity;
            hitBox.y = 38.0f + position.y;

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

    void updateAnimation(float deltaTime) {
        frameTimer += deltaTime; // Увеличиваем таймер

        // Проверяем состояние смерти
        if (isDying) {
            if (currentAnimationState != DYING) {
                previousAnimationState = currentAnimationState; // Обновляем предыдущее состояние
                currentAnimationState = DYING; // Обновляем текущее состояние
                currentDieFrame = 0; // Сбросить к началу анимации смерти
            }
            updateDeathAnimation();
            return;
        }

        // Проверяем состояние прыжка
        if (isJumping) {
            if (currentAnimationState != JUMPING) {
                previousAnimationState = currentAnimationState; // Обновляем предыдущее состояние
                currentAnimationState = JUMPING; // Обновляем текущее состояние
                currentFrame = 0; // Сбросить к началу прыжка
            }
            updateJumpAnimation();
            return;
        }

        // Проверяем состояние движения
        if (isMoving) {
            if (lookUp) {
                if (currentAnimationState != RUNNING) {
                    previousAnimationState = currentAnimationState; // Обновляем предыдущее состояние
                    currentAnimationState = RUNNING; // Обновляем текущее состояние
                    currentRunUpFrame = 0; // Сбросить к началу анимации движения вверх
                }
                updateMovementAnimation();
            } else if (lookDown) {
                if (currentAnimationState != RUNNING) {
                    previousAnimationState = currentAnimationState; // Обновляем предыдущее состояние
                    currentAnimationState = RUNNING; // Обновляем текущее состояние
                    currentRunDownFrame = 0; // Сбросить к началу анимации движения вниз
                }
                updateMovementAnimation();
            } else {
                if (currentAnimationState != RUNNING) {
                    previousAnimationState = currentAnimationState; // Обновляем предыдущее состояние
                    currentAnimationState = RUNNING; // Обновляем текущее состояние
                    currentFrame = 0; // Сбросить к началу бега
                }
                updateMovementAnimation();
            }
        } else {
            if (currentAnimationState != IDLE) {
                previousAnimationState = currentAnimationState; // Обновляем предыдущее состояние
                currentAnimationState = IDLE; // Обновляем текущее состояние
                currentFrame = 0; // Сбросить к началу состояния без движения
            }
            updateIdleAnimation(); // Обновление анимации без движения
        }
    }

/*
    void updateAnimation() {
    // Обновление анимации смерти
    currentFrame++;
    if (isDying) {
        frameTimer += GetFrameTime(); // Увеличиваем таймер

        if (frameTimer >= frameSpeed) { 
            currentDieFrame++;
            frameTimer = 0.0f;

            if (currentDieFrame < deathFrames) {
                position.x -= 10; // Сдвиг позиции
                position.y -= 10; // Сдвиг позиции
                textureRec.x = (float)playerTexture.width / countFrameRun * currentDieFrame;
                textureRec.y = (float)playerTexture.height / countAnimations * 5; // Кадр смерти
            } else {
                isAlive = false; // Устанавливаем флаг мертвого состояния
                isDying = false; // Завершаем анимацию смерти
                currentDieFrame = deathFrames - 1; // Останавливаемся на последнем кадре
            }
        }
        return; // Прекращаем выполнение функции, если метатель мертв
    }

    // Обновление анимации прыжка
    if (isJumping) {
        currentFrame %= countFrameJump;
        textureRec.x = (float)playerTexture.width / countFrames * currentFrame;
        textureRec.y = (float)playerTexture.height / countAnimations * 2; // Кадр прыжка
        return;
    }

    if (isMoving) {
        if (lookDown) {
            currentFrame %= countRunFrames;
            textureRec.x = (float)playerTexture.width / countFrames * currentFrame;
            textureRec.y = (float)playerTexture.height / countAnimations * 4; // Кадр runDown
        } else if (lookUp) {
            currentFrame %= countRunFrames;
            textureRec.x = (float)playerTexture.width / countFrames * currentFrame;
            textureRec.y = (float)playerTexture.height / countAnimations * 3; // Кадр runUp
        } else {
            if (isShooting) {
                currentFrame %= countRunFrames;
                textureRec.x = (float)playerTexture.width / countFrames * currentFrame;
                textureRec.y = (float)playerTexture.height / countAnimations; // Кадр runShoot
            } else {
                currentFrame %= countRunFrames;
                textureRec.x = (float)playerTexture.width / countFrames * currentFrame;
                textureRec.y = 0; // Кадр run
            }
        }
    } else if (lookDown) {
        if (isShooting) {
            currentFrame %= countStatShoot;
            textureRec.x = 4 + (float)playerTexture.width / countFrames * currentFrame; //кадр StatDownShoot
            textureRec.y = (float)playerTexture.height / countAnimations * 6;
        } else {
            textureRec.x = (float)playerTexture.width / countFrames * 4;
            textureRec.y = (float)playerTexture.height / countAnimations * 6; // Кадр down   
        }
    } else if (lookUp) {
        if (isShooting) {
            currentFrame %= countStatShoot;
            textureRec.x = 2 + (float)playerTexture.width / countFrames * currentFrame; //кадр StatUpShoot
            textureRec.y = (float)playerTexture.height / countAnimations * 6;
        } else {
            textureRec.x = (float)playerTexture.width / countFrames * 2;
            textureRec.y = (float)playerTexture.height / countAnimations * 6; // Кадр up
        }
    } else {
        if (isShooting) {
            currentFrame %= countStatShoot;
            textureRec.x = (float)playerTexture.width / countFrames * currentFrame; //кадр StatShoot
            textureRec.y = (float)playerTexture.height / countAnimations * 6;
        } else {
            textureRec.x = 0;
            textureRec.y = (float)playerTexture.height / countAnimations * 6; // Кадр up
        }
    }
}   
*/
    void updatePhysics(const std::vector<std::shared_ptr<MapObject>> &mapObjects) {

        //std::cout << velocity.y << " " << position.y << std::endl;

        if (isJumping) {

            velocity.y += gravity; // Применяем гравитацию к вертикальной скорости
            position.y += velocity.y; // Обновляем позицию по Y
            hitBox.y = 38.0f + position.y;

            if (isOnGround(mapObjects)) { // Проверка на приземление

                position.y = isOnGround(mapObjects) - playerTexture.height / countAnimations; // Сброс позиции на землю
                isJumping = false; // Завершение прыжка
                isShooting = false; // Завершение стрельбы
                velocity.y = 0.0f; // Сброс скорости прыжка
                hitBox.y = 38.0f + position.y;
            } else if (isOnLadder(mapObjects)) {
                // Если игрок на лестнице, корректируем его позицию
                position.y -= 2.5; // Поднимаем игрока немного выше, чтобы он не проваливался сквозь лестницу
                hitBox.y = 38.0f + position.y; // Обновляем хитбокс
                isJumping = false; 
                velocity.y = 0.0f;
            }
        } else if (!isOnGround(mapObjects)) {
            if (!isOnLadder(mapObjects)) {
                //std::cout << "falling" << std::endl;
                velocity.y += gravity; // Если не на земле, применяем гравитацию
                position.y += velocity.y;
                //std::cout << velocity.y << " " << position.y << std::endl;
                hitBox.y = 38.0f + position.y;
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
        float bulletDirectionX;
        float bulletDirectionY;
        float bulletPosX;
        float bulletPosY;
        if (isMoving) {
            bulletDirectionX = rightRun ? bulletSpeed : -bulletSpeed;
            if (lookUp) {
                bulletDirectionY = -bulletSpeed;
                bulletPosX = rightRun ? position.x + 50 : position.x + 20;
                bulletPosY = position.y + 38;
            } else if (lookDown) {
                bulletDirectionY = bulletSpeed;
                bulletPosX = rightRun ? position.x + 50 : position.x + 20;
                bulletPosY = position.y + 58;
            } else {
                bulletDirectionY = 0;
                bulletPosX = rightRun ? position.x + 60 : position.x + 10;
                bulletPosY = position.y + 55;
            }
        } else {
            if (lookUp) {
                bulletDirectionX = 0;
                bulletDirectionY = -bulletSpeed;
                bulletPosX = rightRun ? position.x + 40 : position.x + 30;
                bulletPosY = position.y;
            } else if (lookDown) {
                bulletDirectionX = rightRun ? bulletSpeed : -bulletSpeed;
                bulletDirectionY = 0;
                bulletPosX = rightRun ? position.x + 75 : position.x;
                bulletPosY = position.y + 95;
            } else {
                bulletDirectionX = rightRun ? bulletSpeed : -bulletSpeed;
                bulletDirectionY = 0;
                bulletPosX = rightRun ? position.x + 60 : position.x + 10;
                bulletPosY = position.y + 55;
            }
        }
        //float bulletDirectionY = lookUp ? -bulletSpeed : lookDown ? bulletSpeed : 0;
        bullets.push_back(std::make_shared<Bullet>(bulletPosX, bulletPosY, bulletDirectionX, bulletDirectionY));
    
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

    void isPlayerAlive(std::vector<std::shared_ptr<Alien>>& aliens, 
                        std::vector<std::shared_ptr<LedderBullet>> ledderBullets) {
        
        if (!aliens.empty()) {
            for (const auto &alien : aliens) {
                if (CheckCollisionRecs(hitBox, alien->getHitBox())) {
                    isDying = true;
                    return;
                }
            }
        }

        if (!ledderBullets.empty()) {
            for (const auto &bullet : ledderBullets) {
                if (CheckCollisionRecs(hitBox, bullet->hitBox)) {
                    isDying = true;
                    countLifes -= 1;
                    return;
                }
            }
        }
    }
};