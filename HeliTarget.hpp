#pragma once

#include "raylib.h"

class HeliTarget {
private:
    Vector2 position;
    Vector2 offset;
    Rectangle textureRec;
    Rectangle hitBox;
    Texture2D heliTarget;

    bool isActive = true;

    int currentFrame = 0;
    int countFrames = 3;

    float frameTimer = 0.0f;
    float frameSpeed = 0.05f;

    int hp = 20;

public:
    bool isTargetActive() {
        return isActive;
    }

    HeliTarget(Vector2 startPosition, Vector2 offset) 
        : position(startPosition), offset(offset) {
        heliTarget = LoadTexture("resources/HeliTarget.png"); // Загрузка текстуры вертолета
        textureRec = { 0, 0, (float)heliTarget.width / countFrames, (float)heliTarget.height }; // Инициализация рамки текстуры
        hitBox = { startPosition.x, startPosition.y, (float)heliTarget.width, (float)heliTarget.height };
    }

    ~HeliTarget() {
        UnloadTexture(heliTarget);
    }

    void updateAnimation() {
        currentFrame = (currentFrame + 1) % countFrames;
        textureRec.x = heliTarget.width / countFrames * currentFrame;
    }

    void draw() {
        DrawTextureRec(heliTarget, textureRec, position, WHITE);
    }

    void update(Vector2 heliPos) {
        position.x = heliPos.x + offset.x;
        position.y = heliPos.y + offset.y;
        hitBox.x = position.x;
        hitBox.y = position.y;
    }

    void checkDie(std::vector<std::shared_ptr<Bullet>> bullets) {

        if (!isActive) {
            return;
        }

        if (!bullets.empty()) {
            for (const auto &bullet : bullets) {
                if (CheckCollisionRecs(hitBox, bullet->hitBox)) {
                    hp--;
                    bullet->isHidden = true;
                    if (hp <= 0) {
                        isActive = false;
                    }
                    return;
                }
            }
        }
    }

};