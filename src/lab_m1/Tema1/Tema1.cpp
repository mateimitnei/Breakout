#include "lab_m1/Tema1/Tema1.h"

#include <vector>
#include <iostream>

#include "lab_m1/lab3/transform2D.h"
#include "lab_m1/lab3/object2D.h"
#include "lab_m1/Tema1/my_objects.h"
#include "lab_m1/Tema1/grid.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */

Tema1::Tema1() {}

Tema1::~Tema1() {}

void Tema1::Init()
{
    initialResolution = window->GetResolution();

    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)initialResolution.x, 0, (float)initialResolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    glDisable(GL_DEPTH_TEST);

    ChangeState(new EditingState(this));
}


void Tema1::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema1::Update(float deltaTimeSeconds)
{
    state->Update(deltaTimeSeconds);
}


void Tema1::FrameEnd()
{
}


void Tema1::ChangeState(GameState* newState)
{
    if (state) {
        delete state;
    }
    state = newState;
    state->Init();
}


//helper: convert screen (mouse) to world coords used by our transforms
glm::vec2 Tema1::MouseToWorld(int mouseX, int mouseY)
{
    glm::ivec2 resolution = window->GetResolution();
    float scaleX = (float)initialResolution.x / (float)resolution.x;
    float scaleY = (float)initialResolution.y / (float)resolution.y;
    float x = (float)mouseX * scaleX;
    float y = (float)(resolution.y - mouseY) * scaleY; // invert Y
    return glm::vec2(x, y);
}

/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */

void Tema1::OnInputUpdate(float deltaTime, int mods)
{
    state->OnInputUpdate(deltaTime, mods);
}


void Tema1::OnKeyPress(int key, int mods)
{
    // Add key press event
    state->OnKeyPress(key, mods);
}


void Tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    state->OnMouseMove(mouseX, mouseY, deltaX, deltaY);
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    state->OnMouseBtnPress(mouseX, mouseY, button, mods);
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    state->OnMouseBtnRelease(mouseX, mouseY, button, mods);
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema1::OnWindowResize(int width, int height)
{
    state->OnWindowResize(width, height);
}
