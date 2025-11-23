#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace m1 {

    class Grid {
    public:
        // layout
        int cols;
        int rows;
        float cellSize;
        float startX;
        float startY;
        float width;
        float height;
        int itemsNum = 0; // number of items placed in the grid

        struct Cell {
            bool occupied = false;
        };

        std::vector<std::vector<Cell>> cells;

        Grid() {};

        // initialize grid and allocate cells
        void Init(int width, int height);

        glm::vec2 CursorOnCell(const glm::vec2 &cursorPos);

        bool CheckBuildShape();
    };

} // namespace m1