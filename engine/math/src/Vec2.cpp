#include "triple/math/Vec2.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace triple::math {
	Vec2 Vec2::operator+(const Vec2 &other) const {
		glm::vec2 a(x, y);
		glm::vec2 b(other.x, other.y);
		glm::vec2 r = a + b;
		return Vec2(r.x, r.y);
	}

	Vec2 Vec2::operator-(const Vec2 &other) const {
		glm::vec2 a(x, y);
		glm::vec2 b(other.x, other.y);
		glm::vec2 r = a - b;
		return Vec2(r.x, r.y);
	}

	Vec2 Vec2::operator*(float scalar) const {
		glm::vec2 a(x, y);
		glm::vec2 r = a * scalar;
		return Vec2(r.x, r.y);
	}

	Vec2 Vec2::operator/(float scalar) const {
		glm::vec2 a(x, y);
		glm::vec2 r = a / scalar;
		return Vec2(r.x, r.y);
	}

	Vec2 &Vec2::operator+=(const Vec2 &other) {
		*this = *this + other;
		return *this;
	}

	Vec2 &Vec2::operator-=(const Vec2 &other) {
		*this = *this - other;
		return *this;
	}

	Vec2 &Vec2::operator*=(float scalar) {
		*this = *this * scalar;
		return *this;
	}

	Vec2 &Vec2::operator/=(float scalar) {
		*this = *this / scalar;
		return *this;
	}

	float Vec2::length() const {
		glm::vec2 a(x, y);
		return glm::length(a);
	}

	Vec2 Vec2::normalized() const {
		glm::vec2 a(x, y);
		glm::vec2 r = glm::normalize(a);
		return Vec2(r.x, r.y);
	}
} // namespace triple::math