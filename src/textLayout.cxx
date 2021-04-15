//SPDX-License-Identifier: BSD-3-Clause
#include <contort/textLayout.hxx>
#include <contort/utils.hxx>

namespace contort
{
	template<horizontalAlignment_t...> struct alignValueIs_t;
	template<horizontalAlignment_t a, horizontalAlignment_t... aligns> struct alignValueIs_t<a, aligns...>
	{
		constexpr bool operator ()(const horizontalAlignment_t align) noexcept
			{ return a == align || alignValueIs_t<aligns...>{}(align); }
	};
	template<> struct alignValueIs_t<>
		{ constexpr bool operator ()(const horizontalAlignment_t) noexcept { return false; } };

	template<wrapping_t...> struct wrapValueIs_t;
	template<wrapping_t w, wrapping_t... wraps> struct wrapValueIs_t<w, wraps...>
	{
		constexpr bool operator ()(const wrapping_t wrap) noexcept
			{ return w == wrap || wrapValueIs_t<wraps...>{}(wrap); }
	};
	template<> struct wrapValueIs_t<>
		{ constexpr bool operator ()(const wrapping_t) noexcept { return false; } };

	bool standardTextLayout_t::supportsAlignMode(const horizontalAlignment_t align) const noexcept
		{ return alignValueIs_t<horizontalAlignment_t::left, horizontalAlignment_t::center,
			horizontalAlignment_t::right>{}(align); }

	bool standardTextLayout_t::supportsWrapMode(const wrapping_t wrap) const noexcept
		{ return wrapValueIs_t<wrapping_t::any, wrapping_t::space, wrapping_t::clip, wrapping_t::ellipsis>{}(wrap); }

	std::vector<layout_t> standardTextLayout_t::layout(const std::string_view text, const uint32_t width,
		const horizontalAlignment_t align, const wrapping_t wrap) const noexcept try
	{
		const auto segments{calculateTextSegments(text, width, wrap)};

		return {};
	}
	catch (const cantDisplayText_t &)
		{ return {}; }

	segments_t standardTextLayout_t::calculateTextSegments(const std::string_view text,
		const uint32_t width, const wrapping_t wrap) const
	{
		if (wrapValueIs_t<wrapping_t::clip, wrapping_t::ellipsis>{}(wrap))
		{
			segments_t segments{};
			for (size_t i = 0; i < text.length(); )
			{
				std::vector<segment_t> segment{};
				auto lineEnd{[=]()
				{
					const auto end{text.find('\n', i)};
					if (end == std::string_view::npos)
						return text.length() - 1;
					return end;
				}()};
				const auto screenCols{utils::calcWidth(text, i, lineEnd)};

				const auto trimmed{wrap == wrapping_t::ellipsis && screenCols > width};
				if (trimmed)
				{
				}
				else
				{
				}

				segments.emplace_back(std::move(segment));
				i = lineEnd + 1;
			}
			return segments;
		}

		return {};
	}
} // namespace contort
