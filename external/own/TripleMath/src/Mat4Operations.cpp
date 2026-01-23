#include "Mat4Operations.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace TripleEngineCore::TripleMath {

    static glm::mat4 toGlm(const Mat4& m) {
        return glm::make_mat4(m.data);
    }

    static Mat4 fromGlm(const glm::mat4& m) {
        Mat4 out;
        memcpy(out.data, &m[0][0], sizeof(float) * 16);
        return out;
    }

    Mat4 translate(const Vec3& v) {
        glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(v.x, v.y, v.z));
        return fromGlm(m);
    }

    Mat4 scale(const Vec3& v) {
        glm::mat4 m = glm::scale(glm::mat4(1.0f), glm::vec3(v.x, v.y, v.z));
        return fromGlm(m);
    }

    Mat4 rotate(float radians, const Vec3& axis) {
        glm::mat4 m = glm::rotate(glm::mat4(1.0f), radians, glm::vec3(axis.x, axis.y, axis.z));
        return fromGlm(m);
    }

    Mat4 lookAt(const Vec3& eye, const Vec3& target, const Vec3& up) {
        glm::mat4 m = glm::lookAt(
            glm::vec3(eye.x, eye.y, eye.z),
            glm::vec3(target.x, target.y, target.z),
            glm::vec3(up.x, up.y, up.z)
        );
        return fromGlm(m);
    }

    Mat4 perspective(float fov, float aspect, float nearP, float farP) {
        glm::mat4 m = glm::perspective(fov, aspect, nearP, farP);
        return fromGlm(m);
    }

    Mat4 ortho(float left, float right, float bottom, float top, float nearP, float farP) {
        glm::mat4 m = glm::ortho(left, right, bottom, top, nearP, farP);
        return fromGlm(m);
    }

    Mat4 multiply(const Mat4& a, const Mat4& b) {
        glm::mat4 ma = toGlm(a);
        glm::mat4 mb = toGlm(b);
        return fromGlm(ma * mb);
    }

}
