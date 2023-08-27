#include "func2D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"

using namespace std;

Mesh* func2D::CreateCircle(
    const string& name,
    glm::vec3 center,
    float radius,
    glm::vec3 color,
    bool fill)
{
    vector<VertexFormat> vertices;
    vertices.push_back(VertexFormat(center, color));
    float delta = 2.0f * 3.14159f / 360;
    for (int i = 0; i < 360; i++) {
        vertices.push_back(VertexFormat(center + glm::vec3(radius * cos(i * delta), radius * sin(i * delta), 0), color));
    }

    Mesh* circle = new Mesh(name);

    vector<unsigned int> indices;

    if (!fill) {
        circle->SetDrawMode(GL_LINE_LOOP);
        //Se adauga indicii daca se deseneaza doar conturul unui cerc
        for (int i = 1; i <= 360; i++) {
            indices.push_back(i);
        }
    }
    else {
        circle->SetDrawMode(GL_TRIANGLES);
        //Altfel, se adauga indicii pentru desenarea de triunghiuri, daca va fi un cerc plin
        for (int i = 1; i < 360; i++) {
            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i + 1);
        }
        indices.push_back(0);
        indices.push_back(360);
        indices.push_back(1);
    }
    circle->InitFromData(vertices, indices);
    return circle;
}


Mesh* func2D::CreateRectangle(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float x_l,
    float y_l,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(x_l, 0, 0), color),
        VertexFormat(corner + glm::vec3(x_l, y_l, 0), color),
        VertexFormat(corner + glm::vec3(0, y_l, 0), color)
    };

    Mesh* square = new Mesh(name);
    vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}