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
        std::cout << "Loading map from: " << path << std::endl;
        background = LoadTexture(path.c_str());

        // Check if the texture loaded successfully
        if (background.id == 0) {
            std::cerr << "Failed to load texture from: " << path << std::endl;
            // Handle error appropriately (e.g., set a default texture or exit)
            return; // Prevent further execution if texture loading fails
        } else {
            std::cout << "Texture loaded successfully." << std::endl;
        }

        // Initialize map objects
        mapObjects.push_back(std::make_shared<Platform>(0, 540, Rectangle{0, 540, 1720, 100}));
        mapObjects.push_back(std::make_shared<Platform>(2200, 505, Rectangle{1720, 610, 230, 100}));
        mapObjects.push_back(std::make_shared<Platform>(2500, 390, Rectangle{1950, 540, 645, 100}));
        mapObjects.push_back(std::make_shared<Ladder>(450, 485, Vector2{2595, 540}, Vector2{3385, 540}, Vector2{3385, 130}));
        mapObjects.push_back(std::make_shared<Platform>(4315, -140, Rectangle{3375, 130, 810, 100}));
        mapObjects.push_back(std::make_shared<Ladder>(450, 485, Vector2{4185, 130}, Vector2{4885, 130}, Vector2{4885, -220}));
        mapObjects.push_back(std::make_shared<Platform>(4315, -140, Rectangle{4875, -220, 910, 100}));
        mapObjects.push_back(std::make_shared<Ladder>(450, 485, Vector2{5785, -220}, Vector2{6570, -220}, Vector2{6570, -620}));
        mapObjects.push_back(std::make_shared<Platform>(4315, -140, Rectangle{6565, -620, 1610, 100}));
        mapObjects.push_back(std::make_shared<Ladder>(1970, 128, Vector2{8175, -620}, Vector2{8895, -620}, Vector2{8895, -970}));
        mapObjects.push_back(std::make_shared<Platform>(4315, -140, Rectangle{8895, -970, 2500, 100}));
    }

    ~Map() {
        UnloadTexture(background); // Release the texture
    }

    void mapDraw() {
        DrawTexture(background, 0, -1580, WHITE); 
    }

    const std::vector<std::shared_ptr<MapObject>>& getPlatforms() const {
        return this->mapObjects;
    }
};