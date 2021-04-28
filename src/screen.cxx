//SPDX-License-Identifier: BSD-3-Clause
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <fcntl.h>
#include <termios.h>
#include <csignal>
#include <contort/screen.hxx>

using namespace std::literals::string_view_literals;

namespace contort
{
	// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
	std::vector<screen_t *> screens{};

	void sigwinchHandler(const int32_t)
	{
	}

	void sigcontHandler(const int32_t)
	{
	}

	namespace escapes
	{
		constexpr static auto switchToAlternateBuffer{"\x1B\x07\x1B[?47h"sv};

		constexpr static auto mouseTrackingOn{"\x1B[?1000h\x1B[?1002h"sv};
		constexpr static auto mouseTrackingOff{"\x1B[?1002l\x1B[?1000l"sv};
	} // namespace escapes

	void screen_t::start()
	{
		if (started_)
			return;
		started_ = true;
		start_();
	}

	void screen_t::stop()
	{
		if (started_)
			stop_();
		started_ = false;
	}

	rawTerminal_t::rawTerminal_t(FILE *const inputFile, FILE *const outputFile) :
		termInput{fileno(inputFile)}, termOutput{fileno(outputFile)}
	{
		if (!resizePipe.valid())
			throw std::exception{};
		fcntl(resizePipe.readFD(), F_SETFL, O_NONBLOCK);
	}

	/*bool*/void rawTerminal_t::write(const std::string_view &data) const noexcept
		{ ::write(termOutput, data.data(), data.size()); }
		//{ return ::write(termOutput, data.data(), data.size()) == data.size(); }

	void rawTerminal_t::setMouseTracking(const bool enable) noexcept
	{
		if (mouseTrackingEnabled_ == enable)
			return;
		mouseTracking(enable);
		mouseTrackingEnabled_ = enable;
	}

	void rawTerminal_t::mouseTracking(const bool enable) noexcept
	{
		write(enable ? escapes::mouseTrackingOn : escapes::mouseTrackingOff);
		if (enable)
			startGPMTracking();
		else
			stopGPMTracking();
	}

	void rawTerminal_t::signalInit() noexcept
	{
		contort::screens.push_back(this);
		struct sigaction action{};
		action.sa_flags = SA_RESTART;
		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
		action.sa_handler = contort::sigwinchHandler;
		sigaction(SIGWINCH, &action, nullptr);
		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
		action.sa_handler = contort::sigcontHandler;
		sigaction(SIGCONT, &action, nullptr);
	}

	void rawTerminal_t::signalRestore() noexcept
	{
		const auto self{std::find(contort::screens.begin(), contort::screens.end(), this)};
		if (self != std::end(contort::screens))
			contort::screens.erase(self);
	}

	void rawTerminal_t::sigwinchHandler(const int32_t) noexcept
	{
		if (!resized_)
			// TODO: Give writePipe_t support for the nicer IO functions
			[[maybe_unused]] const auto result{resizePipe.writeFD().write("R", 1)};//.write('R');
		resized_ = true;
		//screenBuf = nullptr | std::nullopt;
	}

	void rawTerminal_t::sigcontHandler(const int32_t) noexcept
	{
		start();
		stop();
		sigwinchHandler(0);
	}

	// https://github.com/python/cpython/blob/63298930fb531ba2bb4f23bc3b915dbf1e17e9e1/Lib/tty.py
	void rawTerminal_t::start_()
	{
		write(escapes::switchToAlternateBuffer);
		rowsUsed = std::nullopt;

		if (isatty(termInput))
		{
			termios result{};
			tcgetattr(termInput, &result);
			oldTermiosSettings = result;
			// NOLINTNEXTLINE(hicpp-signed-bitwise)
			result.c_iflag &= tcflag_t(~(BRKINT | ICRNL | INPCK | ISTRIP | IXON));
			// NOLINTNEXTLINE(hicpp-signed-bitwise)
			result.c_oflag &= tcflag_t(~OPOST);
			// NOLINTNEXTLINE(hicpp-signed-bitwise)
			result.c_cflag &= tcflag_t(~(CSIZE | PARENB));
			// NOLINTNEXTLINE(hicpp-signed-bitwise)
			result.c_lflag &= tcflag_t(~(ECHO | ICANON | IEXTEN | ISIG));
			// NOLINTNEXTLINE(hicpp-signed-bitwise)
			result.c_cflag |= CS8;
			result.c_cc[VMIN] = 1;
			result.c_cc[VTIME] = 0;
			tcsetattr(termInput, TCSAFLUSH, &result);
		}

		signalInit();
		nextTimeout = maxWait;

		// signal keys?

		//signals.emitSignal(this, INPUT_DESCRIPTORS_CHANGED)
		mouseTracking(mouseTrackingEnabled_);
		screen_t::start_();
	}

	void rawTerminal_t::stop_()
	{
		signalRestore();

		screen_t::stop_();
	}
} // namespace contort
