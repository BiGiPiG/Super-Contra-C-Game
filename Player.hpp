#pragma once
#include "raylib.h"
#include "raymath.h"
#include "Bullet.hpp"
#include "Alien.hpp"
#include "LedderBullet.hpp"
#include "Granate.hpp"
#include "TurretBullet.hpp"
#include "Bonus.hpp"

#include <variant>

using BulletVariant = std::variant<std::shared_ptr<LedderBullet>, std::shared_ptr<TurretBullet>, std::shared_ptr<Granate>>;

enum TypeOfShooting { SHOOTTYPE_1, SHOOTTYPE_2 };

class Player {
private:

    std::vector<std::shared_ptr<Bullet>> bullets; // Вектор для хранения пуль
    TypeOfShooting typeOfShooting = SHOOTTYPE_1;


    int countFrameRun = 7;
    
    int countFrame = 7;  
    int currentShootFrame = 0;
    int currentRunUpFrame = 0;
    int currentRunDownFrame = 0;
    int currentDieFrame = 0;
    int countLives = 3;
    int deathFrames = 4;

    int currentFrame = 0;
    int countFrames = 7;
    int countAnimations = 7;
    int countRunFrames = 6;
    int countFrameJump = 4;

    enum AnimationState { IDLE, RUNNING, JUMPING, DYING };
    AnimationState currentAnimationState = IDLE; // Текущее состояние анимации
    AnimationState previousAnimationState = IDLE; // Предыдущее состояние анимации

    float gravity = 0.5;
    float jumpHeight = 14;
    float bulletSpeed = 20.0f; // Скорость пулиg
    float frameTimer = 0.0f;
    float frameSpeed = 0.2f;

    Vector2 velocity = {5, 0};

    Rectangle hitBox;
    Rectangle textureRec;

    bool isJumping = false;
    bool isMoving = false;
    bool isShooting = false;
    bool leftRun = false;
    bool rightRun = false;
    bool lookUp = false;
    bool lookDown = false;
    bool isAlive = true;
    bool isDying = false;

    // Переменные для мигания
    float blinkTimer = 0.0f; // Таймер для управления миганием
    float blinkDuration = 0.1f; // Длительность одного мигания
    int blinkCount = 15; // Количество миганий
    int currentBlink = 0; // Текущий счетчик миганий
    bool isBlinking = false; // Флаг, указывающий на то, что игрок мигает


    Texture2D playerTexture;
    Texture2D lifeIcon;
    Vector2 position;

    void updateDeathAnimation() {

        hitBox.width = 50.0f;
        hitBox.height = 100.0f;

        if (frameTimer >= frameSpeed) {
            currentDieFrame++;
            frameTimer = 0.0f;

            if (currentDieFrame < deathFrames) {
                position.x -= 10; // Сдвиг позиции
                position.y -= 10; // Сдвиг позиции
                textureRec.x = (float)playerTexture.width / countFrameRun * currentDieFrame;
                textureRec.y = (float)playerTexture.height / countAnimations * 5; // Кадр смерти
            } else {
                isDying = false; // Завершаем анимацию смерти
                isBlinking = true; // Начинаем мигание
                currentDieFrame = deathFrames - 1; // Останавливаемся на последнем кадре
            }
        }
    }

    void updateJumpAnimation() {

        hitBox.width = 50.0f;
        hitBox.height = 100.0f;

        currentFrame %= countFrameJump; // Обновляем кадр
        textureRec.x = (float)playerTexture.width / countFrames * currentFrame;
        textureRec.y = (float)playerTexture.height / countAnimations * 2; // Кадр прыжка
    }

    void updateMovementAnimation() {

        currentFrame %= countRunFrames; // Обновляем кадр
        hitBox.width = 50.0f;
        hitBox.height = 100.0f;

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

        hitBox.width = 50.0f;
        hitBox.height = 100.0f;

        if (lookUp) {
            textureRec.x = (float)playerTexture.width / countFrames * 2; // Кадр ожидания
            textureRec.y = (float)playerTexture.height / countAnimations * 6;
        } else if (lookDown) {
            hitBox.y = position.y + 100;
            hitBox.x = position.x;
            hitBox.width = 100.0f;
            hitBox.height = 38.0f;
            textureRec.x = (float)playerTexture.width / countFrames * 4; // Кадр ожидания
            textureRec.y = (float)playerTexture.height / countAnimations * 6;
        } else {
            textureRec.x = 0; // Кадр ожидания
            textureRec.y = (float)playerTexture.height / countAnimations * 6;
        }
    }

    void die() {
        isDying = true; // Устанавливаем флаг смерти
        currentBlink = 0; // Сброс счетчика миганий
        typeOfShooting = SHOOTTYPE_1;
    }

    void updateBlinking(float deltaTime) {
        if (isBlinking) {
            blinkTimer += deltaTime;

            if (blinkTimer >= blinkDuration) {
                blinkTimer = 0.0f; // Сброс таймера
                currentBlink++; // Увеличиваем счетчик миганий

                if (currentBlink >= blinkCount) {
                    isBlinking = false; // Завершаем мигание
                }
            }
        }
    }

public:

    Player(float startX, float startY, const char* texturePath) {
        position.x = startX;
        position.y = startY;
        hitBox = { 25.0f + startX, 38.0f + startY, 50.0f, 100.0f }; 
        textureRec = { 0, 0, 100.0f, 138.0f };
        playerTexture = LoadTexture(texturePath); // Загрузка текстуры игрока
        lifeIcon = LoadTexture("resources/life_icon.png");
    }

    std::vector<std::shared_ptr<Bullet>>& getBullets() {
        return bullets;
    }

    int getCOuntLives() {
        return countLives;
    }

