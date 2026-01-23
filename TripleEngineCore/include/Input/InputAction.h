#ifndef INPUT_ACTION_H
#define INPUT_ACTION_H

#include "ExportMacros.h"
#include "KeyCode.h"
#include "MouseButton.h"

namespace TripleEngineCore::Input {
    enum class CORE_API InputTriggerType {
        Key,
        MouseButton
    };

    enum class CORE_API TriggerState {
        Pressed,
        Held,
        Released
    };

    struct CORE_API InputTrigger {
        InputTriggerType type;
        TriggerState state;

        union {
            Input::KeyCode key;
            Input::MouseButton mouse;
        };
    };
}

#endif // INPUT_ACTION_H
