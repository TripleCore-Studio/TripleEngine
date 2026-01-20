#include "System/InputActionSystem.h"

#include <unordered_map>

#include "TLogger.h"

namespace TripleEngineCore::System {
	struct InputActionSystem::Impl
	{
		std::unordered_map<std::string, InputAction> _actions;
	};
	InputActionSystem::InputActionSystem(System::InputSystem* inputSystem) : _pInputSystem(inputSystem)
		, _impl(new Impl())
	{
	}
	InputActionSystem::~InputActionSystem()
	{
		delete _impl;
	}
	void InputActionSystem::bind(const std::string& name, std::vector<InputTrigger> triggers, std::function<void()> callback)
	{
        if (_impl->_actions.find(name) != _impl->_actions.end()) {
            TripleLogger::TLogger::ModuleWarn("InputActionSystem", "InputAction '{}' already exists, overwriting", name);
        }

        InputAction action;
        action.triggers = std::move(triggers);
        action.callback = std::move(callback);

        _impl->_actions[name] = std::move(action);
	}
	void InputActionSystem::update(float /*dt*/)
	{
        for (auto& [name, action] : _impl->_actions)
        {
            bool allTriggered = true;

            for (const auto& trigger : action.triggers)
            {
                if (!checkTrigger(trigger)) {
                    allTriggered = false;
                    break;
                }
            }

            if (allTriggered && action.callback)
                action.callback();
        }
	}
	bool InputActionSystem::checkTrigger(const InputTrigger& t)
	{
        switch (t.type)
        {
        case InputTriggerType::Key:
            switch (t.state)
            {
            case TriggerState::Pressed:
                return _pInputSystem->isKeyPressed(t.key);
            case TriggerState::Held:
                return _pInputSystem->isKeyDown(t.key);
            case TriggerState::Released:
                return _pInputSystem->isKeyReleased(t.key);
            }
            break;

        case InputTriggerType::MouseButton:
            switch (t.state)
            {
            case TriggerState::Pressed:
                return _pInputSystem->isMouseButtonPressed(t.mouse);
            case TriggerState::Held:
                return _pInputSystem->isMouseButtonDown(t.mouse);
            case TriggerState::Released:
                return _pInputSystem->isMouseButtonReleased(t.mouse);
            }
            break;
        }

        return false;
	}
}