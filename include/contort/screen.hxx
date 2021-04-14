//SPDX-License-Identifier: BSD-3-Clause
#ifndef CONTORT_SCREEN__HXX
#define CONTORT_SCREEN__HXX

#include <cstdint>
#include <cstdio>
#include <contort/defs.hxx>
#include <substrate/pipe>

namespace contort
{
	struct screen_t final
	{
		int32_t termInput{-1};
		int32_t termOutput{-1};
		substrate::pipe_t resizePipe{};

	public:
		screen_t(FILE *inputFile = stdin, FILE *outputFile = stdout);
		screen_t(const screen_t &) = delete;
		screen_t(screen_t &&) = default;
	};
} // namespace contort

#endif /*CONTORT_SCREEN__HXX*/
