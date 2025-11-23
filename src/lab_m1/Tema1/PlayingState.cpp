#include "lab_m1/Tema1/Tema1.h"
#include "components/text_renderer.h"
#include "core/managers/resource_path.h"

#include <vector>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <ctime>

#include "lab_m1/lab3/transform2D.h"
#include "lab_m1/lab3/object2D.h"
#include "lab_m1/Tema1/my_objects.h"
#include "lab_m1/Tema1/grid.h"

using namespace std;
using namespace m1;

gfxc::TextRenderer* textRenderer = nullptr;

/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */

PlayingState::PlayingState(Tema1* game) : GameState(game) {}

PlayingState::~PlayingState() {}

void PlayingState::Init()
{
    lives = 3;
    score = 0;
    gameStarted = false;

    build.startX = game->initialResolution.x / 2 - build.cellSize * build.cols / 2;
    ball.posX = build.startX + build.cellSize * build.cols / 2;
    ball.posY = build.startY + build.cellSize * build.rows + ball.radius;

    grid.blocks.clear();
    grid.blockHeight = game->initialResolution.y / 2 / grid.rows;
    grid.blockWidth = game->initialResolution.x / grid.cols;
    grid.startY = game->initialResolution.y - grid.rows * grid.blockHeight;
    grid.blocks.resize(grid.rows, std::vector<Block>(grid.cols));

    for (int i = 0; i < grid.rows; i++) {
        for (int j = 0; j < grid.cols; j++) {
            grid.blocks[i][j].startX = j * grid.blockWidth;
            grid.blocks[i][j].startY = grid.startY + i * grid.blockHeight;

            if (i == j || i == grid.cols - j - 1) {
                grid.blocks[i][j].life = 3;
            }
            else if (i < j && i < grid.cols - j - 1) {
                grid.blocks[i][j].life = 2;
            }
            else {
                grid.blocks[i][j].life = 1;
            }
        }
    }
    
    game->meshes.clear();
    // Ball mesh
    game->AddMeshToList(CreateBall("ball", glm::vec3(0, 1, 0), ball.radius, ball.smoothness));
    // Build item mesh
    game->AddMeshToList(object2D::CreateSquare("block_item", glm::vec3(0,0,0), 1, glm::vec3(0.6f, 0.6f, 0.6f), true));
    // Block meshes
    game->AddMeshToList(object2D::CreateSquare("one_point_block", glm::vec3(0,0,0), 1, glm::vec3(0.3f, 0.6f, 0.65f), true));
    game->AddMeshToList(object2D::CreateSquare("two_points_block", glm::vec3(0,0,0), 1, glm::vec3(0.15f, 0.4f, 0.45f), true));
    game->AddMeshToList(object2D::CreateSquare("three_points_block", glm::vec3(0,0,0), 1, glm::vec3(0.07f, 0.23f, 0.27f), true));
    game->AddMeshToList(object2D::CreateSquare("black_outline", glm::vec3(0, 0, 0), 1, glm::vec3(0.0f, 0.0f, 0.0f), false));
    // White edge meshes for the build
    game->AddMeshToList(object2D::CreateSquare("white_square", glm::vec3(0,0,0), 1, glm::vec3(1.0f, 1.0f, 1.0f), true));
    // Heart mesh
    game->AddMeshToList(m1::CreateHeart("heart", glm::vec3(1, 0, 0), 12.0f, 30));

    textRenderer = new gfxc::TextRenderer(game->window->props.selfDir,
            game->window->GetResolution().x, game->window->GetResolution().y);
    textRenderer->Load(PATH_JOIN(game->window->props.selfDir, RESOURCE_PATH::FONTS, "Hack-Bold.ttf"), 18);
}


