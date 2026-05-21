#include "System/InputActionSystem.h"
#include <unordered_map>
#include "TLogger.h"

namespace TripleEngineCore::System {
    struct InputAction {
        std::vector<Input::InputTrigger> triggers;
        std::function<void()> callback;
    };

	struct InputActionSystem::Impl
	{
		std::unordered_map<std::string, InputAction> m_actions;
	};
	InputActionSystem::InputActionSystem(System::InputSystem* inputSystem) : m_inputSystem(inputSystem)
		, m_impl(new Impl())
	{
	}
	InputActionSystem::~InputActionSystem()
	{
		delete m_impl;
	}
	void InputActionSystem::bind(const std::string& name, std::vector<Input::InputTrigger> triggers, std::function<void()> callback)
	{
        if (m_impl->m_actions.find(name) != m_impl->m_actions.end()) {
            TripleLogger::TLogger::ModuleWarn("InputActionSystem", "InputAction '{}' already exists, overwriting", name);
        }

        InputAction action;
        action.triggers = std::move(triggers);
        action.callback = std::move(callback);

        m_impl->m_actions[name] = std::move(action);
	}
	void InputActionSystem::update(float /*dt*/)
	{
        for (auto& [name, action] : m_impl->m_actions)
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
	bool InputActionSystem::checkTrigger(const Input::InputTrigger& t)
	{
        switch (t.type)
        {
        case Input::InputTriggerType::Key:
            switch (t.state)
            {
            case Input::TriggerState::Pressed:
                return m_inputSystem->isKeyPressed(t.key);
            case Input::TriggerState::Held:
                return m_inputSystem->isKeyDown(t.key);
            case Input::TriggerState::Released:
                return m_inputSystem->isKeyReleased(t.key);
            }
            break;

        case Input::InputTriggerType::MouseButton:
            switch (t.state)
            {
            case Input::TriggerState::Pressed:
                return m_inputSystem->isMouseButtonPressed(t.mouse);
            case Input::TriggerState::Held:
                return m_inputSystem->isMouseButtonDown(t.mouse);
            case Input::TriggerState::Released:
                return m_inputSystem->isMouseButtonReleased(t.mouse);
            }
            break;
        }

        return false;
	}
}