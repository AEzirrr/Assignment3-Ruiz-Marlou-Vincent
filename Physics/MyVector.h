#pragma once
#include <glm/glm.hpp>

namespace Physics {
    class MyVector {
    public:
        float x, y, z;

        MyVector();
        MyVector(float _x, float _y, float _z);

        explicit operator glm::vec3() const;

        float Magnitude() const;
        MyVector Normalize() const;

        MyVector operator+(const MyVector& rhs) const;
        void operator+=(const MyVector rhs);

        MyVector operator-(const MyVector& rhs) const;
        void operator-=(const MyVector rhs);

        MyVector operator*(float scalar) const;

        MyVector ComponentProduct(const MyVector& rhs) const;
        float ScalarProduct(const MyVector& rhs) const;
        MyVector VectorProduct(const MyVector& rhs) const;
    };
}