void PlayingState::Update(float deltaTimeSeconds)
{
    // Camera shake for each block destruction
    if (cameraShakeTimer > 0.0f) {
        cameraShakeTimer -= deltaTimeSeconds;
        float elapsed = cameraShakeDuration - cameraShakeTimer;
        if (elapsed < 0.0f) elapsed = 0.0f;
        if (elapsed > cameraShakeDuration) elapsed = cameraShakeDuration;
        float progress = elapsed / cameraShakeDuration;

        const glm::vec2 movements[] = {
            glm::vec2(-5.0f,  0.5f),
            glm::vec2( 5.0f,  2.5f),
            glm::vec2(-3.0f, -2.5f),
            glm::vec2( 1.5f,  2.0f)
        };

        int i = progress * 4;
        if (i >= 4) i = 0;

        glm::vec2 offset = movements[i] * (1.0f - progress);

        auto camera = game->GetSceneCamera();
        camera->SetPosition(glm::vec3(offset.x, offset.y, 50));
        camera->Update();
    } else {
        // Reset camera
        auto camera = game->GetSceneCamera();
        camera->SetPosition(glm::vec3(0, 0, 50));
        camera->Update();
    }

    // Update each block status
    for (int i = 0; i < grid.rows; i++) {
        for (int j = 0; j < grid.cols; j++) {
            Block &b = grid.blocks[i][j];

            if (b.hitTimer > 0.0f) {
                b.hitTimer -= deltaTimeSeconds;
                if (b.hitTimer < 0.0f) b.hitTimer = 0.0f;
            }

            if (!b.destroying) continue;
            b.destroyTimer += deltaTimeSeconds;
            float t = b.destroyTimer / b.destroyDuration;
            if (t >= 1.0f) {
                // finished shrinking
                b.scale = 0.0f;
                b.destroying = false;
            } else {
                // shrink block
                b.scale = 1.0f - t;
            }
        }
    }

    RenderGrid();
    RenderBuild();

    // Draw the ball
    if (!gameStarted) {
        ball.posX = build.startX + build.cellSize * build.cols / 2;
    } else {
        ball.posX += ball.speedX * deltaTimeSeconds;
        ball.posY += ball.speedY * deltaTimeSeconds;
    }
    glm::mat3 model = glm::mat3(1);
    model *= transform2D::Translate(ball.posX, ball.posY);
    game->RenderMesh2D(game->meshes["ball"], game->shaders["VertexColor"], model);

    CheckCollisions();

    RenderTopBar();
}


void PlayingState::RenderTopBar() {
    // Render hearts in the top left corner
    const float heartRadius = 7.0f;
    const float margin = 20.0f;
    const float spacing = heartRadius * 2.0f + 20.0f;
    for (int i = 0; i < lives; i++) {
        glm::mat3 heartModel = glm::mat3(1);
        float x = 1.5f * margin + i * spacing;
        float y = game->initialResolution.y - margin - heartRadius;
        heartModel *= transform2D::Translate(x, y);
        heartModel *= transform2D::Scale(heartRadius, heartRadius);
        game->RenderMesh2D(game->meshes["heart"], game->shaders["VertexColor"], heartModel);
    }
    // Render score in the top-right corner
    float x = game->window->GetResolution().x - 2.7f * margin;
    textRenderer->RenderText(std::to_string(score), x, margin, 1.5f, glm::vec3(1));
}


void PlayingState::RenderGrid() {
    for (int i = 0; i < grid.rows; i++) {
        for (int j = 0; j < grid.cols; j++) {
            Block &b = grid.blocks[i][j];

            if (b.life <= 0 && !b.destroying) {
                continue;
            }

            glm::mat3 modelMatrix = glm::mat3(1);
            // offset so scaling occurs around the block center
            float offsetX = (1.0f - b.scale) * 0.5f * grid.blockWidth;
            float offsetY = (1.0f - b.scale) * 0.5f * grid.blockHeight;
            modelMatrix *= transform2D::Translate(b.startX + offsetX, b.startY + offsetY);
            modelMatrix *= transform2D::Scale(grid.blockWidth * b.scale, grid.blockHeight * b.scale);

            if (b.life == 3) {
                game->RenderMesh2D(game->meshes["three_points_block"], game->shaders["VertexColor"], modelMatrix);
            }
            else if (b.life == 2) {
                game->RenderMesh2D(game->meshes["two_points_block"], game->shaders["VertexColor"], modelMatrix);
            }
            else if (b.life == 1) {
                game->RenderMesh2D(game->meshes["one_point_block"], game->shaders["VertexColor"], modelMatrix);
            } else {
                game->RenderMesh2D(game->meshes["one_point_block"], game->shaders["VertexColor"], modelMatrix);
            }

            glLineWidth(4);
            game->RenderMesh2D(game->meshes["black_outline"], game->shaders["VertexColor"], modelMatrix);
            glLineWidth(1);
        }
    }
}


