#include "triple/core/input/InputSystem.h"

#include <unordered_map>

#include <triple/log/Logger.h>
#include "triple/core/input/KeyCode.h"
#include "triple/core/input/MouseButton.h"

namespace triple::core {
	struct InputSystem::Impl {
		std::unordered_map<KeyCode, bool> m_keys;
		std::unordered_map<KeyCode, bool> m_keysPressed;
		std::unordered_map<KeyCode, bool> m_keysReleased;

		std::unordered_map<MouseButton, bool> m_mouseButtons;
		std::unordered_map<MouseButton, bool> m_mouseButtonsPressed;
		std::unordered_map<MouseButton, bool> m_mouseButtonsReleased;
	};

	InputSystem::InputSystem()
	    : m_impl(new Impl()), m_mouseX(0), m_mouseY(0), m_lastMouseX(0), m_lastMouseY(0),
	      m_mouseDelta(0, 0), m_firstMouse(true) {}

	InputSystem::~InputSystem() { delete m_impl; }

	void InputSystem::init() {
		m_impl->m_keys.clear();
		m_impl->m_keysPressed.clear();

		m_mouseX = m_mouseY = 0.0f;
		m_lastMouseX = m_lastMouseY = 0.0f;
		m_mouseDelta = triple::math::Vec2(0, 0);
		m_firstMouse = true;
	}

	void InputSystem::update(float /*dt*/) {
		m_impl->m_keysPressed.clear();
		m_impl->m_keysReleased.clear();

		m_impl->m_mouseButtonsPressed.clear();
		m_impl->m_mouseButtonsReleased.clear();

		m_mouseDelta = triple::math::Vec2(0, 0);
	}

	void InputSystem::onKeyboard(KeyboardInputEvent &ke) {
		auto key = ke.getKey();

		if (ke.isPressed()) {
			if (ke.handled)
				return;

			if (!m_impl->m_keys[key])
				m_impl->m_keysPressed[key] = true;

			m_impl->m_keys[key] = true;
		} else if (ke.isReleased()) {
			m_impl->m_keys[key] = false;

			if (!ke.handled)
				m_impl->m_keysReleased[key] = true;
		}
	}

	void InputSystem::onMouseMove(MouseMoveEvent &me) {
		float currentX = me.getX();
		float currentY = me.getY();

		if (m_firstMouse) {
			m_mouseX = currentX;
			m_mouseY = currentY;
			m_firstMouse = false;
			m_mouseDelta = triple::math::Vec2(0, 0);
			return;
		}

		if (me.handled) {
			m_mouseDelta = triple::math::Vec2(0, 0);
		} else {
			m_mouseDelta = triple::math::Vec2(currentX - m_mouseX, m_mouseY - currentY);
		}

		m_mouseX = currentX;
		m_mouseY = currentY;
	}

	void InputSystem::onMouseButton(MouseButtonEvent &me) {
		auto btn = me.getButton();

		if (me.isPressed()) {
			if (me.handled)
				return;

			m_impl->m_mouseButtons[btn] = true;
			m_impl->m_mouseButtonsPressed[btn] = true;
		} else if (me.isReleased()) {
			m_impl->m_mouseButtons[btn] = false;

			if (!me.handled)
				m_impl->m_mouseButtonsReleased[btn] = true;
		}
	}

	bool InputSystem::isKeyDown(KeyCode key) const {
		auto it = m_impl->m_keys.find(key);
		if (it == m_impl->m_keys.end())
			return false;

		return it->second;
	}

	bool InputSystem::isKeyPressed(KeyCode key) const {
		auto it = m_impl->m_keysPressed.find(key);
		if (it == m_impl->m_keysPressed.end())
			return false;

		return it->second;
	}

	bool InputSystem::isKeyReleased(KeyCode key) const {
		auto it = m_impl->m_keysReleased.find(key);
		if (it == m_impl->m_keysReleased.end())
			return false;

		return it->second;
	}

	bool InputSystem::isMouseButtonDown(MouseButton btn) const {
		auto it = m_impl->m_mouseButtons.find(btn);
		if (it == m_impl->m_mouseButtons.end())
			return false;

		return it->second;
	}

	bool InputSystem::isMouseButtonPressed(MouseButton btn) const {
		auto it = m_impl->m_mouseButtonsPressed.find(btn);
		if (it == m_impl->m_mouseButtonsPressed.end())
			return false;

		return it->second;
	}

	bool InputSystem::isMouseButtonReleased(MouseButton btn) const {
		auto it = m_impl->m_mouseButtonsReleased.find(btn);
		if (it == m_impl->m_mouseButtonsReleased.end())
			return false;

		return it->second;
	}

	triple::math::Vec2 InputSystem::getMouseDelta() const { return m_mouseDelta; }
} // namespace triple::core
