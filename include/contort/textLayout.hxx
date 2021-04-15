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
		const char *what() const noexcept final { return error_.data(); }
	};

	struct layout_t { };

	using segment_t = std::tuple<size_t, size_t, std::variant<std::monostate, std::size_t, std::string_view>>;
	using segmentList_t = std::vector<segment_t>;
	using segments_t = std::vector<segmentList_t>;

	struct CONTORT_CLS_API textLayout_t
	{
		virtual ~textLayout_t() noexcept = default;
		virtual bool supportsAlignMode(horizontalAlignment_t) const noexcept { return true; }
		virtual bool supportsWrapMode(wrapping_t) const noexcept { return true; }
		virtual std::vector<layout_t> layout(std::string_view text, uint32_t width,
			horizontalAlignment_t align, wrapping_t wrap) const noexcept = 0;
	};

	struct CONTORT_CLS_API standardTextLayout_t final : textLayout_t
	{
		~standardTextLayout_t() noexcept final = default;
		bool supportsAlignMode(horizontalAlignment_t align) const noexcept final;
		bool supportsWrapMode(wrapping_t wrap) const noexcept final;
		std::vector<layout_t> layout(std::string_view text, uint32_t width,
			horizontalAlignment_t align, wrapping_t wrap) const noexcept final;

		segments_t calculateTextSegments(std::string_view text, uint32_t width, wrapping_t wrap) const;
	};

	using defaultLayout_t = standardTextLayout_t;
} // namespace contort

#endif /*CONTORT_TEXT_LAYOUT__HXX*/
