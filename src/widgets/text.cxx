//SPDX-License-Identifier: BSD-3-Clause
#include <contort/widgets/text.hxx>
#include <contort/utils.hxx>

using namespace std::literals::string_view_literals;
using contort::utils::decomposeTagmarkup;

namespace contort::widgets
{
	text_t::text_t(const std::string_view markup, const horizontalAlignment_t align,
		const wrapping_t wrap, std::unique_ptr<textLayout_t> &&textLayout) noexcept : widget_t{}
	{
		text(markup);
		layout(align, wrap, std::move(textLayout));
	}

	void text_t::text(const std::string_view markup) noexcept
	{
		std::tie(text_) = decomposeTagmarkup(markup);
	}

	void text_t::alignMode(const horizontalAlignment_t mode)
	{
		if (!layout_->supportsAlignMode(mode))
			throw textError_t{"Alignment mode {} not supported."sv, mode};
		align_ = mode;
		invalidate();
	}

	void text_t::wrapMode(const wrapping_t mode)
	{
		if (!layout_->supportsWrapMode(mode))
			throw textError_t{"Wrap mode {} not supported."sv, mode};
		wrap_ = mode;
		invalidate();
	}

	void text_t::layout(const horizontalAlignment_t align, const wrapping_t wrap,
		std::unique_ptr<textLayout_t> textLayout)
	{
		if (!textLayout)
			layout_ = std::make_unique<defaultLayout_t>();
		alignMode(align);
		wrapMode(wrap);
	}

	void text_t::invalidate() noexcept
	{
		widget_t::invalidate();
	}
} // namespace contort::widgets
