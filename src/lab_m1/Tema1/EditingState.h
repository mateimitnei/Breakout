#pragma once

#include "lab_m1/Tema1/GameState.h"
#include "lab_m1/Tema1/grid.h"
#include "lab_m1/Tema1/PlayingState.h"

namespace m1
{
    class EditingState : public m1::GameState
    {
        friend class Tema1;
        friend class PlayingState;

     public:
        EditingState(Tema1* game);
        ~EditingState();

        void Init() override;

     private:
        void Update(float deltaTimeSeconds) override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnWindowResize(int width, int height) override;

        // helpers
        bool GrabbedItem(int mouseX, int mouseY);
        bool ClickedPlay(int mouseX, int mouseY);
        void RenderGrid();
        void RenderItemPanel();
        void RenderTopbar();
        void TransferBuild(PlayingState* pState);
        
        //variables
        m1::Grid grid;
        bool isDragging = false;
        bool goodBuild = false;
        glm::vec2 dragPos = glm::vec2(0.0f);

        // layout for the left panel
        float panelStartX;
        float panelStartY;

        // layout for the top bar
        struct {
            float startX;
            float startY;
            float width;
            float squareSize = 40;
            float gap = 15;
            float padding = 10;
        } topbar;
    };
}
