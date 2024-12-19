#pragma once
#include "Alien.hpp"
#include <vector>
#include <memory>
#include <iostream>

class SpawnerAliens {
private:
    float spawnX; // Координата X для спавна
    float spawnY; // Координата Y для спавна
    float spawnInterval; // Интервал спавна инопланетян
    
    bool isAlive = true;

    bool timeBetween = false;
    int countSpawnAliens = 0;
    int aliensPerSpawn; // Количество инопланетян, которые будут созданы за один раз
    float timeBetweenQueue; // Время между очередями спавна
    float queueElapsedTime; // Время, прошедшее с последнего спавна очереди
    float elapsedTime; // Время, прошедшее с последнего спавна

public:
    SpawnerAliens(float x = 0.0f, float y = 0.0f, float interval = 0.2f, int perSpawn = 2, float queueTime = 3.0f)
        : spawnX(x), spawnY(y), spawnInterval(interval), aliensPerSpawn(perSpawn), timeBetweenQueue(queueTime), queueElapsedTime(0), elapsedTime(0) {}

    bool getIsAlive() {
        return isAlive;
    }

    void update(float deltaTime, std::vector<std::shared_ptr<Alien>> &aliens, Player &player) {

        elapsedTime += deltaTime; // Увеличиваем прошедшее время
        

        bool isInSpawnRange = player.getPosition().x >= spawnX - 500 && player.getPosition().x <= spawnX + 500;
        //std::cout << isInSpawnRange << std::endl;

        if (timeBetween) {
            queueElapsedTime += deltaTime; // Увеличиваем время с последнего спавна очереди
            if (queueElapsedTime >= timeBetweenQueue) {
                countSpawnAliens = 0;
                queueElapsedTime = 0.0f;
            }
        }

        if (isInSpawnRange) {
            if (elapsedTime >= spawnInterval) {

                elapsedTime = 0.0f; // Сбрасываем таймер интервала спавна

                if (countSpawnAliens < aliensPerSpawn) {
                    countSpawnAliens++;
                    bool flag = player.getPosition().x >= spawnX; // Определение направления инопланетян
                    aliens.push_back(std::make_shared<Alien>(spawnX, spawnY, flag)); // Спавн нескольких инопланетян
                    timeBetween = false;
                } else {
                    timeBetween = true;
                }
            }
        }

        if (player.getPosition().x >= spawnX + 500) {
            isAlive = false;
        }
    }
};