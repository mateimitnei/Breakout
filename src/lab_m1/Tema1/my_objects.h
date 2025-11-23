#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace m1
{
    // custom play button mesh
    Mesh* PlayButton(const std::string &name, glm::vec3 leftBottomCorner, float length, glm::vec3 color);
    Mesh* CreateBall(const std::string& name, const glm::vec3 &color, float radius, int smoothness);
    Mesh* CreateHeart(const std::string &name, const glm::vec3 &color, float size, int smoothness);
}
