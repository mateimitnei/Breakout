#include "lab_m1/Tema1/grid.h"
#include <cmath>
#include <algorithm>
#include <queue>
#include <iostream>
#include <climits>

using namespace std;
using namespace m1;


void Grid::Init(int resX, int resY) {
    cellSize = 60;
    startX = 280;
    startY = 70;
    width = resX / 1.5f;
    height = resY / 1.4f;
    cols = fmax(width / (int)cellSize, 5);
    rows = fmax(height / (int)cellSize, 5);
    width = cols * cellSize;
    height = rows * cellSize;
    // allocate cells
    cells.clear();
    cells.resize(rows, std::vector<Cell>(cols));
}

glm::vec2 Grid::CursorOnCell(const glm::vec2 &cursorPos) {
    float relativeX = cursorPos.x - startX;
    float relativeY = cursorPos.y - startY;
    if (relativeX < 0.0f || relativeY < 0.0f)
        return glm::vec2(-1, -1);

    int col = (int)std::floor(relativeX / cellSize);
    int row = (int)std::floor(relativeY / cellSize);
    if (col < 0 || col >= cols || row < 0 || row >= rows)
        return glm::vec2(-1, -1);

    float insideX = relativeX - (col * cellSize);
    float insideY = relativeY - (row * cellSize);
    if (insideX < 0.0f || insideX > cellSize 
        || insideY < 0.0f || insideY > cellSize)
        return glm::vec2(-1, -1);

    return glm::vec2(row, col);
}

// checks if the newly placed item has at least one direct neighbour
bool Grid::CheckBuildShape() {
    if (itemsNum == 0) return true;

    int maxR = -1, minC = INT_MAX, maxC = -1;

    int sr = -1, sc = -1;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (cells[r][c].occupied) {
                if (sr == -1) {
                    sr = r; sc = c; 
                }
                maxR = fmax(maxR, r);
                minC = fmin(minC, c);
                maxC = fmax(maxC, c);
            }
        }
    }

    // Check if the top part of the ship is flat (and of maximum width)
    for (int i = minC; i <= maxC; i++)
        if (!cells[maxR][i].occupied) return false;

    // bfs from (sr, sc)
    std::vector<std::vector<char>> visited(rows, std::vector<char>(cols, 0));
    std::queue<std::pair<int,int>> q;
    q.push({sr, sc});
    visited[sr][sc] = 1;
    int reached = 0;

    const int dr[4] = { -1, 1, 0, 0 };
    const int dc[4] = { 0, 0, -1, 1 };

    while (!q.empty()) {
        std::pair<int,int> p = q.front();
        q.pop();

        if (!cells[p.first][p.second].occupied) continue;
        reached++;

        for (int k = 0; k < 4; k++) {
            int nr = p.first + dr[k];
            int nc = p.second + dc[k];
            if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) continue;
            if (visited[nr][nc] || !cells[nr][nc].occupied) continue;
            visited[nr][nc] = 1;
            q.push({nr, nc});
        }
    }

    // connected if every occupied cell has been reached
    return (reached == itemsNum);
}