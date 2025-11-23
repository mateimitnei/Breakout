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

EditingState::EditingState(Tema1* game) : GameState(game) {}

EditingState::~EditingState() {}

void EditingState::Init()
{
    // configure Grid
    grid.Init(game->initialResolution.x, game->initialResolution.y);

    panelStartX = grid.startX - grid.cellSize - 80;
    panelStartY = grid.startY + grid.height - grid.cellSize;

    topbar.startX = grid.startX;
    topbar.startY = grid.startY + grid.height + 70;
    topbar.width = grid.width;

    isDragging = false;
    dragPos = glm::vec2(0.0f);

    game->meshes.clear();

    game->AddMeshToList(object2D::CreateSquare("unit_square", glm::vec3(0, 0, 0), 1, glm::vec3(0.05f, 0.2f, 0.25f), true));
    game->AddMeshToList(object2D::CreateSquare("square_green", glm::vec3(0,0,0), 1, glm::vec3(0.2f, 0.8f, 0.2f), true));

    // mesh for the block item
    game->AddMeshToList(object2D::CreateSquare("block_item", glm::vec3(0,0,0), 1, glm::vec3(0.6f, 0.6f, 0.6f), true));

    // meshes for outlines
    game->AddMeshToList(object2D::CreateSquare("square_outline", glm::vec3(0, 0, 0), 1, glm::vec3(0.2f, 0.8f, 0.2f), false));
    game->AddMeshToList(object2D::CreateSquare("unit_outline", glm::vec3(0, 0, 0), 1, glm::vec3(0.3f, 0.6f, 0.65f), false));
    game->AddMeshToList(object2D::CreateSquare("black_outline", glm::vec3(0, 0, 0), 1, glm::vec3(0.0f, 0.0f, 0.0f), false));

    // play button meshes (green and red)
    game->AddMeshToList(m1::PlayButton("play_button", glm::vec3(0,0,0), 1, glm::vec3(0.2f, 0.8f, 0.2f)));
    game->AddMeshToList(m1::PlayButton("play_button_red", glm::vec3(0,0,0), 1, glm::vec3(0.8f, 0.2f, 0.2f)));
}


void EditingState::Update(float deltaTimeSeconds)
{
    // Draws the block item and the panel outline
    RenderItemPanel();

    // Draws the grid cells and the grid outline
    RenderGrid();

    // Draws the remaining blocks indicator and the play button
    RenderTopbar();

    // If dragging, draw the dragged block at cursor position
    if (isDragging) {
        glm::mat3 model = glm::mat3(1);
        // center the block at cursor (dragPos is world pos)
        model *= transform2D::Translate(dragPos.x - grid.cellSize * 0.5f, dragPos.y - grid.cellSize * 0.5f);
        model *= transform2D::Scale(grid.cellSize, grid.cellSize);

        game->RenderMesh2D(game->meshes["block_item"], game->shaders["VertexColor"], model);
    }
}

// helper: render the grid
void EditingState::RenderGrid()
{
    for (int row = 0; row < grid.rows; row++) {
        for (int col = 0; col < grid.cols; col++) {
            float x = grid.startX + col * (grid.cellSize);
            float y = grid.startY + row * (grid.cellSize);

            glm::mat3 modelMatrix = glm::mat3(1);
            modelMatrix *= transform2D::Translate(x, y);
            modelMatrix *= transform2D::Scale(grid.cellSize, grid.cellSize);

            game->RenderMesh2D(game->meshes["unit_square"], game->shaders["VertexColor"], modelMatrix);
            glLineWidth(5);
            game->RenderMesh2D(game->meshes["black_outline"], game->shaders["VertexColor"], modelMatrix);
            glLineWidth(1);

            // render placed items in the grid:
            if (grid.cells[row][col].occupied) {
                game->RenderMesh2D(game->meshes["block_item"], game->shaders["VertexColor"], modelMatrix);
            }
        }
    }

    // Draw outer outline around the whole grid using a single rectangle (wireframe)
    glm::mat3 model = glm::mat3(1);
    float padding = 10;
    model *= transform2D::Translate(grid.startX - padding, grid.startY - padding);
    model *= transform2D::Scale(grid.width + 2 * padding, grid.height + 2 * padding);

    glLineWidth(2);
    game->RenderMesh2D(game->meshes["unit_outline"], game->shaders["VertexColor"], model);
    glLineWidth(1);
}

//helper: render the item panel
void EditingState::RenderItemPanel()
{
    glm::mat3 model = glm::mat3(1);
    model *= transform2D::Translate(panelStartX, panelStartY);
    model *= transform2D::Scale(grid.cellSize, grid.cellSize);

    game->RenderMesh2D(game->meshes["block_item"], game->shaders["VertexColor"], model);

    // Draw outer outline around the whole grid using a single rectangle (wireframe)
    model = glm::mat3(1);
    float padding = 10;
    model *= transform2D::Translate(panelStartX - padding, panelStartY - padding);
    model *= transform2D::Scale(grid.cellSize + 2 * padding, grid.cellSize + 2 * padding);

    glLineWidth(2);
    game->RenderMesh2D(game->meshes["unit_outline"], game->shaders["VertexColor"], model);
    glLineWidth(1);
}

