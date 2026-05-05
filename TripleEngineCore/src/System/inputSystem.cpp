#include "System/InputSystem.h"
#include <unordered_map>

#include "TLogger.h"
#include "Input/KeyCode.h"
#include "Input/MouseButton.h"

namespace TripleEngineCore::System {
	struct InputSystem::Impl
	{
		std::unordered_map<Input::KeyCode, bool> m_keys;
		std::unordered_map<Input::KeyCode, bool> m_keysPressed;
		std::unordered_map<Input::KeyCode, bool> m_keysReleased;

		std::unordered_map<Input::MouseButton, bool> m_mouseButtons;
		std::unordered_map<Input::MouseButton, bool> m_mouseButtonsPressed;
		std::unordered_map<Input::MouseButton, bool> m_mouseButtonsReleased;
	};

	InputSystem::InputSystem() : 
		m_impl(new Impl()),
		m_mouseX(0),
		m_mouseY(0),
		m_lastMouseX(0),
		m_lastMouseY(0),
		m_mouseDelta(0, 0),
		m_firstMouse(true)
	{}

	InputSystem::~InputSystem() { delete m_impl; }

	void InputSystem::init()
	{
		m_impl->m_keys.clear();
		m_impl->m_keysPressed.clear();

		m_mouseX = m_mouseY = 0.0f;
		m_lastMouseX = m_lastMouseY = 0.0f;
		m_mouseDelta = TripleMath::Vec2(0, 0);
		m_firstMouse = true;
	}

	void InputSystem::update(float /*dt*/)
	{
		m_impl->m_keysPressed.clear();
		m_impl->m_keysReleased.clear();

		m_impl->m_mouseButtonsPressed.clear();
		m_impl->m_mouseButtonsReleased.clear();

		m_mouseDelta = TripleMath::Vec2(0, 0);
	}

	void InputSystem::onKeyboard(Event::KeyboardInputEvent& ke)
	{
		auto key = ke.getKey();

		if (ke.isPressed())
		{
			if (!m_impl->m_keys[key])
				m_impl->m_keysPressed[key] = true;

			m_impl->m_keys[key] = true;
		}
		else if (ke.isReleased())
		{
			m_impl->m_keys[key] = false;
			m_impl->m_keysReleased[key] = true;
		}
	}

	void InputSystem::onMouseMove(Event::MouseMoveEvent& me)
	{
		if (m_firstMouse) {
			m_mouseX = m_lastMouseX = me.getX();
			m_mouseY = m_lastMouseY = me.getY();
			m_firstMouse = false;
		}
		else {
			m_mouseDelta = TripleMath::Vec2(
				me.getX() - m_mouseX,
				m_mouseY - me.getY()
			);
			m_mouseX = me.getX();
			m_mouseY = me.getY();
		}
	}

	void InputSystem::onMouseButton(Event::MouseButtonEvent& me)
	{
		auto btn = me.getButton();

		if (me.isPressed())
		{
			m_impl->m_mouseButtons[btn] = true;
			m_impl->m_mouseButtonsPressed[btn] = true;
		}
		else if (me.isReleased())
		{
			m_impl->m_mouseButtons[btn] = false;
			m_impl->m_mouseButtonsReleased[btn] = true;
		}
	}

	bool InputSystem::isKeyDown(Input::KeyCode key) const
	{
		auto it = m_impl->m_keys.find(key);
		if (it == m_impl->m_keys.end())
			return false;

		return it->second;
	}

	bool InputSystem::isKeyPressed(Input::KeyCode key) const
	{
		auto it = m_impl->m_keysPressed.find(key);
		if (it == m_impl->m_keysPressed.end())
			return false;

		return it->second;
	}

	bool InputSystem::isKeyReleased(Input::KeyCode key) const
	{
		auto it = m_impl->m_keysReleased.find(key);
		if (it == m_impl->m_keysReleased.end())
			return false;

		return it->second;
	}

	bool InputSystem::isMouseButtonDown(Input::MouseButton btn) const
	{
		auto it = m_impl->m_mouseButtons.find(btn);
		if (it == m_impl->m_mouseButtons.end())
			return false;

		return it->second;
	}

	bool InputSystem::isMouseButtonPressed(Input::MouseButton btn) const
	{
		auto it = m_impl->m_mouseButtonsPressed.find(btn);
		if (it == m_impl->m_mouseButtonsPressed.end())
			return false;

		return it->second;
	}

	bool InputSystem::isMouseButtonReleased(Input::MouseButton btn) const
	{
		auto it = m_impl->m_mouseButtonsReleased.find(btn);
		if (it == m_impl->m_mouseButtonsReleased.end())
			return false;

		return it->second;
	}

	TripleMath::Vec2 InputSystem::getMouseDelta() const
	{
		return m_mouseDelta;
	}
}
