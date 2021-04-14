//SPDX-License-Identifier: BSD-3-Clause
#ifndef CONTORT_MAIN_LOOP__HXX
#define CONTORT_MAIN_LOOP__HXX

#include <optional>
#include <contort/defs.hxx>
#include <contort/screen.hxx>

namespace contort
{
	struct mainLoop_t
	{
	private:
		screen_t screen_;

	public:
		mainLoop_t(std::optional<screen_t> screen = std::nullopt) noexcept;
	};
} // namespace contort

#endif /*CONTORT_MAIN_LOOP__HXX*/
