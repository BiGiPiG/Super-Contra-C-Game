#include "raylib.h"
#include "Map.h"
#include "Player.h"
#include "Bullet.hpp"
#include "Ledder.hpp"
#include "GameCamera.h"
#include "Alien.h"
#include "GranateThrower.hpp"
#include "Turret.hpp"
#include "TurretBullet.hpp"
#include "Granate.hpp"
#include "LedderBullet.hpp"
#include "SpawnerAliens.hpp"
#include "MainMenu.hpp"
#include "GameOverMenu.hpp"
#include "HeliBoss.hpp"

#include <iostream>
#include <map>
#include <variant>


using BulletVariant = std::variant<std::shared_ptr<LedderBullet>, std::shared_ptr<TurretBullet>, std::shared_ptr<Granate>>;

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
                if (alien->isLookRight()) {
                    alien->runRight(mapObjects);
                } else {
                    alien->runLeft(mapObjects);
                }
                
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
                std::vector<std::shared_ptr<GranateThrower>> &granateThrowers,
                std::vector<std::shared_ptr<Ledder>> &ledders,
                float deltaTime, std::vector<std::shared_ptr<Turret>> &turrets, std::vector<BulletVariant> &allBullets,
                std::unique_ptr<HeliBoss> &boss) {

        updateAliens(aliens, bill, map.getPlatforms());

        updateGranateThrowers(granateThrowers, deltaTime, allBullets, bill);

        updateTurrets(turrets, deltaTime, bill, allBullets);

        updateLedders(ledders, deltaTime, bill, allBullets);


        /*
        if (boss != nullptr && !boss->getIsActive()) {
            boss.reset();
        }

        if (boss != nullptr) {
            boss->update(deltaTime, bill, allBullets, aliens);
            boss->updateAnimation(deltaTime);
        }
        */
        
        for (auto it = aliens.begin(); it != aliens.end();) {
            if (!(*it)->isAlive()) {
                it = aliens.erase(it);
            } else {
                ++it;
            }
        }

        bill.update(map.getPlatforms());
    
    }

    void updateGranateThrowers(std::vector<std::shared_ptr<GranateThrower>> &granateThrowers,
        float deltaTime, std::vector<BulletVariant> &allBullets, Player &player) {

        if (granateThrowers.empty()) {
            return;
        }

        for (auto it = granateThrowers.begin(); it != granateThrowers.end();) {
            (*it)->checkDie(player.getBullets());
            (*it)->update(deltaTime, allBullets, player); // Update turret state
            if (!(*it)->getAlive()) {
                it = granateThrowers.erase(it);
            } else {
                it++;
            }
        }

    }

    void drawGranateThrowers(std::vector<std::shared_ptr<GranateThrower>> &granateThrowers) {

        if (granateThrowers.empty()) {
            return;
        }

        for (auto &granateThrower : granateThrowers) {
            granateThrower->draw();
        }

    }

    void updateLedders(std::vector<std::shared_ptr<Ledder>> &ledders, float deltaTime, 
                       Player &player, std::vector<BulletVariant> &allBullets) {

        if (ledders.empty()) {
            return;
        }

        for (auto it = ledders.begin(); it != ledders.end();) {
            (*it)->checkDie(player.getBullets());
            (*it)->update(player.getPosition(), map.getPlatforms(), deltaTime, allBullets); // Update turret state
            if (!(*it)->getAlive()) {
                it = ledders.erase(it);
            } else {
                it++;
            }
        }

    }

    void drawLedders(std::vector<std::shared_ptr<Ledder>> ledders) {

        if (ledders.empty()) {
            return;
        }

        for (auto &ledder : ledders) {
            ledder->draw();
        }

    }

    void updateTurrets(std::vector<std::shared_ptr<Turret>> &turrets, float deltaTime, 
                       Player &player, std::vector<BulletVariant> &allBullets) {

        if (turrets.empty()) {
            return;
        }

        for (auto it = turrets.begin(); it != turrets.end();) {
            (*it)->checkDie(player.getBullets());
            (*it)->update(deltaTime, player, map.getPlatforms(), allBullets); // Update turret state
            if ((*it)->getDie()) {
                it = turrets.erase(it);
            } else {
                it++;
            }
        }

    }

    void drawTurrets(std::vector<std::shared_ptr<Turret>> &turrets) {
        
        if (turrets.empty()) {
            return;
        }

        for (auto &turret : turrets) {
            turret->draw();
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

    void drawScene(Player& bill, const std::vector<std::shared_ptr<Alien>>& aliens,
               std::vector<std::shared_ptr<GranateThrower>> &granateThrowers,
               std::vector<std::shared_ptr<Ledder>> &ledders,
               GameCamera& gameCamera, std::vector<std::shared_ptr<Turret>> &turrets, std::vector<BulletVariant> &bullets,
               std::unique_ptr<HeliBoss> &boss) {
                
        BeginDrawing();

        BeginMode2D(gameCamera.camera);

        drawMap();

        drawTurrets(turrets);
        drawAliens(aliens);
        drawGranateThrowers(granateThrowers);

        drawLedders(ledders);

        /*
        if (boss != nullptr) {
            boss->draw();
        }
        */

        bill.draw();

        drawBullets(bullets);

        
        
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

    void updateBullets(std::vector<BulletVariant> &bullets, Player &bill) {

        if (bullets.empty()) {
            return;
        }

        for (auto &bullet : bullets) {
            if (auto granate = std::get_if<std::shared_ptr<Granate>>(&bullet)) {
                (*granate)->updateGranate(map.getPlatforms());
            } else if (auto ledderBullet = std::get_if<std::shared_ptr<LedderBullet>>(&bullet)) {
                (*ledderBullet)->update(map.getPlatforms(), bill.getPosition());
            } else if (auto turretBullet = std::get_if<std::shared_ptr<TurretBullet>>(&bullet)) {
                (*turretBullet)->update(map.getPlatforms(), bill.getPosition());
            }
        }
    }

    void drawBullets(std::vector<BulletVariant> &bullets) {

        if (bullets.empty()) {
            return;
        }

        for (auto &bullet : bullets) {
            if (auto granate = std::get_if<std::shared_ptr<Granate>>(&bullet)) {
                (*granate)->draw();
            } else if (auto ledderBullet = std::get_if<std::shared_ptr<LedderBullet>>(&bullet)) {
                (*ledderBullet)->draw();
            } else if (auto turretBullet = std::get_if<std::shared_ptr<TurretBullet>>(&bullet)) {
                
                (*turretBullet)->draw();
            }
        }
    }

    void updateBulletsAnimation(float deltaTime, std::vector<BulletVariant> &bullets) {
        if (bullets.empty()) {
            return;
        }

        for (auto &bullet : bullets) {
            if (auto granate = std::get_if<std::shared_ptr<Granate>>(&bullet)) {
                (*granate)->updateAnimation(deltaTime);
            } else if (auto ledderBullet = std::get_if<std::shared_ptr<LedderBullet>>(&bullet)) {
                (*ledderBullet)->updateAnimation(deltaTime);
            } else if (auto turretBullet = std::get_if<std::shared_ptr<TurretBullet>>(&bullet)) {
                (*turretBullet)->updateAnimation(deltaTime);
            }
        }
    }

    void deleteBullets(std::vector<BulletVariant> &bullets) {

        if (bullets.empty()) {
            return;
        }

        for (auto it = bullets.begin(); it != bullets.end();) {
            if (auto granate = std::get_if<std::shared_ptr<Granate>>(&(*it))) {
                if (!(*granate)->getActive()) {
                    it = bullets.erase(it);
                } else {
                    it++;
                }
            } else if (auto ledderBullet = std::get_if<std::shared_ptr<LedderBullet>>(&(*it))) {
                if (!(*ledderBullet)->isActive) {
                    it = bullets.erase(it);
                } else {
                    it++;
                }
            } else if (auto turretBullet = std::get_if<std::shared_ptr<TurretBullet>>(&(*it))) {
                if (!(*turretBullet)->getAlive()) {
                    it = bullets.erase(it);
                } else {
                    it++;
                }
            }
        }
    }

};

int gameRun() {
    // Initialization
    //--------------------------------------------------------------------------------------   

    int frameDelay = 7; // Задержка между кадрами
    int frameDelayCounter = 0;

    std::vector<BulletVariant> allBullets;

    const int screenWidth = 900;
    const int screenHeight = 700;
    
    Game game("resources/SuperContraMap.png");
    Player bill(0, 200, "resources/Bill.png");

    std::vector<std::shared_ptr<Alien>> aliens;
    std::vector<std::shared_ptr<Ledder>> ledders;
    std::vector<std::shared_ptr<GranateThrower>> granateThrowers;
    std::vector<std::shared_ptr<Turret>> turrets;

    //турели
    turrets.push_back(std::make_shared<Turret>(900.0f, 410.0f, 3.0f));

    //снайперы
    ledders.push_back(std::make_shared<Ledder>(500, 200));

    //метатели
    granateThrowers.push_back(std::make_shared<GranateThrower>(5360, -680));


    //std::unique_ptr<GranateThrower> testThrower = std::make_unique<GranateThrower>(5360, -680, "resources/GranateThrower.png");
    //std::unique_ptr<Ledder> testLedder = std::make_unique<Ledder>(500, 200);
    //std::unique_ptr<Turret> turret = std::make_unique<Turret>(900.0f, 410.0f, 3.0f);
    std::unique_ptr<HeliBoss> testBoss = std::make_unique<HeliBoss>(Vector2 {100, 0});

    SpawnerAliens spawner(2.0f, 0, 200);
    
    GameCamera gameCamera(screenWidth, screenHeight);

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    
    int spawnDelay = 120; // Задержка между спавном инопланетян (в кадрах)
    int spawnCounter = 0; // Счетчик кадров
    int spawnGranates = 0;

    GameOverMenu gameOverMenu;

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

        if (bill.getCOuntLives() <= 0) {
            return gameOverMenu.show();
        }
  
        bill.isPlayerAlive(aliens, allBullets);
        
        game.processInput(bill);
        
        //spawner.update(deltaTime);
        //auto aliens = spawner.getAliens();
        game.deleteBullets(allBullets);

        game.updateGame(bill, aliens, granateThrowers, ledders, deltaTime, turrets, allBullets, testBoss);
        game.updateBullets(allBullets, bill);
        game.updateBulletsAnimation(deltaTime, allBullets);
        
        
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
        game.drawScene(bill, aliens, granateThrowers, ledders, gameCamera, turrets, allBullets, testBoss);
        
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
}

int main(void)
{
    int enter = 1;
    const int screenWidth = 900;
    const int screenHeight = 700;

    InitWindow(screenWidth, screenHeight, "SuperContra");

    MainMenu mainMenu;
    

    while (enter) {
        mainMenu.show();
        enter = gameRun();
    }
    return 0;
}

