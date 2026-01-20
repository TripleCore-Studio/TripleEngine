#ifndef INPUT_SYSTEM_H
#define INPUT_SYSTEM_H

#include <string>

#include "Interfaces/ISystem.h"
#include "ExportMacros.h"
#include "Engine/EventSystem.h"
#include "Vec2.h"

namespace TripleEngineCore::System {
    class CORE_API InputSystem : public ISystem {
    public:
        InputSystem();
        ~InputSystem();

        void init() override;
        void update(float dt) override;

        void onEvent(Event& e);

        bool isKeyDown(Event::KeyCode key) const;
        bool isKeyPressed(Event::KeyCode key) const;
        bool isKeyReleased(Event::KeyCode key) const;

        bool isMouseButtonDown(Event::MouseButton btn) const;
        bool isMouseButtonPressed(Event::MouseButton btn) const;
        bool isMouseButtonReleased(Event::MouseButton btn) const;

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
