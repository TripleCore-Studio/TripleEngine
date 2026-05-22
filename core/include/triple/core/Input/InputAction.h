#ifndef INPUT_ACTION_H
#define INPUT_ACTION_H

#include "triple/core/ExportMacros.h"
#include "KeyCode.h"
#include "MouseButton.h"

namespace triple::core {
    enum class InputTriggerType {
        Key,
        MouseButton
    };

    enum class TriggerState {
        Pressed,
        Held,
        Released
    };

    struct CORE_API InputTrigger {
        InputTriggerType type;
        TriggerState state;

        union {
            KeyCode key;
            MouseButton mouse;
        };
    };
}

#endif // INPUT_ACTION_H
