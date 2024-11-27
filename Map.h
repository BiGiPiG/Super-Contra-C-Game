#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "Platform.h"
#include "Ladder.h"
#include "raylib.h"
#include <memory>

class Map {
private:
    std::vector<std::shared_ptr<MapObject>> mapObjects;
    Texture2D background;

public:
    Map(const std::string& path) {
        this->background = LoadTexture(path.c_str());
        if (background.id == 0) { // Проверка на успешную загрузку текстуры
            std::cerr << "Failed to load texture from: " << path << std::endl;
            background = {}; // Инициализация текстуры по умолчанию в случае ошибки
        }
        mapObjects.push_back(std::make_shared<Platform>(0, 390, Rectangle{0, 400, 2200, 200}));
        mapObjects.push_back(std::make_shared<Platform>(2200, 505, Rectangle{2200, 505, 300, 95}));
        mapObjects.push_back(std::make_shared<Platform>(2500, 390, Rectangle{2500, 400, 780, 200}));
        mapObjects.push_back(std::make_shared<Ladder>(450, 485, Vector2{3280, 400}, Vector2{4315, 400}, Vector2{4315, -140}));
        mapObjects.push_back(std::make_shared<Platform>(4315, -140, Rectangle{4300, -140, 1000, 200}));
        //mapObjects.push_back(std::make_shared<Ladder>(450, 485, Vector2{450, 485}, Vector2{900, 485}, Vector2{900, 100}));
        //mapObjects.push_back(std::make_shared<Ladder>(1970, 128, Vector2{1970, 472}, Vector2{2586, 472}, Vector2{2586, 168}));
        
    }

    ~Map() {
        UnloadTexture(background); // Освобождение текстуры
    }

    void mapDraw() {
        DrawTexture(background, 0, -2320, WHITE); 
    }

    const std::vector<std::shared_ptr<MapObject>>& getPlatforms() const {
        return this->mapObjects;
    }
};