#include "raylib.h"
#include "Map.h"
#include "Player.h"
#include "Bullet.h"
#include "Ledder.hpp"
#include "GameCamera.h"
#include "Alien.h"
#include "GranateThrower.hpp"
#include <iostream>
#include <map>

class Game {
private:
    Map map;
public:
    Game(const std::string& path) : map(path) {
        std::cout << "start" << std::endl;
    }
    
    void drawMap() {
        map.mapDraw();
    }

    void run() {
        const int screenWidth = 900;
        const int screenHeight = 700;

        int frameDelay = 7; // Задержка между кадрами
        int frameDelayCounter = 0;

        std::cout << "run" << std::endl;
        InitWindow(screenWidth, screenHeight, "SuperContra");
        std::cout << "init" << std::endl;
        Player bill(0, 200, "resources/Bill.png");
        std::vector<std::shared_ptr<Alien>> aliens;
        std::unique_ptr<GranateThrower> testThrower = std::make_unique<GranateThrower>(5360, -680, "resources/GranateThrower.png");
        std::unique_ptr<Ledder> testLedder = std::make_unique<Ledder>(500, 200);
        
        GameCamera gameCamera(screenWidth, screenHeight);

        SetTargetFPS(60);

        int spawnDelay = 120; // Задержка между спавном инопланетян (в кадрах)
        int spawnCounter = 0; // Счетчик кадров

        while (!WindowShouldClose()) {

            float deltaTime = GetFrameTime();
            frameDelayCounter++;

            spawnCounter++;
            if (spawnCounter >= spawnDelay) {
                //aliens.push_back(std::make_shared<Alien>(bill.getPosition().x - 500, bill.getPosition().y, "resources/AlienRunner.png"));
                spawnCounter = 0; // Сброс счетчика
            }

            if (testLedder != nullptr) {
                bill.isPlayerAlive(aliens, testLedder->getBullets());
            }

            handlePlayerMovement(bill);

            updateAliens(aliens, bill, map.getPlatforms());
            updateGameObjects(testThrower, testLedder, bill);

            bill.update(map.getPlatforms());
            bill.updateAnimation(deltaTime);

            if (frameDelayCounter >= frameDelay) {
        
                bill.addCurFrame();

                updateAliensFrames(aliens);
                frameDelayCounter = 0;
            
            }

            gameCamera.setCameraTarget(bill.getPosition().x + bill.getWidth() / 2, 
                                       bill.getPosition().y + bill.getHeight() / 2 - 140);

            // Draw
            BeginDrawing();
            BeginMode2D(gameCamera.camera);
            drawMap();
            bill.draw();
            drawAliens(aliens);
            if (testThrower != nullptr) {
                testThrower->draw();
            }
            if (testLedder != nullptr) {
                testLedder->draw();
            }
            EndMode2D(); 
            EndDrawing();
        }
        std::cout << "end" << std::endl;
    }

    void updateGameObjects(std::unique_ptr<GranateThrower>& testThrower,
                           std::unique_ptr<Ledder>& testLedder,
                           Player& bill) {
        if (testThrower != nullptr) {
            testThrower->isDie(bill.getBullets());
            testThrower->update(GetFrameTime());
            testThrower->updateGranates(map.getPlatforms(), GetFrameTime());
            
            if (!testThrower->getAlive()) { 
                testThrower.reset(); // Reset if not alive
            } 
        }

        if (testLedder != nullptr) {
            testLedder->isDie(bill.getBullets());
            testLedder->update(bill.getPosition(), map.getPlatforms(), GetFrameTime());
            
            if (!testLedder->getAlive()) { 
                testLedder.reset(); // Reset if not alive
            } 
        }
    }

    void handlePlayerMovement(Player& bill) {
        if (IsKeyPressed(KEY_SPACE)) {
            bill.jump(map.getPlatforms());
        }

        if (IsKeyPressed(KEY_F)) { // Если нажата клавиша A для прыжка
            bill.shoot(); // Вызываем метод прыжка у игрока
            bill.setShooting(true);
            //bill.setShooting(false);
        } 

        if (IsKeyDown(KEY_UP)) {
            bill.setLookUp(true);
        } else {
            bill.setLookUp(false);
        }

        if (IsKeyDown(KEY_DOWN)) {
            bill.setLookDown(true);
        } else {
            bill.setLookDown(false);
        }
        
        if (IsKeyDown(KEY_RIGHT)) {
            bill.runRight(map.getPlatforms());
            bill.setMoving(true);
        } else if (IsKeyDown(KEY_LEFT)) {
            bill.runLeft(map.getPlatforms());
            bill.setMoving(true);
        } else {
            bill.setMoving(false);
        }
    }

    void updateAliens(std::vector<std::shared_ptr<Alien>>& aliens, Player& bill, 
                  const std::vector<std::shared_ptr<MapObject>>& mapObjects) {

        if (!aliens.empty()) {

            for (const auto &alien : aliens) {
                alien->runRight(mapObjects);
                alien->updatePhysics(mapObjects);
                for (const auto &bullet : bill.getBullets()) {
                    alien->checkAlien(mapObjects, bill.getPosition(), bullet->hitBox);
                }
                alien->updateAnimation();
            }

        }
    }

    void updateAliensFrames(std::vector<std::shared_ptr<Alien>>& aliens) {

        if (!aliens.empty()) {

            for (const auto &alien : aliens) {
                alien->updateCurrentRunFrame();
                alien->setChangeFrame();
            }
            
        }
    }

    void drawAliens(std::vector<std::shared_ptr<Alien>>& aliens) {

        if (!aliens.empty()) {

            for (const auto &alien : aliens) {
                alien->draw();
            }
        }
    }
};

int main(void)
{
    std::cout << "run0" << std::endl;
    Game game("resources/SuperContraMap.png");
    std::cout << "run" << std::endl;
    //game.run(); // Start the game's run method

    return 0;
}

