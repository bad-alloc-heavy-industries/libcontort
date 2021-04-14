//SPDX-License-Identifier: BSD-3-Clause
#include <contort/textLayout.hxx>

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
} // namespace contort