void PlayingState::RenderBuild() {
    for (int r = 0; r < build.rows; r++) {
        for (int c = 0; c < build.cols; c++) {
            if (build.cells[r][c].occupied) {
                float x = build.startX + c * build.cellSize;
                float y = build.startY + r * build.cellSize;

                glm::mat3 modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(x, y);
                modelMatrix *= transform2D::Scale(build.cellSize, build.cellSize);

                game->RenderMesh2D(game->meshes["block_item"], game->shaders["VertexColor"], modelMatrix);

                float edgeWidth = 2.5f;
                // Draw white outline where it doesn't have neighbors
                if (r == build.rows - 1 || !build.cells[r + 1][c].occupied) {
                    // top
                    glm::mat3 lineModel = glm::mat3(1);
                    lineModel *= transform2D::Translate(x, y + build.cellSize - edgeWidth);
                    lineModel *= transform2D::Scale(build.cellSize, edgeWidth);
                    game->RenderMesh2D(game->meshes["white_square"], game->shaders["VertexColor"], lineModel);
                }
                if (r == 0 || !build.cells[r - 1][c].occupied) {
                    // bottom
                    glm::mat3 lineModel = glm::mat3(1);
                    lineModel *= transform2D::Translate(x, y);
                    lineModel *= transform2D::Scale(build.cellSize, edgeWidth);
                    game->RenderMesh2D(game->meshes["white_square"], game->shaders["VertexColor"], lineModel);
                }
                if (c == 0 || !build.cells[r][c - 1].occupied) {
                    // left
                    glm::mat3 lineModel = glm::mat3(1);
                    lineModel *= transform2D::Translate(x, y);
                    lineModel *= transform2D::Scale(edgeWidth, build.cellSize);
                    game->RenderMesh2D(game->meshes["white_square"], game->shaders["VertexColor"], lineModel);
                }
                if (c == build.cols - 1 || !build.cells[r][c + 1].occupied) {
                    // right
                    glm::mat3 lineModel = glm::mat3(1);
                    lineModel *= transform2D::Translate(x + build.cellSize - edgeWidth, y);
                    lineModel *= transform2D::Scale(edgeWidth, build.cellSize);
                    game->RenderMesh2D(game->meshes["white_square"], game->shaders["VertexColor"], lineModel);
                }
            }
        }
    }
}


void PlayingState::CheckCollisions()
{
    CheckWallCollision();
    CheckBuildCollision();
    // Check for collisions with blocks
    for (int i = 0; i < grid.rows; i++) {
        for (int j = 0; j < grid.cols; j++) {
            CheckBlockCollision(grid.blocks[i][j]);
        }
    }
}

// Screen sides - ball collision
void PlayingState::CheckWallCollision()
{
    // Check for collision with window edges
    if (ball.posX - ball.radius < 0) {
        ball.posX = ball.radius;
        ball.speedX = -ball.speedX;
    }
    if (ball.posX + ball.radius > game->initialResolution.x) {
        ball.posX = game->initialResolution.x - ball.radius;
        ball.speedX = -ball.speedX;
    }
    if (ball.posY + ball.radius > game->initialResolution.y) {
        ball.posY = game->initialResolution.y - ball.radius;
        ball.speedY = -ball.speedY;
    }
    if (ball.posY - ball.radius < 0) {
        // Reset ball position and stop the game
        build.startX = game->initialResolution.x / 2 - build.cellSize * build.cols / 2;
        ball.posX = build.startX + build.cellSize * build.cols / 2;
        ball.posY = build.startY + build.cellSize * build.rows + ball.radius;
        ball.speedX = 0.0f;
        ball.speedY = 0.0f;
        gameStarted = false;
        lives--;
        if (lives == 0) this->Init();
    }
}

// AABB block - ball collision
void PlayingState::CheckBlockCollision(Block &block)
{
    if (block.life <= 0 || block.hitTimer > 0.0f) return;

    float minX = block.startX;
    float minY = block.startY;
    float maxX = minX + grid.blockWidth;
    float maxY = minY + grid.blockHeight;

    // Closest point on AABB to the ball's center
    float closestX = fmax(minX, fmin(ball.posX, maxX));
    float closestY = fmax(minY, fmin(ball.posY, maxY));

    // Check for intersection
    float dx = ball.posX - closestX;
    float dy = ball.posY - closestY;
    float dist = dx * dx + dy * dy;
    if (dist > ball.radius * ball.radius) return; // no collision

    // Collision
    block.life--;
    if (block.life == 0) {
        block.destroying = true;
        score++;
        cameraShakeTimer = cameraShakeDuration;
        if (score == grid.rows * grid.cols){
            Init();
            return;
        }
    }
    block.hitTimer = 0.15f;

    // Block's center and differences
    float centerX = (minX + maxX) / 2.0f;
    float centerY = (minY + maxY) / 2.0f;
    float diffX = ball.posX - centerX;
    float diffY = ball.posY - centerY;

    // Compute overlap amounts along each axis (positive = penetration)
    float halfW = grid.blockWidth * 0.5f;
    float halfH = grid.blockHeight * 0.5f;
    float overlapX = (halfW + ball.radius) - std::abs(diffX);
    float overlapY = (halfH + ball.radius) - std::abs(diffY);

    // Decide direction of bounce by the smaller overlap
    if (overlapX < overlapY) {
        // horizontal
        ball.speedX = -ball.speedX;
        if (diffX > 0.0f)
            ball.posX += overlapX;
        else
            ball.posX -= overlapX;
    } else {
        // vertical
        ball.speedY = -ball.speedY;
        if (diffY > 0.0f)
            ball.posY += overlapY;
        else
            ball.posY -= overlapY;
    }
}

