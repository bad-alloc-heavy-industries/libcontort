//SPDX-License-Identifier: BSD-3-Clause
#ifndef CONTORT_TEXT_LAYOUT__HXX
#define CONTORT_TEXT_LAYOUT__HXX

#include <string_view>
#include <stdexcept>
#include <vector>
#include <variant>
#include <contort/widget.hxx>

namespace contort
{
	struct CONTORT_CLS_API cantDisplayText_t final : std::exception
	{
	private:
		std::string_view error_;

	public:
		cantDisplayText_t(const std::string_view error) : error_{error} { }
		[[nodiscard]] const char *what() const noexcept final { return error_.data(); }
	};

	using segment_t = std::tuple<size_t, size_t, std::variant<std::monostate, std::size_t, std::string_view>>;
	using segmentList_t = std::vector<segment_t>;
	using segments_t = std::vector<segmentList_t>;

	struct CONTORT_CLS_API textLayout_t
	{
		constexpr textLayout_t() noexcept = default;
		textLayout_t(const textLayout_t &) noexcept = delete;
		textLayout_t(textLayout_t &&) noexcept = delete;
		textLayout_t &operator =(const textLayout_t &) noexcept = delete;
		textLayout_t &operator =(textLayout_t &&) noexcept = delete;
		virtual ~textLayout_t() noexcept = default;
		[[nodiscard]] virtual bool supportsAlignMode(horizontalAlignment_t) const noexcept { return true; }
		[[nodiscard]] virtual bool supportsWrapMode(wrapping_t) const noexcept { return true; }
		[[nodiscard]] virtual segments_t layout(std::string_view text, uint32_t width,
			horizontalAlignment_t align, wrapping_t wrap) const noexcept = 0;
	};

	struct CONTORT_CLS_API standardTextLayout_t final : textLayout_t
	{
		constexpr standardTextLayout_t() noexcept = default;
		standardTextLayout_t(const standardTextLayout_t &) noexcept = delete;
		standardTextLayout_t(standardTextLayout_t &&) noexcept = delete;
		standardTextLayout_t &operator =(const standardTextLayout_t &) noexcept = delete;
		standardTextLayout_t &operator =(standardTextLayout_t &&) noexcept = delete;
		~standardTextLayout_t() noexcept final = default;
		[[nodiscard]] bool supportsAlignMode(horizontalAlignment_t align) const noexcept final;
		[[nodiscard]] bool supportsWrapMode(wrapping_t wrap) const noexcept final;
		[[nodiscard]] segments_t layout(std::string_view text, uint32_t width,
			horizontalAlignment_t align, wrapping_t wrap) const noexcept final;

		static void alignLayout(segments_t &layout, uint32_t width, horizontalAlignment_t align) noexcept;
		[[nodiscard]] static segments_t calculateTextSegments(std::string_view text, uint32_t width, wrapping_t wrap);
	};

	using defaultLayout_t = standardTextLayout_t;
} // namespace contort

#endif /*CONTORT_TEXT_LAYOUT__HXX*/
