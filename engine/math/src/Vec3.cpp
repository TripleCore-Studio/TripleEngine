#include "triple/math/Vec3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace triple::math {

    Vec3 Vec3::operator+(const Vec3& other) const {
        glm::vec3 a(x, y, z);
        glm::vec3 b(other.x, other.y, other.z);
        glm::vec3 r = a + b;
        return Vec3(r.x, r.y, r.z);
    }

    Vec3 Vec3::operator-(const Vec3& other) const {
        glm::vec3 a(x, y, z);
        glm::vec3 b(other.x, other.y, other.z);
        glm::vec3 r = a - b;
        return Vec3(r.x, r.y, r.z);
    }

    Vec3 Vec3::operator*(float scalar) const {
        glm::vec3 a(x, y, z);
        glm::vec3 r = a * scalar;
        return Vec3(r.x, r.y, r.z);
    }

    Vec3 Vec3::operator/(float scalar) const {
        glm::vec3 a(x, y, z);
        glm::vec3 r = a / scalar;
        return Vec3(r.x, r.y, r.z);
    }

    Vec3& Vec3::operator+=(const Vec3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    Vec3& Vec3::operator-=(const Vec3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    Vec3& Vec3::operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    Vec3& Vec3::operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    float Vec3::length() const {
        return glm::length(glm::vec3(x, y, z));
    }

    Vec3 Vec3::normalized() const {
        glm::vec3 r = glm::normalize(glm::vec3(x, y, z));
        return Vec3(r.x, r.y, r.z);
    }

} // namespace TripleEngineCore::TripleMath
