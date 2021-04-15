//SPDX-License-Identifier: BSD-3-Clause
#ifndef CONTORT_UTILS__HXX
#define CONTORT_UTILS__HXX

#include <string>
#include <string_view>
#include <tuple>
#include <variant>
#include <optional>
#include <contort/widget.hxx>

namespace contort::utils
{
	template<typename... Ts> struct overloaded_t : Ts... { using Ts::operator()...; };
	template<typename... Ts> overloaded_t(Ts...) -> overloaded_t<Ts...>;

	using markup_t = std::variant<std::string_view>;
	using attributeSpan_t = std::tuple<std::optional<attributes_t>, std::size_t>;
	using tagMark_t = std::tuple<markup_t, attributeSpan_t>;

	std::tuple<std::string> decomposeTagmarkup(markup_t markup);

	size_t calcWidth(std::string_view text, size_t begin, size_t end) noexcept;
	std::tuple<size_t, size_t> calcTextPos(std::string_view text, size_t beginOffset,
		size_t endOffset, size_t preferedColumn) noexcept;
	std::tuple<size_t, size_t, size_t, size_t> calcTrimText(std::string_view text,
		size_t beginOffset, size_t endOffset, size_t startCol, size_t endCol) noexcept;
} // namespace contort::utils

#endif /*CONTORT_UTILS__HXX*/
