//SPDX-License-Identifier: BSD-3-Clause
#ifndef CONTORT_TEXT_LAYOUT__HXX
#define CONTORT_TEXT_LAYOUT__HXX

#include <string_view>
#include <stdexcept>
#include <vector>
#include <contort/widget.hxx>

namespace contort
{
	struct cantDisplayText_t final : std::exception
	{
	public:
		const char *what() const noexcept final { return ""; }
	};

	struct layout_t { };

	using segment_t = std::tuple<size_t, size_t>;
	using segments_t = std::vector<std::vector<segment_t>>;

	struct textLayout_t
	{
		virtual ~textLayout_t() noexcept = default;
		virtual bool supportsAlignMode(horizontalAlignment_t) const noexcept { return true; }
		virtual bool supportsWrapMode(wrapping_t) const noexcept { return true; }
		virtual std::vector<layout_t> layout(std::string_view text, uint32_t width,
			horizontalAlignment_t align, wrapping_t wrap) const noexcept = 0;
	};

	struct standardTextLayout_t final : textLayout_t
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