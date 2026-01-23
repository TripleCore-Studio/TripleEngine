#ifndef INPUT_SYSTEM_H
#define INPUT_SYSTEM_H

#include <string>

#include "Interfaces/ISystem.h"
#include "ExportMacros.h"
#include "Vec2.h"
#include "Event/Event.h"
#include "Input/KeyCode.h"
#include "Input/MouseButton.h"

#include "Event/KeyboardInputEvent.h"
#include "Event/MouseButtonEvent.h"
#include "Event/MouseMoveEvent.h"

namespace TripleEngineCore::System {
    class CORE_API InputSystem : public ISystem {
    public:
        InputSystem();
        ~InputSystem();

        void init() override;
        void update(float dt) override;

        void onKeyboard(Event::KeyboardInputEvent& ke);
        void onMouseMove(Event::MouseMoveEvent& me);
        void onMouseButton(Event::MouseButtonEvent& me);

        bool isKeyDown(Input::KeyCode key) const;
        bool isKeyPressed(Input::KeyCode key) const;
        bool isKeyReleased(Input::KeyCode key) const;

        bool isMouseButtonDown(Input::MouseButton btn) const;
        bool isMouseButtonPressed(Input::MouseButton btn) const;
        bool isMouseButtonReleased(Input::MouseButton btn) const;

        TripleMath::Vec2 getMouseDelta() const;
    private:
        struct Impl;
        Impl* _impl;

        float _mouseX;
        float _mouseY;
        float _lastMouseX;
        float _lastMouseY;
        bool _firstMouse;
        TripleMath::Vec2 _mouseDelta;
    };

}

#endif // INPUT_SYSTEM_H
