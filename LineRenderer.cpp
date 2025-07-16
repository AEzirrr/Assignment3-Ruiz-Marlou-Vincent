#include "LineRenderer.h"

LineRenderer::LineRenderer(GLuint shaderProgram) : shader(shaderProgram) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
}

LineRenderer::~LineRenderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void LineRenderer::DrawLine(const glm::vec3& start, const glm::vec3& end) {
    glm::vec3 vertices[2] = { start, end };

    //glUseProgram(shader);

    glUniform1i(glGetUniformLocation(shader, "isLine"), 1);

    glm::mat4 identity = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader, "transform"), 1, GL_FALSE, glm::value_ptr(identity));
    //glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));
    //glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_LINES, 0, 2);

    //glBindVertexArray(0);
    //glUseProgram(0);
}
