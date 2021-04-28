//SPDX-License-Identifier: BSD-3-Clause
#include <contort/textLayout.hxx>
#include <contort/utils.hxx>

using namespace std::literals::string_view_literals;

namespace contort
{
	constexpr static auto ellipsis{u8"…"sv};

	size_t lineWidth(const segmentList_t &segmentList)
	{
		size_t screenCols{0};
		for (const auto &segment : segmentList)
		{
			const auto &[columns, begin, end] = segment;
			// TODO: Constrain condition to only apply to the first segment in the list.
			if (std::holds_alternative<std::monostate>(end) && begin == 0)
				continue;
			screenCols += columns;
		}
		return screenCols;
	}

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

	segments_t standardTextLayout_t::layout(const std::string_view text, const uint32_t width,
		const horizontalAlignment_t align, const wrapping_t wrap) const noexcept try
	{
		auto layout{calculateTextSegments(text, width, wrap)};
		alignLayout(layout, width, align);
		return layout;
	}
	catch (const cantDisplayText_t &)
		{ return {}; }

	void standardTextLayout_t::alignLayout(segments_t &layout, const uint32_t width,
		const horizontalAlignment_t align) noexcept
	{
		for (auto &segments : layout)
		{
			const auto screenCols{lineWidth(segments)};
			if (screenCols == width || align == horizontalAlignment_t::left)
				continue;
			else if (align == horizontalAlignment_t::right)
				segments.emplace(segments.begin(), segment_t{width - screenCols, 0, std::monostate{}});
			else
				segments.emplace(segments.begin(), segment_t{(width - screenCols + 1) / 2, 0, std::monostate{}});
		}
	}

	segments_t standardTextLayout_t::calculateTextSegments(const std::string_view text,
		const uint32_t width, const wrapping_t wrap)
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

			if (screenCols == 0)
			{
				segments.emplace_back(segmentList_t{{0, lineEnd, std::monostate{}}});
				i = lineEnd + 1;
				continue;
			}
			else if (screenCols < width)
			{
				segments.emplace_back(segmentList_t{
					{screenCols, i, lineEnd},
					{0, lineEnd, std::monostate{}}
				});
				i = lineEnd + 1;
				continue;
			}

			size_t offset{0};
			std::tie(offset, screenCols) = utils::calcTextPos(text, i, lineEnd, width);
			if (offset == i)
				throw cantDisplayText_t{"Wide character will not fit in 1-column width"sv};
			else if (wrap == wrapping_t::any)
			{
				segments.emplace_back(segmentList_t{{screenCols, i, offset}});
				i = offset;
				continue;
			}
			// wrap == wrapping_t::space
			if (text[offset] == ' ')
			{
				segments.emplace_back(segmentList_t{
					{screenCols, i, offset},
					{0, offset, std::monostate{}}
				});
				i = offset + 1;
				continue;
			}
			else if (utils::isWideChar(text, offset))
			{
				segments.emplace_back(segmentList_t{{screenCols, i, offset}});
				i = offset;
				continue;
			}

			auto breakWord{true};
			auto prevOffset{offset};
			while (prevOffset > i)
			{
				prevOffset = utils::movePrevChar(text, i, prevOffset);
				if (text[prevOffset] == ' ')
				{
					screenCols = utils::calcWidth(text, i, prevOffset);
					segmentList_t segment{{0, prevOffset, std::monostate{}}};
					if (i != prevOffset)
						segment.emplace(segment.begin(), segment_t{screenCols, i, prevOffset});
					segments.emplace_back(std::move(segment));
					i = prevOffset + 1;
					breakWord = false;
					break;
				}
				else if (utils::isWideChar(text, prevOffset))
				{
					const auto nextOffset{utils::moveNextChar(text, prevOffset, offset)};
					screenCols = utils::calcWidth(text, i, nextOffset);
					segments.emplace_back(segmentList_t{{screenCols, i, nextOffset}});
					i = nextOffset;
					breakWord = false;
					break;
				}
			}

			if (breakWord && !segments.empty())
			{
				auto &lastSegment{segments.back()};
				// If we didn't exit the above while loop through a break statement, then
				// we didn't find anywhere to break the line and have to break a word.
				if (lastSegment.size() == 2 || (lastSegment.size() == 1 &&
					std::holds_alternative<std::monostate>(std::get<2>(lastSegment[0])) &&
					std::get<1>(lastSegment[0]) == 0))
				{
					size_t alignScreenCols{0};
					size_t alignOffset{0};
					size_t lineScreenCols{0};
					size_t lineOffset{0};
					size_t lineEnd{0};

					if (lastSegment.size() == 1)
					{
						const auto &[screenCols, offset, _] = lastSegment[0];
						(void)_;
						alignScreenCols = screenCols;
						alignOffset = offset;
						lineOffset = lineEnd = offset;
					}
					else
					{
						std::remove_reference_t<decltype(std::get<2>(lastSegment[0]))> end{};
						std::tie(lineScreenCols, lineOffset, end) = lastSegment[0];
						const auto &[screenCols, offset, _] = lastSegment[1];
						(void)_;
						alignScreenCols = screenCols;
						alignOffset = offset;
					}

					if (lineScreenCols < width && alignScreenCols == 0 && text[alignOffset] == ' ')
					{
						segments.pop_back();
						i = lineOffset;
						std::tie(offset, screenCols) = utils::calcTextPos(text, i, lineEnd, width);
						segments.emplace_back(segmentList_t{{screenCols, i, offset}});
						i = offset;
						if (i < text.length() && (text[i] == ' ' || text[i] == '\n'))
						{
							segments.back().emplace_back(segment_t{0, i, std::monostate{}});
							++i;
						}
						continue;
					}
				}
			}

			segments.emplace_back(segmentList_t{{screenCols, i, offset}});
			i = offset;
		}
		return segments;
	}
} // namespace contort
