//SPDX-License-Identifier: BSD-3-Clause
#include <contort/textLayout.hxx>
#include <contort/utils.hxx>

using namespace std::literals::string_view_literals;

namespace contort
{
	constexpr static auto ellipsis{u8"…"sv};

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
				auto lineEnd{[=]()
				{
					const auto end{text.find('\n', i)};
					if (end == std::string_view::npos)
						return text.length() - 1;
					return end;
				}()};
				auto screenCols{utils::calcWidth(text, i, lineEnd)};

				size_t end{lineEnd};
				size_t padRight{0};
				const auto trimmed{wrap == wrapping_t::ellipsis && screenCols > width};
				if (trimmed)
				{
					size_t begin{0};
					size_t padLeft{0};
					std::tie(begin, end, padLeft, padRight) = utils::calcTrimText(text, i, lineEnd, 0, width - 1);
					//padLeft == 0
					//begin == i
					screenCols = width - 1 - padRight;
				}

				segmentList_t segment{};
				if (i != end)
					segment.emplace_back(screenCols, i, end);
				if (trimmed)
					segment.emplace_back(1, end, ellipsis);
				segment.emplace_back(padRight, end, std::monostate{});
				segments.emplace_back(std::move(segment));
				i = lineEnd + 1;
			}
			return segments;
		}

		return {};
	}
} // namespace contort
