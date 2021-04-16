//SPDX-License-Identifier: BSD-3-Clause
#include <contort/mainLoop.hxx>

namespace contort
{
	mainLoop_t::mainLoop_t(widget_t *const widget, std::optional<screen_t> screen,
		std::unique_ptr<eventLoop_t> eventLoop) noexcept : widget_{widget},
		screen_{screen ? std::move(*screen) : screen_t{}}, eventLoop_{std::move(eventLoop)}
	{
		/*if (palette)
			screen.registerPalette(palette);*/

		if (!eventLoop_)
			eventLoop_ = std::make_unique<selectEventLoop_t>();
	}
} // namespace contort
