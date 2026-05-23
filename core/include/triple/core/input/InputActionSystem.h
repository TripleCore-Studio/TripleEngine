#ifndef INPUT_ACTION_SYSTEM_H
#define INPUT_ACTION_SYSTEM_H

#include <string>
#include <vector>
#include <functional>

#include "triple/core/base/ExportMacros.h"
#include "triple/core/base/ISystem.h"

#include "InputSystem.h"
#include "InputAction.h"

namespace triple::core {
    class CORE_API InputActionSystem : public ISystem {
    public:
		InputActionSystem(InputSystem* inputSystem);
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
		Impl* m_impl;
		InputSystem* m_inputSystem;
    };

}

#endif // INPUT_ACTION_SYSTEM_H
