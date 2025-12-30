#include "System/InputSystem.h"

namespace TripleEngineCore::System {

	void InputSystem::init()
	{
		_keys.clear();
		_keysPressed.clear();

		_mouseX = _mouseY = 0.0f;
		_lastMouseX = _lastMouseY = 0.0f;
		_mouseDelta = TripleMath::Vec2(0, 0);
		_firstMouse = true;
	}

	void InputSystem::update(float /*dt*/)
	{
		_keysPressed.clear();
		_mouseButtonsPressed.clear();
		_mouseDelta = TripleMath::Vec2(0, 0);
	}

	void InputSystem::onEvent(Event& e)
	{
		switch (e.getType())
		{
		case Event::Type::KeyboardInput:
		{
			auto& ke = static_cast<KeyboardInputEvent&>(e);

			if (ke.isPressed()) {
				_keys[ke.getKey()] = true;
				_keysPressed[ke.getKey()] = true;
			}
			else if (ke.isReleased()) {
				_keys[ke.getKey()] = false;
			}
			break;
		}
		case Event::Type::MouseMove:
		{
			auto& me = static_cast<MouseMoveEvent&>(e);
			if (_firstMouse) {
				_mouseX = _lastMouseX = me.getX();
				_mouseY = _lastMouseY = me.getY();
				_firstMouse = false;
			}
			else {
				_mouseDelta = TripleMath::Vec2(me.getX() - _mouseX, _mouseY - me.getY());
				_mouseX = me.getX();
				_mouseY = me.getY();
			}
			break;
		}
		case Event::Type::MouseButtonInput:
		{
			auto& me = static_cast<MouseButtonEvent&>(e);

			if (me.isPressed()) {
				_mouseButtons[me.getButton()] = true;
				_mouseButtonsPressed[me.getButton()] = true;
			}
			else if (me.isReleased()) {
				_mouseButtons[me.getButton()] = false;
			}
			break;
		}
		default:
			break;
		}
	}

	bool InputSystem::isKeyDown(Event::KeyCode key) const
	{
		auto it = _keys.find(key);
		if (it == _keys.end())
			return false;

		return it->second;
	}

	bool InputSystem::isKeyPressed(Event::KeyCode key) const
	{
		auto it = _keysPressed.find(key);
		if (it == _keysPressed.end())
			return false;

		return it->second;
	}

	bool InputSystem::isMouseButtonDown(Event::MouseButton btn) const
	{
		auto it = _mouseButtons.find(btn);
		if (it == _mouseButtons.end())
			return false;

		return it->second;
	}

	bool InputSystem::isMouseButtonPressed(Event::MouseButton btn) const
	{
		auto it = _mouseButtonsPressed.find(btn);
		if (it == _mouseButtonsPressed.end())
			return false;

		return it->second;
	}

	TripleMath::Vec2 InputSystem::getMouseDelta() const
	{
		return _mouseDelta;
	}

}
