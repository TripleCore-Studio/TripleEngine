#include "Mat4.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace TripleEngineCore::TripleMath {

    static glm::mat4 toGlm(const Mat4& m) {
        return glm::make_mat4(m.data);
    }

    static Mat4 fromGlm(const glm::mat4& m) {
        Mat4 out;
        memcpy(out.data, glm::value_ptr(m), sizeof(float) * 16);
        return out;
    }

    Mat4::Mat4() {
        glm::mat4 m(1.0f);
        memcpy(data, &m[0][0], sizeof(float) * 16);
    }

    Mat4::Mat4(float diagonal) {
        glm::mat4 m(diagonal);
        memcpy(data, &m[0][0], sizeof(float) * 16);
    }

    Mat4::Mat4(float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33)
    {
        data[0] = m00; data[1] = m01; data[2] = m02; data[3] = m03;
        data[4] = m10; data[5] = m11; data[6] = m12; data[7] = m13;
        data[8] = m20; data[9] = m21; data[10] = m22; data[11] = m23;
        data[12] = m30; data[13] = m31; data[14] = m32; data[15] = m33;
    }

    Mat4 Mat4::identity() {
        return Mat4(1.0f);
    }

    Vec4 Mat4::operator*(const Vec4& v) const {
        glm::mat4 m = toGlm(*this);
        glm::vec4 vec(v.x, v.y, v.z, v.w);
        glm::vec4 res = m * vec;
        return Vec4(res.x, res.y, res.z, res.w);
    }

    Mat4 Mat4::operator*(const Mat4& other) const {
        glm::mat4 m1 = toGlm(*this);
        glm::mat4 m2 = toGlm(other);
        return fromGlm(m1 * m2);
    }
}