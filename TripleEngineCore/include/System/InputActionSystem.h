#ifndef INPUT_ACTION_SYSTEM_H
#define INPUT_ACTION_SYSTEM_H

#include <string>
#include <vector>
#include <functional>

#include "ExportMacros.h"
#include "Interfaces/ISystem.h"
#include "System/InputSystem.h"
#include "Input/InputAction.h"

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
		Impl* m_impl;
		System::InputSystem* m_inputSystem;
    };

}

#endif // INPUT_ACTION_SYSTEM_H
