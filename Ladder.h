#pragma once
#include "raylib.h"
#include "MapObject.h"

class Ladder : public MapObject {
private:
    Vector2 p1; // Класс, для проверки коллизий
    Vector2 p2;
    Vector2 p3;

public:
    // Конструктор платформы
    Ladder(int _x, int _y, const Vector2 &_p1, const Vector2 &_p2, const Vector2 &_p3) 
        : MapObject(_x, _y), p1(_p1), p2(_p2), p3(_p3) {} // Инициализация базового класса

    // Геттеры для координат
    int getX() const {
        return this->getX();
    }

    int getY() const {
        return this->getY();
    }

    // Реализация проверки коллизий
    bool isCollision(Vector2 p) const {
        return CheckCollisionPointTriangle(p, p1, p2, p3); 
    }
};