#include "Vec4.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace TripleEngineCore::TripleMath {

    Vec4 Vec4::operator+(const Vec4& other) const {
        glm::vec4 a(x, y, z, w);
        glm::vec4 b(other.x, other.y, other.z, other.w);
        glm::vec4 r = a + b;
        return Vec4(r.x, r.y, r.z, r.w);
    }

    Vec4 Vec4::operator-(const Vec4& other) const {
        glm::vec4 a(x, y, z, w);
        glm::vec4 b(other.x, other.y, other.z, other.w);
        glm::vec4 r = a - b;
        return Vec4(r.x, r.y, r.z, r.w);
    }

    Vec4 Vec4::operator*(float scalar) const {
        glm::vec4 a(x, y, z, w);
        glm::vec4 r = a * scalar;
        return Vec4(r.x, r.y, r.z, r.w);
    }

    Vec4 Vec4::operator/(float scalar) const {
        glm::vec4 a(x, y, z, w);
        glm::vec4 r = a / scalar;
        return Vec4(r.x, r.y, r.z, r.w);
    }

    Vec4& Vec4::operator+=(const Vec4& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
        return *this;
    }

    Vec4& Vec4::operator-=(const Vec4& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
        return *this;
    }

    Vec4& Vec4::operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        w *= scalar;
        return *this;
    }

    Vec4& Vec4::operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        w /= scalar;
        return *this;
    }

    float Vec4::length() const {
        return glm::length(glm::vec4(x, y, z, w));
    }

    Vec4 Vec4::normalized() const {
        glm::vec4 r = glm::normalize(glm::vec4(x, y, z, w));
        return Vec4(r.x, r.y, r.z, r.w);
    }

} // namespace TripleEngineCore::TripleMath
