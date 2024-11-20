#pragma once
#include "raylib.h"
#include "MapObject.h"

class Platform : public MapObject {
private:
    Rectangle rect; // Класс, для проверки коллизий

public:
    // Конструктор платформы
    Platform(int _x, int _y, const Rectangle &_rect) 
        : MapObject(_x, _y), rect(_rect) {} // Инициализация базового класса

    // Геттеры для координат
    int getX() const {
        return this->getX();
    }

    int getY() const {
        return this->getY();
    }

    Rectangle getRectangle() const {
        return this->rect;
    }

    // Реализация проверки коллизий
    bool isCollision(const Rectangle &hitBox) const {
        return CheckCollisionRecs(this->rect, hitBox); 
    }
};