#ifndef INPUT_ACTION_SYSTEM_H
#define INPUT_ACTION_SYSTEM_H

#include <string>
#include <functional>

#include <triple/core/input/InputSystem.h>
#include <triple/core/base/ExportMacros.h>

#include "InputAction.h"

namespace triple::core {
	class CORE_API ActionMap {
	public:
		ActionMap(core::InputSystem *inputSystem);
		~ActionMap();

		void bind(const std::string &name, std::vector<InputTrigger> triggers,
		          std::function<void()> callback);

		void update();

	private:
		struct Impl;
		Impl *m_impl;

		bool checkTrigger(const InputTrigger &t);
		core::InputSystem *m_inputSystem;
	};

} // namespace triple::core

#endif // INPUT_ACTION_SYSTEM_H
