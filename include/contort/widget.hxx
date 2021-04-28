//SPDX-License-Identifier: BSD-3-Clause
#ifndef CONTORT_WIDGET__HXX
#define CONTORT_WIDGET__HXX

#include <cstdint>
#include <type_traits>
#include <contort/defs.hxx>

namespace contort
{
	enum struct horizontalAlignment_t : uint8_t
	{
		left   = 1U,
		right  = 2U,
		center = 4U,
	};

	enum struct verticalAlignment_t : uint8_t
	{
		top =    0x01U,
		middle = 0x02U,
		bottom = 0x04U
	};

	enum struct wrapping_t : uint8_t
	{
		space    = 1U,
		any      = 2U,
		clip     = 4U,
		ellipsis = 8U
	};

	enum struct packing_t : uint8_t
	{
		pack     = 1U,
		given    = 2U,
		relative = 4U,
		weight   = 8U
	};

	template<typename> struct is_attribute : std::false_type { };
	template<> struct is_attribute<horizontalAlignment_t> : std::true_type { };
	template<> struct is_attribute<verticalAlignment_t> : std::true_type { };
	template<> struct is_attribute<wrapping_t> : std::true_type { };
	template<> struct is_attribute<packing_t> : std::true_type { };
	template<typename T> constexpr inline bool is_attribute_v = is_attribute<T>::value;

	struct attributes_t final
	{
	private:
		uint16_t attributes{0};

	public:
		constexpr attributes_t() noexcept = default;
		constexpr attributes_t(const horizontalAlignment_t attr) noexcept :
			attributes{uint16_t(static_cast<uint8_t>(attr) << 0U)} { }
		constexpr attributes_t(const verticalAlignment_t attr) noexcept :
			attributes{uint16_t(static_cast<uint8_t>(attr) << 4U)} { }
		constexpr attributes_t(const wrapping_t attr) noexcept :
			attributes{uint16_t(static_cast<uint8_t>(attr) << 8U)} { }
		constexpr attributes_t(const packing_t attr) noexcept :
			attributes{uint16_t(static_cast<uint8_t>(attr) << 12U)} { }

		constexpr attributes_t &operator |(const attributes_t attribute) noexcept
		{
			attributes |= attribute.attributes;
			return *this;
		}

		template<typename T> constexpr attributes_t &
			operator |(const std::enable_if_t<is_attribute_v<T>, T> attribute) noexcept
				{ return *this | attributes_t{attribute}; }
	};

	template<typename T, typename U> constexpr attributes_t
		operator |(const std::enable_if_t<is_attribute_v<T>, T> a,
			const std::enable_if_t<is_attribute_v<U>, U> b) noexcept
		{ return attributes_t{a} | b; }

	struct CONTORT_CLS_API widget_t
	{
	protected:
		bool selectable_{false};

		void invalidate() noexcept;

	public:
		widget_t() noexcept = default;

		[[nodiscard]] auto selectable() const noexcept { return selectable_; }
	};
} // namespace contort

#endif /*CONTORT_WIDGET__HXX*/