// AABB build (platform) - ball collision
void PlayingState::CheckBuildCollision()
{
    float minX = build.startX;
    float minY = build.startY;
    float maxX = minX + build.cellSize * build.cols;
    float maxY = minY + build.cellSize * build.rows;

    // Closest point on AABB to the ball's center
    float closestX = fmax(minX, fmin(ball.posX, maxX));
    float closestY = fmax(minY, fmin(ball.posY, maxY));

    // Check for intersection
    float dx = ball.posX - closestX;
    float dy = ball.posY - closestY;
    float dist2 = dx*dx + dy*dy;
    if (dist2 > ball.radius * ball.radius) return; // no collision

    float centerX = (minX + maxX) / 2.0f;
    float centerY = (minY + maxY) / 2.0f;
    float diffX = ball.posX - centerX;
    float diffY = ball.posY - centerY;

    // Compute overlap amounts along each axis (positive = penetration)
    float halfW = build.cellSize * build.cols * 0.5f;
    float halfH = build.cellSize * build.rows * 0.5f;
    float overlapX = (halfW + ball.radius) - std::abs(diffX);
    float overlapY = (halfH + ball.radius) - std::abs(diffY);

    // Decide axis of response by smaller overlap (penetration)
    if (overlapX < overlapY) {
        // horizontal response
        ball.speedX = -ball.speedX;
        if (diffX > 0.0f)
            ball.posX += overlapX;
        else
            ball.posX -= overlapX;
    } else {
        if (ball.posY < centerY) {
            return;
        }
        // vertical
        // formula din cerinta
        float unghi = (ball.posX - centerX) / (halfW);
        float viteza_bila = sqrtf(ball.speedX * ball.speedX + ball.speedY * ball.speedY);
        float v_x_bila = viteza_bila * sinf(unghi);
        float v_y_bila = viteza_bila * cosf(unghi);

        ball.speedX = v_x_bila;
        ball.speedY = v_y_bila;

        if (diffY > 0.0f)
            ball.posY += overlapY;
        else
            ball.posY -= overlapY;
    }
}

/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void PlayingState::OnInputUpdate(float deltaTime, int mods)
{
    if (game->window->KeyHold(GLFW_KEY_LEFT)) {
        float newX = build.startX - 700 * deltaTime;
        if (newX > 0)
            build.startX = newX;
        else build.startX = 0;
        if (!gameStarted)
            ball.posX = build.startX + build.cellSize * build.cols / 2;
    }
    if (game->window->KeyHold(GLFW_KEY_RIGHT)) {
        float newX = build.startX + 700 * deltaTime;
        if (newX + build.cols * build.cellSize < game->initialResolution.x)
            build.startX = newX;
        else build.startX = game->initialResolution.x - build.cols * build.cellSize;
        if (!gameStarted)
            ball.posX = build.startX + build.cellSize * build.cols / 2;
    }
}

void PlayingState::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_Q) {
        // switch back to editing
        game->ChangeState(new EditingState(game));
    }
    if (key == GLFW_KEY_SPACE) {
        // launch the ball
        if (!gameStarted) {
            ball.speedX = 300.0f;
            ball.speedY = 300.0f;
            gameStarted = true;
        }
    }
}


void PlayingState::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{

}


void PlayingState::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{

}


void PlayingState::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{

}


void PlayingState::OnWindowResize(int width, int height)
{
    textRenderer = new gfxc::TextRenderer(game->window->props.selfDir,
            game->window->GetResolution().x, game->window->GetResolution().y);
    textRenderer->Load(PATH_JOIN(game->window->props.selfDir, RESOURCE_PATH::FONTS, "Hack-Bold.ttf"), 24);
}
