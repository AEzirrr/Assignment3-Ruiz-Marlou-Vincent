#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

#include <string>
#include <vector>

class Model3D {
public:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    GLuint shader;
    glm::vec3 modelColor;

    std::vector<GLfloat> fullVertexData;
    GLuint VAO, VBO, texture;

    Model3D(const std::string& modelPath, const std::string& texturePath, glm::vec3 pos, glm::vec3 rot, glm::vec3 scl, GLuint shaders);

    void color(glm::vec3 color);
    void draw();
    glm::mat4 getTransform();

private:
    void loadModel(const std::string& path);
    void loadTexture(const std::string& path);
    void setupBuffers();
};
