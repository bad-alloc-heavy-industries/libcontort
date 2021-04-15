//SPDX-License-Identifier: BSD-3-Clause
#ifndef CONTORT_WIDGETS_TEXT__HXX
#define CONTORT_WIDGETS_TEXT__HXX

#include <string>
#include <string_view>
#include <memory>
#include <contort/widget.hxx>
#include <contort/textLayout.hxx>

namespace contort::widgets
{
	struct CONTORT_CLS_API textError_t : std::exception
	{
	private:
		std::string what_{};

	public:
		textError_t(std::string_view format, ...) noexcept;
		const char *what() const noexcept final { return what_.c_str(); }
	};

	struct CONTORT_CLS_API text_t final : widget_t
	{
	private:
		std::string text_{};
		horizontalAlignment_t align_{horizontalAlignment_t::left};
		wrapping_t wrap_{wrapping_t::space};
		std::unique_ptr<textLayout_t> layout_{nullptr};

		void invalidate() noexcept;

	public:
		text_t(const std::string &markup, horizontalAlignment_t align = horizontalAlignment_t::left,
			wrapping_t wrap = wrapping_t::space, std::unique_ptr<textLayout_t> &&textLayout = nullptr) noexcept :
			text_t{std::string_view{markup}, align, wrap, std::move(textLayout)} { }
		text_t(std::string_view markup, horizontalAlignment_t align = horizontalAlignment_t::left,
			wrapping_t wrap = wrapping_t::space, std::unique_ptr<textLayout_t> &&textLayout = nullptr) noexcept;

		auto &text() const noexcept { return text_; }
		void text(const std::string &markup) noexcept { text(std::string_view{markup}); }
		void text(std::string_view markup) noexcept;

		void alignMode(horizontalAlignment_t align);
		void wrapMode(wrapping_t wrap);
		void layout(horizontalAlignment_t align, wrapping_t wrap,
			std::unique_ptr<textLayout_t> textLayout = nullptr);
	};
} // namespace contort::widgets

#endif /*CONTORT_WIDGETS_TEXT__HXX*/
