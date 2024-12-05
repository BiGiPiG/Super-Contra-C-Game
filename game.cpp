#include "raylib.h"
#include "Map.h"
#include "Player.h"
#include "Bullet.h"
#include "Ledder.hpp"
#include "GameCamera.h"
#include "Alien.h"
#include "GranateThrower.hpp"
#include "Turret.hpp"
#include <iostream>
#include <map>
#include <variant>
#include "SpawnerAliens.hpp"

class Game {
public:
    Map map;

    Game(const std::string& path) : map(path) {}
    
    void drawMap() {
        map.mapDraw();
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

    void processInput(Player& bill) {
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

    // Обновление состояния игры
void updateGame(Player& bill, std::vector<std::shared_ptr<Alien>>& aliens,
                std::unique_ptr<GranateThrower>& testThrower,
                std::unique_ptr<Ledder>& testLedder,
                float deltaTime, std::unique_ptr<Turret>& turret) {
        updateAliens(aliens, bill, map.getPlatforms());

        if (testThrower != nullptr) {
            testThrower->isDie(bill.getBullets());
            testThrower->update(deltaTime);
            testThrower->updateGranates(map.getPlatforms(), deltaTime);
            if (!testThrower->getAlive()) { // Проверяем состояние метателя перед обновлением
                testThrower.reset();
            } 
        }

        if (turret) {
            turret->checkDie(bill.getBullets());
            turret->update(deltaTime, bill, map.getPlatforms()); // Update turret state
            if (turret->getDie()) {
                turret.reset();
            }
        }

        if (testLedder != nullptr) {
            testLedder->isDie(bill.getBullets());
            testLedder->update(bill.getPosition(), map.getPlatforms(), deltaTime);
            if (!testLedder->getAlive()) { // Проверяем состояние метателя перед обновлением
                testLedder.reset();
            } 
        }
        
        for (auto it = aliens.begin(); it != aliens.end();) {
            if (!(*it)->isAlive()) {
                it = aliens.erase(it);
            } else {
                ++it;
            }
        }

        bill.update(map.getPlatforms());
    
    }

    void updateAliensFrames(std::vector<std::shared_ptr<Alien>>& aliens) {

        if (!aliens.empty()) {

            for (const auto &alien : aliens) {
                alien->updateCurrentRunFrame();
                alien->setChangeFrame();
            }
            
        }

    }

    void drawScene(Player& bill, const std::vector<std::shared_ptr<Alien>>& aliens,
               const std::unique_ptr<GranateThrower>& testThrower,
               const std::unique_ptr<Ledder>& testLedder,
               GameCamera& gameCamera, std::unique_ptr<Turret>& turret) {
        BeginDrawing();

        BeginMode2D(gameCamera.camera);

        drawMap();

        if (turret) {
            turret->draw(); // Draw the turret
        }

        bill.draw();

        drawAliens(aliens);

        if (testThrower != nullptr) {
            testThrower->draw();
        }

        if (testLedder != nullptr) {
            testLedder->draw();
        }

        if (turret) {
            turret->drawBullet(); // Draw the turret
        }

        
        
        //DrawTriangleLines(Vector2{3280, 400}, Vector2{4315, 400}, Vector2{4315, -140}, BLUE); 
        DrawTriangleLines( Vector2{4185, 130}, Vector2{4885, 130}, Vector2{4885, -220}, BLUE);
        DrawRectangleLines(4885, -220, 900, 100, BLUE);
        EndMode2D(); 
        EndDrawing();
    }

    void drawAliens(const std::vector<std::shared_ptr<Alien>>& aliens) {

        if (!aliens.empty()) {

            for (const auto &alien : aliens) {
                alien->draw();
            }

        }
    }

};

void gameRun() {
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 900;
    const int screenHeight = 700;

    int frameDelay = 7; // Задержка между кадрами
    int frameDelayCounter = 0;

    std::vector<std::variant<std::shared_ptr<LedderBullet>, std::shared_ptr<TurretBullet>, std::shared_ptr<Granate>> allBullets;

    InitWindow(screenWidth, screenHeight, "SuperContra");
    Game game("resources/SuperContraMap.png");
    Player bill(0, 200, "resources/Bill.png");
    std::vector<std::shared_ptr<Alien>> aliens;
    std::unique_ptr<GranateThrower> testThrower = std::make_unique<GranateThrower>(5360, -680, "resources/GranateThrower.png");
    std::unique_ptr<Ledder> testLedder = std::make_unique<Ledder>(500, 200);
    std::unique_ptr<Turret> turret = std::make_unique<Turret>(900.0f, 390.0f, 3.0f);

    SpawnerAliens spawner(2.0f, 0, 200, "resources/AlienRunner.png");
    
    GameCamera gameCamera(screenWidth, screenHeight);

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    
    int spawnDelay = 120; // Задержка между спавном инопланетян (в кадрах)
    int spawnCounter = 0; // Счетчик кадров
    int spawnGranates = 0;

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
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
        
        game.processInput(bill);
        
        //spawner.update(deltaTime);
        //auto aliens = spawner.getAliens();

        game.updateGame(bill, aliens, testThrower, testLedder, deltaTime, turret);
        
        
        if (spawnGranates >= spawnCounter) {
            spawnGranates = 0;
        }
        
        
        bill.updateAnimation(deltaTime);
        
        gameCamera.setCameraTarget(bill.getPosition().x + bill.getWidth() / 2, bill.getPosition().y + bill.getHeight() / 2 - 140);
        if (frameDelayCounter >= frameDelay) {
        
            bill.addCurFrame();
            game.updateAliensFrames(aliens);
            frameDelayCounter = 0;
            
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        game.drawScene(bill, aliens, testThrower, testLedder, gameCamera, turret);
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
}

int main(void)
{
    gameRun();
    return 0;
}

