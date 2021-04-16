//SPDX-License-Identifier: BSD-3-Clause
#ifndef CONTORT_MAIN_LOOP__HXX
#define CONTORT_MAIN_LOOP__HXX

#include <optional>
#include <memory>
#include <contort/defs.hxx>
#include <contort/widget.hxx>
#include <contort/screen.hxx>
#include <contort/eventLoop.hxx>

namespace contort
{
	struct CONTORT_CLS_API mainLoop_t
	{
	private:
		widget_t *widget_;
		screen_t screen_;
		std::unique_ptr<eventLoop_t> eventLoop_;

	public:
		mainLoop_t(widget_t *widget, std::optional<screen_t> screen = std::nullopt,
			std::unique_ptr<eventLoop_t> eventLoop = {}) noexcept;
	};
} // namespace contort

#endif /*CONTORT_MAIN_LOOP__HXX*/
