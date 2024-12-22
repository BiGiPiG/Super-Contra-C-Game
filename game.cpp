#include "raylib.h"

#include "Map.hpp"
#include "Player.hpp"
#include "Bullet.hpp"
#include "Ledder.hpp"
#include "GameCamera.hpp"
#include "Alien.hpp"
#include "GranateThrower.hpp"
#include "Turret.hpp"
#include "TurretBullet.hpp"
#include "Granate.hpp"
#include "LedderBullet.hpp"
#include "SpawnerAliens.hpp"
#include "MainMenu.hpp"
#include "GameOverMenu.hpp"
#include "HeliBoss.hpp"
#include "Bonus.hpp"

#include <ctime>
#include <cstdlib>
#include <iostream>
#include <map>
#include <variant>


using BulletVariant = std::variant<std::shared_ptr<LedderBullet>, std::shared_ptr<TurretBullet>, std::shared_ptr<Granate>>;

class Game {
public:
    Map map;
    int hiScore = 0;
    int curScore = 0;
    bool bossStage = true;

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
                    alien->checkAlien(mapObjects, bill.getPosition(), bullet, curScore);
                }
                alien->updateAnimation();
            }

        }

    }

    void processInput(Player& bill, float leftBorder, float rightBorder, Sound shootSound) {

        if (IsKeyPressed(KEY_SPACE)) {
            bill.jump(map.getPlatforms());
        }

        if (IsKeyPressed(KEY_F)) { // Если нажата клавиша A для прыжка
            bill.shoot(); // Вызываем метод прыжка у игрока
            PlaySound(shootSound);
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
            bill.runRight(map.getPlatforms(), rightBorder);
            bill.setMoving(true);
        } else if (IsKeyDown(KEY_LEFT)) {
            bill.runLeft(map.getPlatforms(), leftBorder);
            bill.setMoving(true);
        } else {
            bill.setMoving(false);
        }
    }

    // Обновление состояния игры
    void updateGame(Player& bill, std::vector<std::shared_ptr<Alien>>& aliens,
                    std::vector<std::shared_ptr<GranateThrower>> &granateThrowers,
                    std::vector<std::shared_ptr<Ledder>> &ledders, float deltaTime, 
                    std::vector<std::shared_ptr<Turret>> &turrets, std::vector<BulletVariant> &allBullets,
                    std::unique_ptr<HeliBoss> &boss, std::vector<std::shared_ptr<SpawnerAliens>> &spawnersAliens, 
                    GameCamera& gameCamera, std::vector<std::shared_ptr<Bonus>> &bonuses, Music &music) {

            updateSpawners(spawnersAliens, deltaTime, aliens, bill);

            updateAliens(aliens, bill, map.getPlatforms());

            updateGranateThrowers(granateThrowers, deltaTime, allBullets, bill);

            updateTurrets(turrets, deltaTime, bill, allBullets);

            updateLedders(ledders, deltaTime, bill, allBullets);

            updateBonuses(bonuses, deltaTime, bill);

            if (boss == nullptr && bill.getPosition().x >= 10020 && bossStage) {
                boss = std::make_unique<HeliBoss>(Vector2 {8750, -1575});

                // Заменяем музыку при появлении босса
                StopMusicStream(music); // Остановка текущей музыки
                UnloadMusicStream(music); // Освобождение памяти текущей музыки
                music = LoadMusicStream("resources/HeliSound.ogg"); // Загрузка новой музыки
                PlayMusicStream(music); // Воспроизведение новой музыки
            }

            if (boss != nullptr && !boss->getIsActive()) {
                boss.reset();
                gameCamera.rightBorder = 11270;
                bossStage = false;
                curScore += 50000;

                StopMusicStream(music); // Остановка текущей музыки
                UnloadMusicStream(music); // Освобождение памяти текущей музыки
                music = LoadMusicStream("resources/BackgroundMusic.ogg"); // Загрузка обычной музыки
                PlayMusicStream(music); // Воспроизведение обычной музыки
            }

            if (boss != nullptr) {
                boss->update(deltaTime, bill, allBullets, aliens);
                boss->updateAnimation(deltaTime);
            }
            
            for (auto it = aliens.begin(); it != aliens.end();) {
                if (!(*it)->isAlive()) {
                    it = aliens.erase(it);
                } else {
                    ++it;
                }
            }

            bill.update(map.getPlatforms());
            bill.checkBonus(bonuses);
        
    }

    void updateBonuses(std::vector<std::shared_ptr<Bonus>> &bonuses, float deltaTime, Player &player) {
       
        if (bonuses.empty()) {
            return;
        }

        for (auto it = bonuses.begin(); it != bonuses.end();) {

            (*it)->isBonusActive(player.getPosition());
            (*it)->updateAnimation(deltaTime);
            (*it)->update(deltaTime, map.getPlatforms());
            (*it)->checkDie(player.getBullets(), curScore);

            if (!(*it)->getAlive()) {
                it = bonuses.erase(it);
            } else {
                it++;
            }
        }

    }

    void drawBonuses(std::vector<std::shared_ptr<Bonus>> &bonuses) {

        if (bonuses.empty()) {
            return;
        }
        
        for (auto &bonus : bonuses) {
            bonus->draw();
        }

    }

    void updateSpawners(std::vector<std::shared_ptr<SpawnerAliens>> &spawnersAliens, float deltaTime, 
        std::vector<std::shared_ptr<Alien>>& aliens, Player &player) {
        for (auto it = spawnersAliens.begin(); it != spawnersAliens.end();) {
            if ((*it)->getIsAlive()) {
                (*it)->update(deltaTime, aliens, player);
                ++it;
            } else {
                it = spawnersAliens.erase(it);
            }
        }
    }

    void updateGranateThrowers(std::vector<std::shared_ptr<GranateThrower>> &granateThrowers,
        float deltaTime, std::vector<BulletVariant> &allBullets, Player &player) {

        if (granateThrowers.empty()) {
            return;
        }

        for (auto it = granateThrowers.begin(); it != granateThrowers.end();) {
            (*it)->checkDie(player.getBullets(), curScore);
            (*it)->update(deltaTime, allBullets, player); // Update turret state
            if (!(*it)->getAlive() || player.getPosition().x - (*it)->getPosition().x >= 600) {
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
            (*it)->checkDie(player.getBullets(), curScore);
            (*it)->update(player.getPosition(), map.getPlatforms(), deltaTime, allBullets);
            if (!(*it)->getAlive() || player.getPosition().x - (*it)->getPosition().x >= 600) {
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
            (*it)->checkDie(player.getBullets(), curScore, player.getPosition()  );
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
               std::unique_ptr<HeliBoss> &boss, std::vector<std::shared_ptr<Bonus>> &bonuses) {
                
        BeginDrawing();

        BeginMode2D(gameCamera.camera);

        drawMap();

        drawTurrets(turrets);
        drawAliens(aliens);
        drawGranateThrowers(granateThrowers);

        drawLedders(ledders);

        drawBonuses(bonuses);

        if (boss != nullptr) {
            boss->draw();
        }

        bill.draw(gameCamera.camera.target);

        drawBullets(bullets);

        
        
        //DrawTriangleLines(Vector2{3280, 400}, Vector2{4315, 400}, Vector2{4315, -140}, BLUE); 
        
        //DrawRectangleLines(6760, -950, 450, 2, BLUE);

        /*
        DrawTriangleLines( Vector2{2595, 540}, Vector2{3385, 540}, Vector2{3385, 130}, BLUE);
        DrawRectangleLines(3375, 130, 810, 100, BLUE);
        DrawTriangleLines(Vector2{4185, 130}, Vector2{4885, 130}, Vector2{4885, -220}, BLUE);
        DrawRectangleLines(4875, -220, 910, 100, BLUE);
        DrawTriangleLines( Vector2{5785, -220}, Vector2{6570, -220}, Vector2{6570, -620}, BLUE);
        DrawRectangleLines(6565, -620, 1610, 100, BLUE);
        DrawTriangleLines( Vector2{8175, -620}, Vector2{8895, -620}, Vector2{8895, -970}, BLUE);
        DrawRectangleLines(8895, -970, 2500, 100, BLUE);
        */
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

    int gameRun() {
        // Initialization
        //--------------------------------------------------------------------------------------  
        bossStage = true;
        Music music = LoadMusicStream("resources/BackgroundMusic.ogg"); // Загрузка музыки
        PlayMusicStream(music); // Воспроизведение музыки

        Sound shootSound = LoadSound("resources/ShootSound.ogg"); // Загрузка звука стрельбы

        int frameDelay = 7; // Задержка между кадрами
        int frameDelayCounter = 0;

        std::vector<BulletVariant> allBullets;

        const int screenWidth = 900;
        const int screenHeight = 700;
        
        map.initialization("resources/SuperContraMap.png");
        Player bill(0, 300, "resources/Bill.png");

        GameOverMenu gameOverMenu(hiScore, curScore);

        std::vector<std::shared_ptr<Alien>> aliens;
        std::vector<std::shared_ptr<Ledder>> ledders;
        std::vector<std::shared_ptr<GranateThrower>> granateThrowers;
        std::vector<std::shared_ptr<Turret>> turrets;
        std::vector<std::shared_ptr<SpawnerAliens>> spawnersAliens;
        std::vector<std::shared_ptr<Bonus>> bonuses;

        //турели
        //turrets.push_back(std::make_shared<Turret>(900.0f, 410.0f, 3.0f));
        turrets.push_back(std::make_shared<Turret>(7630.0f, -770.0f, 2.0f));
        turrets.push_back(std::make_shared<Turret>(8050.0f, -770.0f, 2.0f));
        turrets.push_back(std::make_shared<Turret>(4025.0f, -10.0f, 2.0f));
    
        //снайперы
        ledders.push_back(std::make_shared<Ledder>(1330, 130));
        ledders.push_back(std::make_shared<Ledder>(1735, 130));
        ledders.push_back(std::make_shared<Ledder>(2140, 130));
        ledders.push_back(std::make_shared<Ledder>(2850, 50));
        ledders.push_back(std::make_shared<Ledder>(5060, -680));
        ledders.push_back(std::make_shared<Ledder>(7575, -1080));

        //метатели
        granateThrowers.push_back(std::make_shared<GranateThrower>(5360, -680));

        //спаунеры
        spawnersAliens.push_back(std::make_shared<SpawnerAliens>(3760, -380));
        spawnersAliens.push_back(std::make_shared<SpawnerAliens>(6950, -1100));

        //бонусы
        bonuses.push_back(std::make_shared<Bonus>(0, 200, "resources/Bonus.png", TYPE_2, 200));
        bonuses.push_back(std::make_shared<Bonus>(7400, -1100, "resources/Bonus.png", TYPE_1, -1100));
        bonuses.push_back(std::make_shared<Bonus>(4900, -600, "resources/Bonus.png", TYPE_1, -600));
        bonuses.push_back(std::make_shared<Bonus>(8000, -1600, "resources/Bonus.png", TYPE_2, -1600));

        std::unique_ptr<HeliBoss> heliBoss;
        
        GameCamera gameCamera(screenWidth, screenHeight);

        SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
        //--------------------------------------------------------------------------------------
        
        float spawnDelay = 2.0f; // Задержка между спавном инопланетян (в кадрах)
        float spawnCounter = 0.0f; // счетчик времени
        int direction;
        curScore = 0;
        

        // Main game loop
        while (bill.getCOuntLives() > 0)    // Detect window close button or ESC key
        {   
            UpdateMusicStream(music); // Обновление музыкального потока
            std::cout << bill.getPosition().x << " " << bill.getPosition().y << std::endl;
            if (WindowShouldClose()) {
                break;
            }
            // Update
            //----------------------------------------------------------------------------------

            float deltaTime = GetFrameTime();
            spawnCounter += deltaTime;
            frameDelayCounter++;

            if (spawnCounter >= spawnDelay) {
                spawnCounter = 0.0f; // Сброс счетчика
                spawnDelay = ((float) rand() / RAND_MAX) * 2 + 2;
                direction = rand() % 2;
                if (direction) {
                    aliens.push_back(std::make_shared<Alien>(bill.getPosition().x - 600, bill.getPosition().y, direction));
                } else {
                    aliens.push_back(std::make_shared<Alien>(bill.getPosition().x + 900, bill.getPosition().y - 400, direction));
                }
            }

            if (bill.getPosition().x > 11050) {
                break;
            }
    
            bill.isPlayerAlive(aliens, allBullets);
            
            processInput(bill, gameCamera.leftBorder, gameCamera.rightBorder, shootSound);

            deleteBullets(allBullets);

            updateGame(bill, aliens, granateThrowers, ledders, deltaTime, turrets, allBullets, 
                       heliBoss, spawnersAliens, gameCamera, bonuses, music);

            updateBullets(allBullets, bill);
            updateBulletsAnimation(deltaTime, allBullets);
            
            bill.updateAnimation(deltaTime);
            
            gameCamera.setCameraTarget(bill.getPosition().x + bill.getWidth() / 2, bill.getPosition().y + bill.getHeight() / 2 - 140);
            if (frameDelayCounter >= frameDelay) {
            
                bill.addCurFrame();
                updateAliensFrames(aliens);
                frameDelayCounter = 0;
                
            }
            //----------------------------------------------------------------------------------

            // Draw
            //----------------------------------------------------------------------------------
            drawScene(bill, aliens, granateThrowers, ledders, gameCamera, turrets, allBullets, heliBoss, bonuses);
            
            //----------------------------------------------------------------------------------
        }

        UnloadMusicStream(music); // Освобождение памяти
        UnloadSound(shootSound); 

        hiScore = hiScore >= curScore ? hiScore : curScore;
        gameOverMenu.updateScore(hiScore, curScore);
        return gameOverMenu.show();;
    }

};


int main(void)
{
    int enter = 1;
    const int screenWidth = 900;
    const int screenHeight = 700;

    srand(time(NULL));

    InitWindow(screenWidth, screenHeight, "SuperContra");
    InitAudioDevice();

    Game game;
    MainMenu mainMenu;

    int menu;

    while (enter) {
        if (enter == 1) {
            menu = mainMenu.show();
            game.hiScore = 0;
        }
        
        if (menu == 1) {
            enter = game.gameRun();
        } else {
            enter = 0;
        }
        
    }

    return 0;

}  
