#include "System/InputSystem.h"

#include "TLogger.h"

namespace TripleEngineCore::System {
	struct InputSystem::Impl
	{
		std::unordered_map<Event::KeyCode, bool> _keys;
		std::unordered_map<Event::KeyCode, bool> _keysPressed;
		std::unordered_map<Event::KeyCode, bool> _keysReleased;

		std::unordered_map<Event::MouseButton, bool> _mouseButtons;
		std::unordered_map<Event::MouseButton, bool> _mouseButtonsPressed;
		std::unordered_map<Event::MouseButton, bool> _mouseButtonsReleased;
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

	void InputSystem::onEvent(Event& e)
	{
		switch (e.getType())
		{
		case Event::Type::KeyboardInput:
		{
			auto& ke = static_cast<KeyboardInputEvent&>(e);
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

			if (me.isPressed())
			{
				_impl->_mouseButtons[me.getButton()] = true;
				_impl->_mouseButtonsPressed[me.getButton()] = true;
			}
			else if (me.isReleased())
			{
				_impl->_mouseButtons[me.getButton()] = false;
				_impl->_mouseButtonsReleased[me.getButton()] = true;
			}
			break;
		}
		default:
			break;
		}
	}

	bool InputSystem::isKeyDown(Event::KeyCode key) const
	{
		auto it = _impl->_keys.find(key);
		if (it == _impl->_keys.end())
			return false;

		return it->second;
	}

	bool InputSystem::isKeyPressed(Event::KeyCode key) const
	{
		auto it = _impl->_keysPressed.find(key);
		if (it == _impl->_keysPressed.end())
			return false;

		return it->second;
	}

	bool InputSystem::isKeyReleased(Event::KeyCode key) const
	{
		auto it = _impl->_keysReleased.find(key);
		if (it == _impl->_keysReleased.end())
			return false;

		return it->second;
	}

	bool InputSystem::isMouseButtonDown(Event::MouseButton btn) const
	{
		auto it = _impl->_mouseButtons.find(btn);
		if (it == _impl->_mouseButtons.end())
			return false;

		return it->second;
	}

	bool InputSystem::isMouseButtonPressed(Event::MouseButton btn) const
	{
		auto it = _impl->_mouseButtonsPressed.find(btn);
		if (it == _impl->_mouseButtonsPressed.end())
			return false;

		return it->second;
	}

	bool InputSystem::isMouseButtonReleased(Event::MouseButton btn) const
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
