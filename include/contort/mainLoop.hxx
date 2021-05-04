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
		std::unique_ptr<screen_t> screen_;
		std::unique_ptr<eventLoop_t> eventLoop_;
		bool handleMouse_;

		void update(const std::vector<int32_t> &keys, const std::vector<int32_t> &rawKeystrokes) noexcept;
		[[nodiscard]] std::vector<int32_t> inputFilter(const std::vector<int32_t> &keys,
			const std::vector<int32_t> &rawKeystrokes) const noexcept;

	public:
		mainLoop_t(widget_t *widget, std::unique_ptr<screen_t> screen = {},
			bool handleMouse = true, std::unique_ptr<eventLoop_t> eventLoop = {}) noexcept;

		int32_t run();
		void start();
		void stop();
	};
} // namespace contort

#endif /*CONTORT_MAIN_LOOP__HXX*/
