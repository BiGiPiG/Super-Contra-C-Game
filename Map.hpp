#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "Platform.hpp"
#include "Ladder.hpp"
#include "raylib.h"
#include <memory>

class Map {
private:
    std::vector<std::shared_ptr<MapObject>> mapObjects;
    Texture2D background;

public:

    Map() {};

    void initialization(const std::string& path) {
        this->background = LoadTexture(path.c_str());

        if (background.id == 0) { // Проверка на успешную загрузку текстуры
            std::cerr << "Failed to load texture from: " << path << std::endl;
            background = {}; // Инициализация текстуры по умолчанию в случае ошибки
        }

        mapObjects.push_back(std::make_shared<Platform>(0, 540, Rectangle{0, 540, 2595, 100}));
        mapObjects.push_back(std::make_shared<Ladder>(450, 485, Vector2{2595, 540}, Vector2{3385, 540}, Vector2{3385, 130}));
        mapObjects.push_back(std::make_shared<Platform>(4315, -140, Rectangle{3375, 130, 810, 100}));
        mapObjects.push_back(std::make_shared<Ladder>(450, 485, Vector2{4185, 130}, Vector2{4885, 130}, Vector2{4885, -220}));
        mapObjects.push_back(std::make_shared<Platform>(4315, -140, Rectangle{4875, -220, 910, 100}));
        mapObjects.push_back(std::make_shared<Ladder>(450, 485, Vector2{5785, -220}, Vector2{6570, -220}, Vector2{6570, -620}));
        mapObjects.push_back(std::make_shared<Platform>(4315, -140, Rectangle{6565, -620, 1610, 100}));
        mapObjects.push_back(std::make_shared<Ladder>(1970, 128, Vector2{8175, -620}, Vector2{8895, -620}, Vector2{8895, -970}));
        mapObjects.push_back(std::make_shared<Platform>(4315, -140, Rectangle{8890, -970, 2500, 100}));

        mapObjects.push_back(std::make_shared<Platform>(3533, 15, Rectangle{3533, -190, 450, 2}));
        mapObjects.push_back(std::make_shared<Platform>(6760, -745, Rectangle{6760, -950, 450, 2}));
        
    }

    ~Map() {
        UnloadTexture(background); // Освобождение текстуры
    }

    void mapDraw() {
        DrawTexture(background, 0, -1580, WHITE); 
    }

    const std::vector<std::shared_ptr<MapObject>>& getPlatforms() const {
        return this->mapObjects;
    }
};