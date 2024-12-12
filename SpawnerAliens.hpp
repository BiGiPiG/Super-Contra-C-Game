#pragma once
#include "Alien.h"
#include <vector>
#include <memory>

class SpawnerAliens {
private:
    std::vector<std::shared_ptr<Alien>> aliens; // Вектор для хранения инопланетян
    float spawnInterval; // Интервал спавна инопланетян
    float elapsedTime; // Время, прошедшее с последнего спавна
    float spawnX; // Координата X для спавна
    float spawnY; // Координата Y для спавна

public:
    SpawnerAliens(float interval, float x, float y)
        : spawnInterval(interval), spawnX(x), spawnY(y), elapsedTime(0) {}

    void update(float deltaTime) {
        elapsedTime += deltaTime; // Увеличиваем прошедшее время

        if (elapsedTime >= spawnInterval) {
            // Создаем двух новых инопланетян и добавляем их в вектор
            aliens.push_back(std::make_shared<Alien>(spawnX, spawnY, true));
            aliens.push_back(std::make_shared<Alien>(spawnX + 60, spawnY, true)); // Спавн второго инопланетянина немного правее
            elapsedTime = 0; // Сбрасываем таймер
        }
    }

    const std::vector<std::shared_ptr<Alien>>& getAliens() const {
        return aliens; // Возвращаем вектор инопланетян
    }

    void clearDeadAliens() {
        // Удаляем мертвых инопланетян из вектора
        aliens.erase(std::remove_if(aliens.begin(), aliens.end(),
            [](const std::shared_ptr<Alien>& alien) { return !alien->isAlive(); }), aliens.end());
    }
};

/*
    добавить обновление глобального вектора, выбор направления и сделать очередь
*/