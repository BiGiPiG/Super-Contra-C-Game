#include "raylib.h"
#include "Map.h"
#include "Player.h"
#include "GameCamera.h"
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

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 900;
    const int screenHeight = 600;

    int frameDelay = 5; // Задержка между кадрами
    int frameDelayCounter = 0;
    int currentFrame = 0;

    InitWindow(screenWidth, screenHeight, "SuperContra");
    Game game("resources/SuperContraMap.png");
    Player bill(0, 100, "resources/Bill.png");
    
    GameCamera gameCamera(screenWidth, screenHeight);

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
  

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        frameDelayCounter++;
        if (IsKeyPressed(KEY_SPACE)) {
            bill.jump(game.map.getPlatforms());
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

        bill.update(game.map.getPlatforms(), currentFrame);
        /*
        if (camera.target.x < bill.getPosition().x + bill.getWidth() / 2) {
            camera.target.x = bill.getPosition().x + bill.getWidth() / 2; 
        }
        */
        gameCamera.setCameraTarget(bill.getPosition().x + bill.getWidth() / 2);
        //camera.target.y = bill.getPosition().y + bill.getHeight() / 2; 
        
        if (frameDelayCounter >= frameDelay) {
            frameDelayCounter = 0;
            
            //bill.update(game.map.getPlatforms(), currentFrame);
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
        DrawTriangleLines(Vector2{450, 480}, Vector2{900, 480}, Vector2{900, 400}, BLUE); 
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