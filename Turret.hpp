#pragma once

#include "raylib.h"

#include "TurretBullet.hpp"
#include "Player.hpp"
#include "LedderBullet.hpp"
#include "Granate.hpp"

#include <vector>
#include <memory>
#include <variant>


using BulletVariant = std::variant<std::shared_ptr<LedderBullet>, std::shared_ptr<TurretBullet>, std::shared_ptr<Granate>>;

class Turret {
private:

    Vector2 position; // Позиция турели
    Texture2D turretTexture; // Текстура турели
    Rectangle hitBox; 
    Rectangle textureRec;

    float fireRate; // Скорость стрельбы (время между выстрелами)
    float elapsedTime; // Время, прошедшее с последнего выстрела
    float appearanceTime = 1; // Время для анимации появления
    float appearanceElapsedTime = 0.0f; // Прошедшее время для анимации появления
    float explosionTime = 1; // Время для анимации взрыва
    float explosionElapsedTime = 0.0f; // Прошедшее время для анимации взрыва

    bool isActive = false;
    bool isAppearing = false;
    bool isExplosion = false;
    bool isDie = false;

    int currentAppearanceFrame = -1; // Текущий кадр анимации появления
    const int appearanceFrameCount = 2; // Количество кадров анимации появления

    int currentExplosionFrame = 1; // Текущий кадр анимации взрыва
    const int explosionFrameCount = 4; // Количество кадров анимации взрыва

    int hp = 100;

public:
    Turret(float x, float y, float rate)
        : position({ x, y }), fireRate(rate), elapsedTime(0) {
        turretTexture = LoadTexture("resources/Turret.png");
        hitBox = {0.0f, 0.0f};
        textureRec = {0.0f, 0.0f, 0.0f, 0.0f};
    }

    ~Turret() {
        UnloadTexture(turretTexture); // Освобождение текстуры при уничтожении объекта
    }

    bool getDie() {
        return isDie;
    }

    Rectangle getHitBox() {
        return hitBox;
    }

    void update(float deltaTime, Player& player, 
                std::vector<std::shared_ptr<MapObject>> mapObjects, std::vector<BulletVariant> &bullets) {
        if (isActive && !isAppearing && !isExplosion) {

            elapsedTime += deltaTime;

            if (elapsedTime >= fireRate) {
                shoot(bullets);
                std::cout<< bullets.size();
                elapsedTime = 0; // Сбрасываем таймер
            }

        } else if (isAppearing && !isExplosion) {
            appearanceElapsedTime += deltaTime;
            if (appearanceElapsedTime >= appearanceTime / appearanceFrameCount) {
                currentAppearanceFrame++;
                appearanceElapsedTime = 0;
                textureRec.width = 114.0f;
                textureRec.height = 150.0f;
                textureRec.x = turretTexture.width / 4 * currentAppearanceFrame;
                if (currentAppearanceFrame >= appearanceFrameCount) {

                    isActive = true; // Активируем турель после завершения анимации
                    isAppearing = false;
                    textureRec.x = 114.0f;
                }
                std::cout << currentAppearanceFrame << std::endl;
                
            }
        } else if (isExplosion) {
            explosionElapsedTime += deltaTime;
            if (explosionElapsedTime >= explosionTime / explosionFrameCount) {
                currentExplosionFrame++;
                explosionElapsedTime = 0;
                textureRec.width = 114.0f;
                textureRec.height = 150.0f;
                textureRec.x = turretTexture.width / 4 * currentExplosionFrame;
                if (currentExplosionFrame >= explosionFrameCount) {
                    isDie = true; 
                    isExplosion = false;
                    textureRec.x = 114.0f;
                }
                std::cout << currentExplosionFrame << std::endl;
                
            }
        } else {
            if (isPlayerInRange(player)) {
                appearence();
            }
        }
        
    }

    void draw() const {
        if (isActive || isAppearing) {
            DrawTextureRec(turretTexture, textureRec, position, WHITE);
        }
    }

    void checkDie(std::vector<std::shared_ptr<Bullet>> &bullets, int &score, Vector2 &playerPos) {
        if (isExplosion) {
            return;
        }

        if (!bullets.empty()) {
            for (const auto &bullet : bullets) {
                if (CheckCollisionRecs(hitBox, bullet->hitBox)) {
                    hp -= bullet->damage;
                    bullet->isHidden = true;
                    if (hp <= 0) {
                        isExplosion = true;
                        score += 1500;
                    }
                    if (playerPos.x - position.x >= 600) {
                        isDie = true;
                    }
                    return;
                }
            }
        }
    }

private:
    bool isPlayerInRange(Player& player) const {
        // Проверяем, находится ли игрок в пределах досягаемости турели
        return (player.getPosition().x > position.x - 500 && player.getPosition().x < position.x + 500);
    }

    void shoot(std::vector<BulletVariant> &bullets) {
        Vector2 bulletPosition = { position.x, position.y + turretTexture.height / 2 - 27.0f }; // Позиция снаряда
        bullets.push_back(std::make_shared<TurretBullet>(bulletPosition.x, bulletPosition.y)); // Создаем новый снаряд
    }

    void appearence() {
        isAppearing = true; // Начинаем анимацию появления
        hitBox.x = position.x; // Устанавливаем хитбокс на позицию турели
        hitBox.y = position.y;
        hitBox.width = turretTexture.width / 4;
        hitBox.height = turretTexture.height;
    }
};