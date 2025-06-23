#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Camera class
class MyCamera {
protected:
    glm::vec3 front; 
    glm::vec3 up;    
    float yaw;       
    float pitch;    
    float distance;  

public:
    glm::vec3 position;

    // Constructor
    MyCamera(const glm::vec3& pos, const glm::vec3& frontDir, const glm::vec3& upDir, float y, float p, float dist)
        : position(pos), front(frontDir), up(upDir), yaw(y), pitch(p), distance(dist) {
        UpdateCameraVectors();
    }

    virtual ~MyCamera() = default;

    // Pure virtual function to get the projection matrix
    virtual glm::mat4 GetProjectionMatrix(float aspect) const = 0;

    // Function to get the view matrix
    glm::mat4 GetViewMatrix(const glm::vec3& target) const {
        return glm::lookAt(position, target, up);
    }

    // Function to process mouse movement
    void ProcessMouseMovement(float xoffset, float yoffset, float sensitivity = 0.1f) {
        yaw += xoffset * sensitivity;
        pitch += yoffset * sensitivity;

        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        UpdateCameraVectors();
    }

    // Function to update the camera position
    void UpdateCameraPosition(const glm::vec3& target) {
        position.x = target.x + distance * cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        position.y = target.y + distance * sin(glm::radians(pitch));
        position.z = target.z + distance * sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    }

private:
    // Function to update the camera vectors
    void UpdateCameraVectors() {
        glm::vec3 newFront;
        newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newFront.y = sin(glm::radians(pitch));
        newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(newFront);
    }
};

// Perspective camera class
class PerspectiveCamera : public MyCamera {
private:
    float fov;
    float nearPlane;
    float farPlane;

public:
    // Constructor
    explicit PerspectiveCamera(const glm::vec3& pos, const glm::vec3& frontDir, const glm::vec3& upDir, float y, float p, float dist, float fovAngle, float nearP = 0.1f, float farP = 100.0f)
        : MyCamera(pos, frontDir, upDir, y, p, dist), fov(fovAngle), nearPlane(nearP), farPlane(farP) {
    }

    // Function to get the projection matrix
    glm::mat4 GetProjectionMatrix(float aspect) const override {
        return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
    }
};

// Orthographic camera class
class OrthographicCamera : public MyCamera {
private:
    float orthoSize;
    float nearPlane;
    float farPlane;

public:
    // Constructor
    explicit OrthographicCamera(const glm::vec3& pos, const glm::vec3& frontDir, const glm::vec3& upDir, float y, float p, float dist, float size, float nearP = -50.0f, float farP = 50.0f)
        : MyCamera(pos, frontDir, upDir, y, p, dist), orthoSize(size), nearPlane(nearP), farPlane(farP) {
    }

    // Function to get the projection matrix
    glm::mat4 GetProjectionMatrix(float aspect) const override {
        return glm::ortho(-orthoSize * aspect, orthoSize * aspect, -orthoSize, orthoSize, nearPlane, farPlane);
    }
};
