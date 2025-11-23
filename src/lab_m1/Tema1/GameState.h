#pragma once

namespace m1
{
    class Tema1;

    class GameState
    {
      friend class Tema1;

     public:
        explicit GameState(Tema1* game);
        virtual ~GameState() = 0;

        virtual void Init() = 0;

     protected:
        Tema1* game;

     private:
        virtual void Update(float deltaTimeSeconds) = 0;
        virtual void OnInputUpdate(float deltaTime, int mods) = 0;
        virtual void OnKeyPress(int key, int mods) = 0;
        virtual void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) = 0;
        virtual void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) = 0;
        virtual void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) = 0;
        virtual void OnWindowResize(int width, int height) = 0;
    };
}