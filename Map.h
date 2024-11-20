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
        mapObjects.push_back(std::make_shared<Platform>(0, 472, Rectangle{0, 472, 1970, 128}));
        mapObjects.push_back(std::make_shared<Ladder>(450, 480, Vector2{450, 485}, Vector2{900, 485}, Vector2{900, 400}));
        
    }

    ~Map() {
        UnloadTexture(background); // Освобождение текстуры
    }

    void mapDraw() {
        DrawTexture(background, 0, -1320, WHITE); 
    }

    const std::vector<std::shared_ptr<MapObject>>& getPlatforms() const {
        return this->mapObjects;
    }
};