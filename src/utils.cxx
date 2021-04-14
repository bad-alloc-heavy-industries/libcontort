//SPDX-License-Identifier: BSD-3-Clause
#include <contort/utils.hxx>

namespace contort::utils
{
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
} // namespace contort::utils
