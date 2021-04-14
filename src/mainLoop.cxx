//SPDX-License-Identifier: BSD-3-Clause
#include <contort/mainLoop.hxx>

namespace contort
{
	mainLoop_t::mainLoop_t(widget_t *const widget, std::optional<screen_t> screen) noexcept :
		widget_{widget}, screen_{screen ? std::move(*screen) : screen_t{}}
	{
		/*if (palette)
			screen.registerPalette(palette);*/
	}
} // namespace contort
