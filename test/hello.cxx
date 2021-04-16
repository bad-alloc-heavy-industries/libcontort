#include <string_view>
#include <contort/contort.hxx>
#include <contort/widgets.hxx>
#include <contort/decorations.hxx>

using namespace std::literals::string_view_literals;
using namespace contort;

int main(int, char **)
{
	auto text{widgets::text_t{"Hello world"sv}};
	auto fill{decorations::filler_t{text, "top"sv}};
	auto loop{mainLoop_t{&fill}};
	return loop.run();
}
