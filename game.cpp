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
public:
    Map map;

    Game(const std::string& path) : map(path) {}
    
    void drawMap() {
        map.mapDraw();
    }
};

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

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 900;
    const int screenHeight = 700;

    int frameDelay = 7; // Задержка между кадрами
    int frameDelayCounter = 0;
    int currentFrame = 0;

    InitWindow(screenWidth, screenHeight, "SuperContra");
    Game game("resources/SuperContraMap.png");
    Player bill(0, 200, "resources/Bill.png");
    std::vector<std::shared_ptr<Alien>> aliens;
    std::unique_ptr<GranateThrower> testThrower = std::make_unique<GranateThrower>(5360, -680, "resources/GranateThrower.png");
    std::unique_ptr<Ledder> testLedder = std::make_unique<Ledder>(500, 200);
    
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
        
        
        if (IsKeyPressed(KEY_SPACE)) {
            bill.jump(game.map.getPlatforms());
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
            bill.runRight(game.map.getPlatforms());
            bill.setMoving(true);
        } else if (IsKeyDown(KEY_LEFT)) {
            bill.runLeft(game.map.getPlatforms());
            bill.setMoving(true);
        } else {
            bill.setMoving(false);
        }
        
        updateAliens(aliens, bill, game.map.getPlatforms());
        if (spawnGranates >= spawnCounter) {
            spawnGranates = 0;
        }
        
        if (testThrower != nullptr) {
            testThrower->isDie(bill.getBullets());
            testThrower->update(deltaTime);
            testThrower->updateGranates(game.map.getPlatforms(), deltaTime);
            if (!testThrower->getAlive()) { // Проверяем состояние метателя перед обновлением
                testThrower.reset();
            } 
        }

        if (testLedder != nullptr) {
            testLedder->isDie(bill.getBullets());
            testLedder->update(bill.getPosition(), game.map.getPlatforms(), deltaTime);
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

        bill.update(game.map.getPlatforms());
        bill.updateAnimation(deltaTime);
        
        gameCamera.setCameraTarget(bill.getPosition().x + bill.getWidth() / 2, bill.getPosition().y + bill.getHeight() / 2 - 140);
        if (frameDelayCounter >= frameDelay) {
        
            bill.addCurFrame();

            updateAliensFrames(aliens);
            frameDelayCounter = 0;
            
            if (bill.getJumping()) {    
                currentFrame = (currentFrame + 1) % 4;
            } else if (bill.getMoving()) {
                currentFrame = (currentFrame + 1) % 6;   
            } else {  
                currentFrame = 0;
            }
            
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        BeginMode2D(gameCamera.camera);
        game.drawMap();
        bill.draw();
        drawAliens(aliens);
        if (testThrower != nullptr) {
            testThrower->draw();
        }
        if (testLedder != nullptr) {
            testLedder->draw();
        }
        
        //DrawTriangleLines(Vector2{3280, 400}, Vector2{4315, 400}, Vector2{4315, -140}, BLUE); 
        DrawTriangleLines( Vector2{4185, 130}, Vector2{4885, 130}, Vector2{4885, -220}, BLUE);
        DrawRectangleLines(4885, -220, 900, 100, BLUE);
        EndMode2D(); 
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

      return 0;
}

