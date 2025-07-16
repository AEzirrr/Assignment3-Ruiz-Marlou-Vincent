#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class LineRenderer {
public:
    LineRenderer(GLuint shaderProgram);
    ~LineRenderer();

    void DrawLine(const glm::vec3& start, const glm::vec3& end);

private:
    GLuint VAO, VBO;
    GLuint shader;
};
