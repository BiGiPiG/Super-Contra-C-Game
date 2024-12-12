#pragma once

#include "raylib.h"

#include "LedderBullet.hpp"

#include <variant>
#include <vector>
#include <algorithm>


using BulletVariant = std::variant<std::shared_ptr<LedderBullet>, std::shared_ptr<TurretBullet>, std::shared_ptr<Granate>>;

class HeliGun {
private:
    Vector2 position;
    Vector2 offset;
    Rectangle textureRec;
    Rectangle hitBox;
    Texture2D heliGun;

    bool isActive = false;
    bool isExplosion = false;
    bool lookRight = true;
    bool lookLeft = false;

    float shotTimer = 0.0f;
    float frameSpeed = 0.05f;
    float shotInterval = 0.3f;
    float queueTimer = 0.0f;
    float queueInterval = 2.0f;

    int hp = 20;

public:
    bool isGunActive() {
        return !isExplosion;
    }

    HeliGun(Vector2 startPosition, Vector2 offset) 
        : position(startPosition), offset(offset) {
        heliGun = LoadTexture("resources/HeliGun.png"); // Загрузка текстуры вертолета
        textureRec = { 0, 0, (float)heliGun.width, (float)heliGun.height }; // Инициализация рамки текстуры
        hitBox = { startPosition.x, startPosition.y, (float)heliGun.width, (float)heliGun.height };
    }

    ~HeliGun() {
        UnloadTexture(heliGun);
    }

    void updateAnimation(Vector2 playerPos) {

        if ((textureRec.width < 0 && playerPos.x > position.x) || 
            (textureRec.width > 0 && playerPos.x <= position.x)) {
            textureRec.width = -textureRec.width; // Flip texture
            std::swap(lookLeft, lookRight);
        }
    }

    void updatePos(Vector2 heliPos) {
        position.x = heliPos.x + offset.x;
        position.y = heliPos.y + offset.y;
        hitBox.x = position.x;
        hitBox.y = position.y;
    }

    void draw() {
        DrawTextureRec(heliGun, textureRec, position, WHITE);
    }

    void checkDie(std::vector<std::shared_ptr<Bullet>> bullets) {

        if (isExplosion) {
            return;
        }

        if (!bullets.empty()) {
            for (const auto &bullet : bullets) {
                if (CheckCollisionRecs(hitBox, bullet->hitBox)) {
                    hp--;
                    bullet->isHidden = true;
                    if (hp <= 0) {
                        isExplosion = true;
                    }
                    return;
                }
            }
        }
    }

    void shoot(Vector2 target, std::vector<BulletVariant> &bullets) {
        float bulletPosX = position.x + (lookRight ? 40 : 20);
        float bulletPosY = position.y + 40;
        bullets.push_back(std::make_shared<LedderBullet>(bulletPosX, bulletPosY, target.x + 45, target.y + 60, lookRight));
    }


};