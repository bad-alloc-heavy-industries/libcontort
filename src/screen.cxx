//SPDX-License-Identifier: BSD-3-Clause
#include <stdexcept>
#include <contort/screen.hxx>

namespace contort
{
	screen_t::screen_t(FILE *const inputFile, FILE *const outputFile) :
		termInput{fileno(inputFile)}, termOutput{fileno(outputFile)}, resizePipe{}
	{
		if (!resizePipe.valid())
			throw std::exception{};
	}
}
