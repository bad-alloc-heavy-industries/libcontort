//SPDX-License-Identifier: BSD-3-Clause
#include <contort/mainLoop.hxx>

namespace contort
{
	mainLoop_t::mainLoop_t(widget_t *const widget, std::unique_ptr<screen_t> screen,
		const bool handleMouse, std::unique_ptr<eventLoop_t> eventLoop) noexcept :
		widget_{widget}, screen_{screen ? std::move(screen) : std::make_unique<rawTerminal_t>()},
		eventLoop_{std::move(eventLoop)}, handleMouse_{handleMouse}
	{
		/*if (palette)
			screen.registerPalette(palette);*/

		if (!eventLoop_)
			eventLoop_ = std::make_unique<selectEventLoop_t>();
	}
} // namespace contort
