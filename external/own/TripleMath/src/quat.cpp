#include "Quat.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace TripleEngineCore::TripleMath {
    Quat Quat::fromEulerAngles(const Vec3& eulerDeg) {
        glm::vec3 eulerRad = glm::radians(glm::vec3(eulerDeg.x, eulerDeg.y, eulerDeg.z));
        glm::quat q = glm::quat(eulerRad);
        return Quat(q.w, q.x, q.y, q.z);
    }

    Quat Quat::operator*(const Quat& rhs) const {
        glm::quat q1(w, x, y, z);
        glm::quat q2(rhs.w, rhs.x, rhs.y, rhs.z);
        glm::quat r = q1 * q2;
        return Quat(r.w, r.x, r.y, r.z);
    }

    Quat Quat::normalized() const {
        glm::quat q(w, x, y, z);
        glm::quat r = glm::normalize(q);
        return Quat(r.w, r.x, r.y, r.z);
    }

    Vec3 Quat::toEulerAngles() const {
        glm::quat q(w, x, y, z);
        glm::vec3 eulerRad = glm::eulerAngles(q);
        glm::vec3 eulerDeg = glm::degrees(eulerRad);
        return Vec3(eulerDeg.x, eulerDeg.y, eulerDeg.z);
    }
}