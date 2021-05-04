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
		constexpr static auto charSO{"\x0E"sv};
		constexpr static auto charSI{"\x0F"sv};

		constexpr static auto switchToAlternateBuffer{"\x1B\x37\x1B[?47h"sv};
		constexpr static auto restoreNormalBuffer{"\x1B[?47l\x1B\x38"sv};

		constexpr static auto hideCursor{"\x1B[?25l"sv};
		constexpr static auto showCursor{"\x1B[?25h"sv};

		constexpr static auto mouseTrackingOn{"\x1B[?1000h\x1B[?1002h"sv};
		constexpr static auto mouseTrackingOff{"\x1B[?1002l\x1B[?1000l"sv};

		constexpr static auto designateG1Special{"\x1B[K"sv};
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

	bool rawTerminal_t::write(const std::string_view &data) const noexcept
	{
		const auto result{::write(termOutput, data.data(), data.size())};
		if (result < 0)
			return false;
		return size_t(result) == data.size();
	}

	void rawTerminal_t::setMouseTracking(const bool enable)
	{
		if (mouseTrackingEnabled_ == enable)
			return;
		mouseTracking(enable);
		mouseTrackingEnabled_ = enable;
	}

	void rawTerminal_t::mouseTracking(const bool enable)
	{
		if (!write(enable ? escapes::mouseTrackingOn : escapes::mouseTrackingOff))
			throw ioError_t{};
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
		//screenBuff = nullptr | std::nullopt;
	}

	void rawTerminal_t::sigcontHandler(const int32_t) noexcept
	{
		start();
		stop();
		sigwinchHandler(0);
	}

	void rawTerminal_t::setInputNonBlock() const noexcept
	{
		const auto flags{fcntl(termInput, F_GETFL)};
		// NOLINTNEXTLINE(hicpp-signed-bitwise)
		fcntl(termInput, F_SETFL, flags | O_NONBLOCK);
	}

	void rawTerminal_t::clearInputNonBlock() const noexcept
	{
		const auto flags{fcntl(termInput, F_GETFL)};
		// NOLINTNEXTLINE(hicpp-signed-bitwise)
		fcntl(termInput, F_SETFL, flags & (~O_NONBLOCK));
	}

	// https://github.com/python/cpython/blob/63298930fb531ba2bb4f23bc3b915dbf1e17e9e1/Lib/tty.py
	void rawTerminal_t::start_()
	{
		if (!write(escapes::switchToAlternateBuffer))
			throw ioError_t{};
		rowsUsed = std::nullopt;

		setInputNonBlock();
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
		clear();
		//signals.emitSignal(this, INPUT_DESCRIPTORS_CHANGED)
		signalRestore();

		clearInputNonBlock();
		if (isatty(termInput))
		{
			termios settings{*oldTermiosSettings};
			tcsetattr(termInput, TCSADRAIN, &settings);
		}

		mouseTracking(false);
		if (!write(escapes::charSI) ||
			!write(escapes::restoreNormalBuffer) ||
			!write(escapes::showCursor))
			throw ioError_t{};

		// signal keys?

		screen_t::stop_();
	}

	fixedVector_t<int32_t> rawTerminal_t::inputDescriptors() const
	{
		/*if (!started_)
			return {};*/
		fixedVector_t<int32_t> result{2};
		result[0] = resizePipe.readFD();
		result[1] = termInput;
		return result;
	}

	std::vector<int32_t> rawTerminal_t::getAvailableRawInput() const
	{
		auto codes{getGPMCodes()};
		const auto keyCodes{getKeyCodes()};
		codes.insert(codes.end(), keyCodes.begin(), keyCodes.end());

		// TODO: clean resizePipe out
		return codes;
	}

	void rawTerminal_t::parseInput(eventLoop_t *eventLoop, const std::function<screen::callback_t> &callback,
		std::vector<int32_t> codes, const bool waitForMore)
	{
		if (inputTimeout && eventLoop)
		{
			eventLoop->removeAlarm(*inputTimeout);
			inputTimeout = std::nullopt;
		}

		std::vector<int32_t> processed{};
		std::vector<int32_t> processedCodes{};

		callback(processed, processedCodes);
	}

	std::optional<char> rawTerminal_t::tryReadChar() const
	{
		char value{};
		const auto result{read(termInput, &value, 1)};
		if (result == -1 && errno == EAGAIN)
			return std::nullopt;
		else if (result < 0)
			throw ioError_t{};
		return value;
	}

	std::vector<int32_t> rawTerminal_t::getKeyCodes() const
	{
		std::vector<int32_t> codes{};
		while (true)
		{
			const auto code{tryReadChar()};
			if (code)
				codes.emplace_back(*code);
			else
				break;
		}
		return codes;
	}

	void rawTerminal_t::startGPMTracking()
	{
	}

	void rawTerminal_t::stopGPMTracking()
	{
	}

	std::vector<int32_t> rawTerminal_t::getGPMCodes() const
	{
		return {};
	}

	void rawTerminal_t::setupG1() noexcept
	{
		if (setupG1Done_)
			return;
		while (!write(escapes::designateG1Special))
			continue;
		setupG1Done_ = true;
	}

	void rawTerminal_t::clear() noexcept
	{
		//screenBuff = nullptr | std::nullopt;
		setupG1Done_ = true;
	}
} // namespace contort
