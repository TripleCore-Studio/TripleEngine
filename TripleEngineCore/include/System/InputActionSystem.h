#ifndef INPUT_ACTION_SYSTEM_H
#define INPUT_ACTION_SYSTEM_H

#include <string>

#include "Interfaces/ISystem.h"
#include "Engine/EventSystem.h"
#include "System/InputSystem.h"
#include "Input/InputAction.h"
#include "ExportMacros.h"

namespace TripleEngineCore::System {
    class CORE_API InputActionSystem : public ISystem {
    public:
		InputActionSystem(System::InputSystem* inputSystem);
        ~InputActionSystem();

        void bind(
            const std::string& name,
            std::vector<Input::InputTrigger> triggers,
            std::function<void()> callback
        );

        void update(float dt) override;

    private:
        bool checkTrigger(const Input::InputTrigger& t);

        struct Impl;
		Impl* _impl;
		System::InputSystem* _pInputSystem;
    };

}

#endif // INPUT_ACTION_SYSTEM_H
