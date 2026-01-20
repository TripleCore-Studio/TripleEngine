#ifndef INPUT_ACTION_SYSTEM_H
#define INPUT_ACTION_SYSTEM_H

#include <string>

#include "Interfaces/ISystem.h"
#include "Engine/EventSystem.h"
#include "System/InputSystem.h"
#include "ExportMacros.h"

namespace TripleEngineCore::System {
    class CORE_API InputActionSystem : public ISystem {
    public:
        enum class InputTriggerType {
            Key,
            MouseButton
        };

        enum class TriggerState {
            Pressed,
            Held,
            Released
        };

        struct InputTrigger {
            InputTriggerType type;
            TriggerState state;

            union {
                Event::KeyCode key;
                Event::MouseButton mouse;
            };
        };

        struct InputAction {
            std::vector<InputTrigger> triggers;
            std::function<void()> callback;
        };

		InputActionSystem(System::InputSystem* inputSystem);
        ~InputActionSystem();

        void bind(
            const std::string& name,
            std::vector<InputTrigger> triggers,
            std::function<void()> callback
        );

        void update(float dt) override;

    private:
        bool checkTrigger(const InputTrigger& t);

        struct Impl;
		Impl* _impl;
		System::InputSystem* _pInputSystem;
    };

}

#endif // INPUT_ACTION_SYSTEM_H
