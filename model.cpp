#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "model.h"
#include <iostream>

Model3D::Model3D(const std::string& modelPath, const std::string& texturePath, glm::vec3 pos, glm::vec3 rot, glm::vec3 scl, GLuint shaders)
    : position(pos), rotation(rot), scale(scl), shader(shaders), modelColor(glm::vec3(1.0f)) {
    loadModel(modelPath);
    loadTexture(texturePath);
    setupBuffers();
}

void Model3D::color(glm::vec3 color) {
    modelColor = color;
}

void Model3D::loadModel(const std::string& path) {
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warning, error;

    tinyobj::attrib_t attributes;

    bool success = tinyobj::LoadObj(&attributes, &shapes, &materials, &warning, &error, path.c_str());

    if (!success || shapes.empty()) {
        std::cerr << "Failed to load model: " << error << std::endl;
        return;
    }

    for (size_t i = 0; i < shapes[0].mesh.indices.size(); ++i) {
        tinyobj::index_t vData = shapes[0].mesh.indices[i];

        fullVertexData.push_back(attributes.vertices[vData.vertex_index * 3 + 0]);
        fullVertexData.push_back(attributes.vertices[vData.vertex_index * 3 + 1]);
        fullVertexData.push_back(attributes.vertices[vData.vertex_index * 3 + 2]);

        fullVertexData.push_back(attributes.normals[vData.normal_index * 3 + 0]);
        fullVertexData.push_back(attributes.normals[vData.normal_index * 3 + 1]);
        fullVertexData.push_back(attributes.normals[vData.normal_index * 3 + 2]);

        fullVertexData.push_back(attributes.texcoords[vData.texcoord_index * 2 + 0]);
        fullVertexData.push_back(attributes.texcoords[vData.texcoord_index * 2 + 1]);
    }
}

void Model3D::loadTexture(const std::string& path) {
	if (path.empty()) {
		texture = 0; // No texture
		return;
	}
    else {
        stbi_set_flip_vertically_on_load(true);
        int img_width, img_height, color_channels;
        unsigned char* tex_bytes = stbi_load(path.c_str(), &img_width, &img_height, &color_channels, 0);

        if (!tex_bytes) {
            std::cerr << "Failed to load texture: " << path << std::endl;
            return;
        }

        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_bytes);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(tex_bytes);
    }
}

void Model3D::setupBuffers() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, fullVertexData.size() * sizeof(GLfloat), fullVertexData.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

glm::mat4 Model3D::getTransform() {
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
    transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    transform = glm::scale(transform, scale);
    return transform;
}

void Model3D::draw() {
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO);

    unsigned int transformLoc = glGetUniformLocation(shader, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(getTransform()));

    int colorLoc = glGetUniformLocation(shader, "modelColor");
    if (colorLoc != -1) {
        glUniform3fv(colorLoc, 1, glm::value_ptr(this->modelColor));
    }

    glDrawArrays(GL_TRIANGLES, 0, fullVertexData.size() / 8);
}
