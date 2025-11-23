#pragma once

#include "lab_m1/Tema1/GameState.h"
#include "lab_m1/Tema1/grid.h"


namespace m1
{
    class PlayingState : public m1::GameState
    {
        friend class Tema1;
        friend class EditingState;

     public:
        PlayingState(Tema1* game);
        ~PlayingState();

        void Init() override;

     private:
        void Update(float deltaTimeSeconds) override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnWindowResize(int width, int height) override;

        void RenderGrid();
        void RenderBuild();
        void RenderTopBar();
        
        // variables
        bool gameStarted;
        int lives;
        int score;
        // camera shake on block destruction
        float cameraShakeTimer = 0.0f;
        float cameraShakeDuration = 0.25f;

        struct Ball {
            int posX;
            int posY;
            float radius = 9.0f;
            int smoothness = 36;
            float speedX = 0.0f;
            float speedY = 0.0f;
        } ball;

        struct Cell {
            bool occupied = false;
        };

        struct {
            float cellSize = 30.0f;
            float startX;
            float startY = 20.0f;
            int rows;
            int cols;
            std::vector<std::vector<Cell>> cells;
        } build;

        struct Block {
            int life;
            float startX;
            float startY;
            bool destroying = false;
            float destroyTimer = 0.0f;
            float destroyDuration = 0.35f;
            float scale = 1.0f; 
            float hitTimer = 0.0f;
            float hitDuration = 0.1f;
        };

        void CheckBlockCollision(Block &block);
        void CheckBuildCollision();
        void CheckWallCollision();
        void CheckCollisions();

        struct {
            std::vector<std::vector<Block>> blocks;
            int rows = 5;
            int cols = 9;
            float blockWidth;
            float blockHeight;
            float startX;
            float startY;
        } grid;
    };
}
