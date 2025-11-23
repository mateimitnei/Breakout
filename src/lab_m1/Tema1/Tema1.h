#pragma once

#include "components/simple_scene.h"
#include "lab_m1/Tema1/grid.h"
#include "lab_m1/Tema1/GameState.h"
#include "lab_m1/Tema1/EditingState.h"
#include "lab_m1/Tema1/PlayingState.h"

namespace m1
{
    class Tema1 : public gfxc::SimpleScene
    {
        friend class EditingState;
        friend class PlayingState;

     public:
        Tema1();
        ~Tema1();

        void Init() override;

     private:
        GameState* state = nullptr;

        void ChangeState(GameState* newState);

        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        glm::vec2 MouseToWorld(int mouseX, int mouseY);

        // variables
        glm::ivec2 initialResolution = glm::ivec2(0, 0);
    };
}