    void addCurFrame() {
        currentFrame++;
    }

    void setLookDown(bool flag) {
        lookDown = flag;
    }

    void setLookUp(bool flag) {
        lookUp = flag;
    }

    void setShooting(bool flag) {
        isShooting = flag;
    }

    int getWidth() {
        return hitBox.width;
    }

    int getHeight() {
        return hitBox.height;
    }

    Vector2& getPosition() {
        return position;
    }

    void setMoving(bool flag) {
        isMoving = flag;
    }

    ~Player() {
        UnloadTexture(playerTexture);
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
                    //std::cout << "ladder" << std::endl;
                    return true;
                }
            }
        }
        return false;
    }

    void runLeft(std::vector<std::shared_ptr<MapObject>> &mapObjects, float leftBorder) {

        if (isDying) {
            return;
        }

        leftRun = true;
        rightRun = false;
        float beginPos = position.x;

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

        while (hitBox.x < leftBorder) {
            position.x = beginPos;
            hitBox.x = position.x + 25;
        }

    }

    void runRight(std::vector<std::shared_ptr<MapObject>> &mapObjects, float rightBorder) {

        if (isDying) {
            return;
        }

        rightRun = true;
        leftRun = false;
        float beginPos = position.x;
        
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

        while (hitBox.x > rightBorder) {
            position.x = beginPos;
            hitBox.x = position.x + 25;
        }
    }

    void jump(std::vector<std::shared_ptr<MapObject>> &mapObjects) {

        if (isDying) {
            return;
        }

        if (!isJumping && (isOnGround(mapObjects) || isOnLadder(mapObjects))) { // Проверка, может ли игрок прыгнуть
            velocity.y = -jumpHeight; // Устанавливаем вертикальную скорость для прыжка
            isJumping = true;   // Устанавливаем флаг прыжка
        }
    }

    void updateAnimation(float deltaTime) {
        frameTimer += deltaTime; // Увеличиваем таймер

        if (isBlinking) {
            updateBlinking(deltaTime);
        }

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

    void updatePhysics(std::vector<std::shared_ptr<MapObject>> &mapObjects) {

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

    void draw(Vector2 &pos) const {
        if (isBlinking) {
            // Определяем прозрачность на основе текущего мигания
            Color color = (currentBlink % 2 == 0) ? WHITE : (Color){255, 255, 255, 0}; // Прозрачность на четных кадрах
            
            DrawTextureRec(playerTexture, textureRec, position, color); // Отрисовка с учетом прозрачности
            DrawRectangleRec(hitBox, GREEN);
        } else {
            DrawTextureRec(playerTexture, textureRec, position, WHITE); // Обычная отрисовка
            DrawRectangleRec(hitBox, GREEN);
        }
        drawBullets();
        drawLives(pos);
    }

    void drawLives(Vector2 &pos) const {
        for (int i = 0; i < countLives; i++) {
            DrawTexture(lifeIcon, pos.x - 430 + 10 + i * (lifeIcon.width + 5), pos.y - 320, WHITE); // Отрисовка иконок жизней
        }
    }

    void shoot() {

        if (isDying) {
            return;
        }

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
        if (typeOfShooting == SHOOTTYPE_1) {
            bullets.push_back(std::make_shared<Bullet>(bulletPosX, bulletPosY, bulletDirectionX, bulletDirectionY, 1));
        } else {
            bullets.push_back(std::make_shared<Bullet>(bulletPosX, bulletPosY, bulletDirectionX, bulletDirectionY, 3));
        }
        
    
    }

    void updateBullets(std::vector<std::shared_ptr<MapObject>>& mapObjects) {
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

    void update(std::vector<std::shared_ptr<MapObject>>& mapObjects) {
        updatePhysics(mapObjects); // Обновляем физику (включая прыжки)
        updateBullets(mapObjects); // Обновляем состояние пуль
    }   

    void isPlayerAlive(std::vector<std::shared_ptr<Alien>>  &aliens, 
                        std::vector<BulletVariant> &bullets) {

        if (isBlinking || isDying) {
            return;
        }
        
        if (!aliens.empty()) {
            for (const auto &alien : aliens) {
                if (!alien->getIsHidden() && CheckCollisionRecs(hitBox, alien->getHitBox())) {
                    die();
                    countLives--;
                    return;
                }
            }
        }

        if (!bullets.empty()) {
            for (auto &bullet : bullets) {
                if (auto granate = std::get_if<std::shared_ptr<Granate>>(&bullet)) {
                    if (CheckCollisionRecs(hitBox, (*granate)->hitBox)) {
                        die();
                        countLives--;
                        return;
                    }
                } else if (auto ledderBullet = std::get_if<std::shared_ptr<LedderBullet>>(&bullet)) {
                    if (CheckCollisionRecs(hitBox, (*ledderBullet)->hitBox)) {
                        die();
                        countLives--;
                        return;
                    }
                } else if (auto turretBullet = std::get_if<std::shared_ptr<TurretBullet>>(&bullet)) {
                    if (CheckCollisionRecs(hitBox, (*turretBullet)->hitBox)) {
                        die();
                        countLives--;
                        return;
                    }
                }
            }
        }
    }

    void checkBonus(std::vector<std::shared_ptr<Bonus>> &bonuses) {

        if (bonuses.empty()) {
            return;
        }

        for (auto &bonus : bonuses) {
            if (CheckCollisionRecs(hitBox, (*bonus).getHitBox())) {
                std::cout << "bonus" << std::endl;
                if ((*bonus).getType() == TYPE_1) {
                    countLives++;
                } else {
                    typeOfShooting = SHOOTTYPE_2;
                }
                (*bonus).setAlive();
            }
        }
    }
};