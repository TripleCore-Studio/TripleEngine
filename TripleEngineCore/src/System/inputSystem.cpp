#include "System/InputSystem.h"
#include <unordered_map>

#include "TLogger.h"
#include "Input/KeyCode.h"
#include "Input/MouseButton.h"

namespace TripleEngineCore::System {
	struct InputSystem::Impl
	{
		std::unordered_map<Input::KeyCode, bool> _keys;
		std::unordered_map<Input::KeyCode, bool> _keysPressed;
		std::unordered_map<Input::KeyCode, bool> _keysReleased;

		std::unordered_map<Input::MouseButton, bool> _mouseButtons;
		std::unordered_map<Input::MouseButton, bool> _mouseButtonsPressed;
		std::unordered_map<Input::MouseButton, bool> _mouseButtonsReleased;
	};

	InputSystem::InputSystem() : 
		_impl(new Impl()),
		_mouseX(0),
		_mouseY(0),
		_lastMouseX(0),
		_lastMouseY(0),
		_mouseDelta(0, 0),
		_firstMouse(true)
	{}

	InputSystem::~InputSystem() { delete _impl; }

	void InputSystem::init()
	{
		_impl->_keys.clear();
		_impl->_keysPressed.clear();

		_mouseX = _mouseY = 0.0f;
		_lastMouseX = _lastMouseY = 0.0f;
		_mouseDelta = TripleMath::Vec2(0, 0);
		_firstMouse = true;
	}

	void InputSystem::update(float /*dt*/)
	{
		_impl->_keysPressed.clear();
		_impl->_keysReleased.clear();

		_impl->_mouseButtonsPressed.clear();
		_impl->_mouseButtonsReleased.clear();

		_mouseDelta = TripleMath::Vec2(0, 0);
	}

	void InputSystem::onKeyboard(Event::KeyboardInputEvent& ke)
	{
		auto key = ke.getKey();

		if (ke.isPressed())
		{
			if (!_impl->_keys[key])
				_impl->_keysPressed[key] = true;

			_impl->_keys[key] = true;
		}
		else if (ke.isReleased())
		{
			_impl->_keys[key] = false;
			_impl->_keysReleased[key] = true;
		}
	}

	void InputSystem::onMouseMove(Event::MouseMoveEvent& me)
	{
		if (_firstMouse) {
			_mouseX = _lastMouseX = me.getX();
			_mouseY = _lastMouseY = me.getY();
			_firstMouse = false;
		}
		else {
			_mouseDelta = TripleMath::Vec2(
				me.getX() - _mouseX,
				_mouseY - me.getY()
			);
			_mouseX = me.getX();
			_mouseY = me.getY();
		}
	}

	void InputSystem::onMouseButton(Event::MouseButtonEvent& me)
	{
		auto btn = me.getButton();

		if (me.isPressed())
		{
			_impl->_mouseButtons[btn] = true;
			_impl->_mouseButtonsPressed[btn] = true;
		}
		else if (me.isReleased())
		{
			_impl->_mouseButtons[btn] = false;
			_impl->_mouseButtonsReleased[btn] = true;
		}
	}

	bool InputSystem::isKeyDown(Input::KeyCode key) const
	{
		auto it = _impl->_keys.find(key);
		if (it == _impl->_keys.end())
			return false;

		return it->second;
	}

	bool InputSystem::isKeyPressed(Input::KeyCode key) const
	{
		auto it = _impl->_keysPressed.find(key);
		if (it == _impl->_keysPressed.end())
			return false;

		return it->second;
	}

	bool InputSystem::isKeyReleased(Input::KeyCode key) const
	{
		auto it = _impl->_keysReleased.find(key);
		if (it == _impl->_keysReleased.end())
			return false;

		return it->second;
	}

	bool InputSystem::isMouseButtonDown(Input::MouseButton btn) const
	{
		auto it = _impl->_mouseButtons.find(btn);
		if (it == _impl->_mouseButtons.end())
			return false;

		return it->second;
	}

	bool InputSystem::isMouseButtonPressed(Input::MouseButton btn) const
	{
		auto it = _impl->_mouseButtonsPressed.find(btn);
		if (it == _impl->_mouseButtonsPressed.end())
			return false;

		return it->second;
	}

	bool InputSystem::isMouseButtonReleased(Input::MouseButton btn) const
	{
		auto it = _impl->_mouseButtonsReleased.find(btn);
		if (it == _impl->_mouseButtonsReleased.end())
			return false;

		return it->second;
	}

	TripleMath::Vec2 InputSystem::getMouseDelta() const
	{
		return _mouseDelta;
	}
}
