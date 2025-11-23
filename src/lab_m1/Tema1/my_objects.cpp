#include "my_objects.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"


Mesh* m1::PlayButton(
    const std::string &name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length * 0.75f, length * 0.5f, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color)
    };

    // triangle fan
    Mesh* playBtn = new Mesh(name);
    std::vector<unsigned int> indices = {
        0, 1, 2,
        0, 2, 3,
        0, 3, 4
    };

    playBtn->InitFromData(vertices, indices);
    return playBtn;
}


Mesh* m1::CreateBall(const std::string& name, const glm::vec3 &color, float radius, int smoothness)
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    // center
    vertices.push_back(VertexFormat(glm::vec3(0.0f, 0.0f, 0.0f), color));

    for (int i = 0; i <= smoothness; ++i) {
        float angle = 2.0f * M_PI * float(i) / float(smoothness);
        float x = cosf(angle) * radius;
        float y = sinf(angle) * radius;
        vertices.push_back(VertexFormat(glm::vec3(x, y, 0.0f), color));
    }

    // triangle fan indices
    for (int i = 1; i <= smoothness; ++i) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    Mesh* mesh = new Mesh(name);
    mesh->InitFromData(vertices, indices);
    return mesh;
}

Mesh* m1::CreateHeart(const std::string& name, const glm::vec3 &color, float size, int smoothness)
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    // center vertex
    vertices.push_back(VertexFormat(glm::vec3(0.0f, 0.0f, 0.0f), color));

    // Heart curve
    // x = 16 * sin^3(t)
    // y = 13 * cos(t) - 5 * cos(2t) - 2 * cos(3t) - cos(4t)
    const float PI = M_PI;
    float scale = size * 0.01f;

    for (int i = 0; i <= smoothness; ++i) {
        float t = 2.0f * PI * float(i) / float(smoothness);
        float x = 16.0f * sinf(t) * sinf(t) * sinf(t);
        float y = 13.0f * cosf(t) - 5.0f * cosf(2.0f * t) - 2.0f * cosf(3.0f * t) - cosf(4.0f * t);
        vertices.push_back(VertexFormat(glm::vec3(x * scale, y * scale, 0.0f), color));
    }

    // triangle fan indices
    for (int i = 1; i <= smoothness; ++i) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    Mesh* mesh = new Mesh(name);
    mesh->InitFromData(vertices, indices);
    mesh->SetDrawMode(GL_TRIANGLES);
    return mesh;
}