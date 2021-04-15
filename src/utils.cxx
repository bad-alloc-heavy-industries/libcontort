//SPDX-License-Identifier: BSD-3-Clause
#include <cstring>
#include <limits>
#include <substrate/utility>
#include <contort/utils.hxx>

namespace contort::utils
{
	constexpr static auto screenWidths{substrate::make_array<std::tuple<char32_t, size_t>>(
	{
		{126, 1}, {159, 0}, {687, 1}, {710, 0},
		{711, 1}, {727, 0}, {733, 1}, {879, 0},
		{1154, 1}, {1161, 0}, {4347, 1}, {4447, 2},
		{7467, 1}, {7521, 0}, {8369, 1}, {8426, 0},
		{9000, 1}, {9002, 2}, {11021, 1}, {12350, 2},
		{12351, 1}, {12438, 2}, {12442, 0}, {19893, 2},
		{19967, 1}, {55203, 2}, {63743, 1}, {64106, 2},
		{65039, 1}, {65059, 0}, {65131, 2}, {65279, 1},
		{65376, 2}, {65500, 1}, {120831, 1}, {262141, 2},
		{1114109, 1}
	})};

	constexpr static auto replacementChar{U'\uFFFD'};

	tagMark_t tagmarkRecurse(markup_t markup, std::optional<attributes_t> attr)
	{
		return std::visit(overloaded_t
		{
			[&attr](std::string_view text) -> tagMark_t
				{ return {text, {attr, text.length()}}; }
		}, markup);
	}

	std::tuple<std::string> decomposeTagmarkup(const markup_t markup)
	{
		tagmarkRecurse(markup, std::nullopt);
		return {{}};
	}

	inline bool isMultiValid() noexcept { return true; }
	template<typename... values_t> inline bool isMultiValid(const uint8_t value, values_t... values) noexcept
		{ return (value & 0xC0U) == 0x80U && isMultiValid(values...); }

	inline uint8_t safeIndex(const std::string_view str, const size_t index) noexcept
	{
		if (index >= str.length())
			return std::numeric_limits<uint8_t>::max();
		uint8_t result{};
		memcpy(&result, &str[index], sizeof(uint8_t));
		return result;
	}

	std::tuple<char32_t, size_t> decodeOne(const std::string_view text, const size_t offset) noexcept
	{
		auto i{offset};
		const auto byteA{safeIndex(text, i)};
		++i;
		if (!(byteA & 0x80U))
			return {byteA, i};

		const auto byteB{safeIndex(text, i)};
		++i;
		if ((byteA & 0x60U) == 0x40U)
		{
			if (!isMultiValid(byteB))
				return {replacementChar, i};
			return {((byteA & 0x1FU) << 6U) | (byteB & 0x3FU), i};
		}

		const auto byteC{safeIndex(text, i)};
		++i;
		if ((byteA & 0x70U) == 0x60U)
		{
			if (!isMultiValid(byteB, byteC) || ((byteA & 0x0FU) == 0x0DU && (byteB & 0x20U)))
				return {replacementChar, i};
			return {((byteA & 0x0FU) << 12U) | ((byteB & 0x3FU) << 6U) | (byteC & 0x3FU), i};
		}

		const auto byteD{safeIndex(text, i)};
		++i;
		if ((byteA & 0x78U) == 0x70U)
		{
			if (!isMultiValid(byteB, byteC, byteD))
				return {replacementChar, i};
			return {((byteA & 0x07U) << 18U) | ((byteB & 0x3FU) << 12U) |
				((byteC & 0x3FU) << 6U) | (byteD & 0x3FU), i};
		}
		return {replacementChar, i};
	}

	size_t screenWidth(const char32_t value) noexcept
	{
		if (value == 0xE || value == 0xF)
			return 0U;
		for (const auto &[codepoint, width] : screenWidths)
		{
			if (value <= codepoint)
				return width;
		}
		return 1U;
	}

	size_t calcWidth(const std::string_view text, const size_t begin, const size_t end) noexcept
	{
		if (begin > text.length())
			return 0;
		size_t cols{0};
		for (size_t i = begin; i < end; )
		{
			const auto [codepoint, offset] = decodeOne(text, i);
			cols += screenWidth(codepoint);
			i = offset;
			if (i >= text.length())
				i = end;
		}
		return cols;
	}
} // namespace contort::utils