void EditingState::RenderTopbar()
{
    glm::mat3 model = glm::mat3(1);
    model *= transform2D::Translate(topbar.startX - topbar.padding, topbar.startY - topbar.padding);
    model *= transform2D::Scale(topbar.width + 2 * topbar.padding, topbar.squareSize / 2 + 2 * topbar.padding);
    glLineWidth(2);
    game->RenderMesh2D(game->meshes["unit_outline"], game->shaders["VertexColor"], model);
    glLineWidth(1);

    // Draw the green squares representing the remaining blocks
    for (int i = 0; i < 10; i++) {
        glm::mat3 squareModel = glm::mat3(1);
        float x = topbar.startX + topbar.padding + i * (topbar.squareSize + topbar.gap);
        float y = topbar.startY;
        squareModel *= transform2D::Translate(x, y);
        squareModel *= transform2D::Scale(topbar.squareSize, topbar.squareSize / 2);
        if (i < 10 - grid.itemsNum) {
            game->RenderMesh2D(game->meshes["square_green"], game->shaders["VertexColor"], squareModel);
        } else {
            game->RenderMesh2D(game->meshes["square_outline"], game->shaders["VertexColor"], squareModel);
        }
    }

    // play button
    glm::mat3 squareModel = glm::mat3(1);
    float x = topbar.startX + topbar.width - topbar.padding - topbar.squareSize;
    float y = topbar.startY;
    squareModel *= transform2D::Translate(x, y);
    squareModel *= transform2D::Scale(topbar.squareSize, topbar.squareSize);
    if (goodBuild) {
        game->RenderMesh2D(game->meshes["play_button"], game->shaders["VertexColor"], squareModel);
    } else {
        game->RenderMesh2D(game->meshes["play_button_red"], game->shaders["VertexColor"], squareModel);
    }
}

// helper: detect which left panel item (0..3) clicked or -1
bool EditingState::GrabbedItem(int mouseX, int mouseY)
{
    glm::vec2 cursor = game->MouseToWorld(mouseX, mouseY);
    float x = panelStartX;
    float y = panelStartY;
    if (cursor.x >= x && cursor.x <= x + grid.cellSize
        && cursor.y >= y && cursor.y <= y + grid.cellSize) {
        return true;
    }
    return false;
}

// helper: detect if play button clicked
bool EditingState::ClickedPlay(int mouseX, int mouseY) {
    glm::vec2 cursor = game->MouseToWorld(mouseX, mouseY);
    float x = topbar.startX + topbar.width - topbar.padding - topbar.squareSize;
    float y = topbar.startY;
    if (cursor.x >= x && cursor.x <= x + topbar.squareSize
        && cursor.y >= y && cursor.y <= y + topbar.squareSize) {
        return true;
    }
    return false;
}

void EditingState::TransferBuild(PlayingState* pState) {
    int minR = INT_MAX, minC = INT_MAX, maxR = 0, maxC = 0;

    for (int r = 0; r < grid.rows; r++) {
        for (int c = 0; c < grid.cols; c++) {
            if (grid.cells[r][c].occupied) {
                minR = fmin(minR, r);
                minC = fmin(minC, c);
                maxR = fmax(maxR, r);
                maxC = fmax(maxC, c);
            }
        }
    }
    pState->build.cells.clear();
    pState->build.rows = maxR - minR + 1;
    pState->build.cols = maxC - minC + 1;
    pState->build.cells.resize(pState->build.rows, std::vector<PlayingState::Cell>(pState->build.cols));

    for (int r = 0; r < grid.rows; r++) {
        for (int c = 0; c < grid.cols; c++) {
            if (grid.cells[r][c].occupied) {
                pState->build.cells[r - minR][c - minC].occupied = true;
            }
        }
    }
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void EditingState::OnInputUpdate(float deltaTime, int mods)
{
}


void EditingState::OnKeyPress(int key, int mods)
{
    // Add key press event
}


void EditingState::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
    if (isDragging) {
        dragPos = game->MouseToWorld(mouseX, mouseY);
    }
}


void EditingState::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
    if (button == 1) {
        isDragging = GrabbedItem(mouseX, mouseY);
        if (isDragging) {
            dragPos = game->MouseToWorld(mouseX, mouseY);
        }

        // Switch the game state to playing if the play button is clicked and the build is correct
        bool play = ClickedPlay(mouseX, mouseY);
        if (play && goodBuild) {
            PlayingState* newState = new PlayingState(game);
            TransferBuild(newState);
            game->ChangeState(newState);
        }
    }

    if (button == 2) {
        // if right click, delete placed item from grid
        glm::vec2 p = game->MouseToWorld(mouseX, mouseY);
        glm::vec2 cell = grid.CursorOnCell(p);
        if (cell.x != -1 && grid.cells[cell.x][cell.y].occupied) {
            grid.cells[cell.x][cell.y].occupied = false;
            grid.itemsNum--;
            if (grid.itemsNum <= 10) {
                goodBuild = true;
            }
            if (grid.itemsNum < 1) {
                goodBuild = false;
            }
            goodBuild = goodBuild && grid.CheckBuildShape();
        }
    }
}


void EditingState::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
    if (button == 1 && isDragging) {
        glm::vec2 p = game->MouseToWorld(mouseX, mouseY);
        glm::vec2 cell = grid.CursorOnCell(p);
        if (cell.x != -1) {
            if (!grid.cells[cell.x][cell.y].occupied) {
                grid.itemsNum++;
                goodBuild = grid.itemsNum > 10 ? false : true;
            }
            grid.cells[cell.x][cell.y].occupied = true;
            goodBuild = goodBuild && grid.CheckBuildShape();
            if (grid.itemsNum == 1) goodBuild = true;
        }
        // stop dragging even if not placed in grid
        isDragging = false;
    }
}

void EditingState::OnWindowResize(int width, int height)
{
}