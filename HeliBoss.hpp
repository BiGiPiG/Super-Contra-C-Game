#pragma once

#include "raylib.h"

#include "HeliGun.hpp"
#include "HeliTarget.hpp"

#include <memory>

class HeliBoss {
private:

    Vector2 position;   
    Vector2 startPos;
    float positionDelta = 1800.0f;
    Vector2 velocity = {3.0f, 0.1f};
    bool changeMusic = true;

    Rectangle textureRec;
    Texture2D heli;
    std::vector<std::shared_ptr<HeliGun>> guns;
    std::unique_ptr<HeliTarget> heliTarget;

    bool isActive = true;
    bool isDying = false;
    bool isStage1 = true;

    int countFrames = 3;
    int currantFrame = 0;

    float frameTimer = 0.0f;
    float frameSpeed = 0.05f;

    float shotTimer = 0.0f;
    float timeBetweenShots = 2.0f;
    int currentGunIndex = -1;

    float spawnTimer = 0.0;
    float timeBetweenSpawn = 2.0f;

    void shoot(Vector2 target, std::vector<BulletVariant> &bullets) {

        shotTimer += GetFrameTime();

        if (shotTimer >= timeBetweenShots) {

            currentGunIndex = (currentGunIndex + 1) % guns.size();

            if (!guns.empty()) {
                guns[currentGunIndex]->shoot(target, bullets);
            }
            
            // Переход к следующей пушке
            shotTimer = 0.0f; // Сбрасываем таймер
        }
    }

    void stage2() {
        heliTarget = std::make_unique<HeliTarget>(Vector2 {position.x + 510, position.y + 150}, Vector2{510, 150});
        isStage1 = false;
        
    }

public:
    HeliBoss(Vector2 startPosition) 
        : position(startPosition), startPos(startPosition) {
        heli = LoadTexture("resources/HeliBoss.png"); // Загрузка текстуры вертолета
        textureRec = { 0, 0, (float)heli.width / 2, (float)heli.height / countFrames }; // Инициализация рамки текстуры
        guns.push_back(std::make_shared<HeliGun>(Vector2 {startPosition.x + 346, startPosition.y + 150}, Vector2 {346, 150}));
        guns.push_back(std::make_shared<HeliGun>(Vector2 {startPosition.x + 428, startPosition.y + 150}, Vector2 {428, 150}));
        guns.push_back(std::make_shared<HeliGun>(Vector2 {startPosition.x + 510, startPosition.y + 150}, Vector2 {510, 150}));
        guns.push_back(std::make_shared<HeliGun>(Vector2 {startPosition.x + 595, startPosition.y + 150}, Vector2 {595, 150}));
        
    }

    ~HeliBoss() {
        UnloadTexture(heli);
    }

    bool getIsActive() {
        return isActive;
    }

     void updateAnimation(float deltaTime) {
        frameTimer += deltaTime;
        if (frameTimer >= frameSpeed) {
            frameTimer = 0.0f;
            currantFrame = (currantFrame + 1) % countFrames;
            textureRec.y = heli.height / countFrames * currantFrame;
            if (!isStage1 && heliTarget && heliTarget->isTargetActive()) {
                heliTarget->updateAnimation();
            }
        }
    }

    void update(float deltaTime, Player &player, std::vector<BulletVariant> &bullets,
                std::vector<std::shared_ptr<Alien>> &aliens, Music &music) {

        if (isDying && position.y >= startPos.y + 200) {
            isActive = false;
        }

        if (position.x < startPos.x + positionDelta && 
            position.x >= startPos.x) {
            position.x += velocity.x;
            position.y += velocity.y;
        } else {
            velocity.x = -velocity.x;
            position.x += velocity.x;
            position.y = startPos.y;
        }

        if (!isStage1) {
            if (heliTarget != nullptr) {
                heliTarget->update(position);
                heliTarget->checkDie(player.getBullets());
            }

            if (heliTarget != nullptr && heliTarget->isTargetActive()) {
                spawnTimer += deltaTime;
                if (spawnTimer >= timeBetweenSpawn) {
                    spawnTimer = 0.0f;
                    bool flag = position.x + 260 <= player.getPosition().x ? true : false;
                    aliens.push_back(std::make_shared<Alien>(position.x + 260, position.y + 140, flag));
                }
            } else {
                textureRec.x = heli.width / 2;
                velocity = {0.0f, 1.0f};
                isDying = true;
                if (changeMusic) {
                    StopMusicStream(music); // Остановка текущей музыки
                    UnloadMusicStream(music); // Освобождение памяти текущей музыки
                    music = LoadMusicStream("resources/BossExplosion.ogg"); // Загрузка новой музыки
                    PlayMusicStream(music); // Воспроизведение новой музыки
                    changeMusic = false;
                }
                
            }
            
            return;
        }
        
        if (!guns.empty()) {
            for (auto it = guns.begin(); it != guns.end();) {
                (*it) -> checkDie(player.getBullets());
                if ((*it) -> isGunActive()) {
                    if (position.x < startPos.x + positionDelta && 
                        position.x >= startPos.x) {
                        (*it)->updatePos(position);
                    }
                    (*it)->updateAnimation(player.getPosition()); // Обновление анимации каждого оружия
                    ++it;
                } else {
                    guns.erase(it);
                }
            }
            shoot(player.getPosition(), bullets);
        } else if (isStage1) {
            stage2();
        }
        
    }

    void draw() {
        DrawTextureRec(heli, textureRec, position, WHITE);
        for (auto& gun : guns) {
            if (gun) { // Проверка на nullptr
                gun->draw(); // Обновление анимации каждого оружия
            }
        }

        if (!isStage1 && heliTarget && heliTarget->isTargetActive()) {
            heliTarget->draw(); // Отрисовка цели если она активна
        }
    }
};