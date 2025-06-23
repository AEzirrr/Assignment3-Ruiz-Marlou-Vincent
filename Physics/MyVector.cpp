#include "MyVector.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

namespace Physics {
    MyVector::MyVector() : x(0), y(0), z(0) {}

    MyVector::MyVector(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

    MyVector::operator glm::vec3() const {
        return glm::vec3(x, y, z);
    }

    float MyVector::Magnitude() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    MyVector MyVector::Normalize() const {
        float mag = Magnitude();
        if (mag == 0.0f) {
            return MyVector(0.0f, 0.0f, 0.0f);
        }
        return MyVector(x / mag, y / mag, z / mag);
    }


    MyVector MyVector::operator+(const MyVector& rhs) const {
        return MyVector(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    void MyVector::operator+=(const MyVector rhs) {
        this->x += rhs.x;
        this->y += rhs.y;
        this->z += rhs.z;
    }

    MyVector MyVector::operator-(const MyVector& rhs) const {
        return MyVector(x - rhs.x, y - rhs.y, z - rhs.z);
    }

    void MyVector::operator-=(const MyVector rhs) {
        this->x -= rhs.x;
        this->y -= rhs.y;
        this->z -= rhs.z;
    }

    MyVector MyVector::operator*(float scalar) const {
        return MyVector(x * scalar, y * scalar, z * scalar);
    }

    MyVector MyVector::ComponentProduct(const MyVector& rhs) const {
        return MyVector(x * rhs.x, y * rhs.y, z * rhs.z);
    }

    float MyVector::ScalarProduct(const MyVector& rhs) const {
        return x * rhs.x + y * rhs.y + z * rhs.z;
    }

    MyVector MyVector::VectorProduct(const MyVector& rhs) const {
        return MyVector(
            y * rhs.z - z * rhs.y,
            z * rhs.x - x * rhs.z,
            x * rhs.y - y * rhs.x
        );
    }
}
