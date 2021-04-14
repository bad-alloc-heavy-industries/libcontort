//SPDX-License-Identifier: BSD-3-Clause
#include <contort/mainLoop.hxx>

namespace contort
{
	mainLoop_t::mainLoop_t(std::optional<screen_t> screen) noexcept :
		screen_{screen ? std::move(*screen) : screen_t{}}
	{
	}
} // namespace contort
