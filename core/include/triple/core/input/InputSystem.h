#ifndef INPUT_SYSTEM_H
#define INPUT_SYSTEM_H

#include <triple/math/Vec2.h>

#include "triple/core/base/ExportMacros.h"

#include "KeyCode.h"
#include "MouseButton.h"

#include "triple/core/event/KeyboardInputEvent.h"
#include "triple/core/event/MouseButtonEvent.h"
#include "triple/core/event/MouseMoveEvent.h"

namespace triple::core {
	class CORE_API InputSystem {
	public:
		InputSystem();
		~InputSystem();

		void init();
		void update(float dt);

		void onKeyboard(KeyboardInputEvent &ke);
		void onMouseMove(MouseMoveEvent &me);
		void onMouseButton(MouseButtonEvent &me);

		bool isKeyDown(KeyCode key) const;
		bool isKeyPressed(KeyCode key) const;
		bool isKeyReleased(KeyCode key) const;

		bool isMouseButtonDown(MouseButton btn) const;
		bool isMouseButtonPressed(MouseButton btn) const;
		bool isMouseButtonReleased(MouseButton btn) const;

		triple::math::Vec2 getMouseDelta() const;

	private:
		struct Impl;
		Impl *m_impl;

		float m_mouseX;
		float m_mouseY;
		float m_lastMouseX;
		float m_lastMouseY;
		bool m_firstMouse;
		triple::math::Vec2 m_mouseDelta;
	};

} // namespace triple::core

#endif // INPUT_SYSTEM_H
