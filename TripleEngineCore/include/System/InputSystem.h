#ifndef INPUT_SYSTEM_H
#define INPUT_SYSTEM_H

#include "Interfaces/ISystem.h"
#include "Engine/EventSystem.h"
#include "Vec2.h"

namespace TripleEngineCore::System {
    class InputSystem : public ISystem {
    public:
        void init() override;
        void update(float dt) override;

        void onEvent(Event& e);

        bool isKeyDown(Event::KeyCode key) const;
        bool isKeyPressed(Event::KeyCode key) const;

        bool isMouseButtonDown(Event::MouseButton btn) const;
        bool isMouseButtonPressed(Event::MouseButton btn) const;

        TripleMath::Vec2 getMouseDelta() const;

    private:
        std::unordered_map<Event::KeyCode, bool> _keys;
        std::unordered_map<Event::KeyCode, bool> _keysPressed;

        std::unordered_map<Event::MouseButton, bool> _mouseButtons;
        std::unordered_map<Event::MouseButton, bool> _mouseButtonsPressed;

        float _mouseX;
        float _mouseY;
        float _lastMouseX;
        float _lastMouseY;
        bool _firstMouse;
        TripleMath::Vec2 _mouseDelta;
    };

}

#endif // INPUT_SYSTEM_H
