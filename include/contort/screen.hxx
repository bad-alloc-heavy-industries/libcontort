//SPDX-License-Identifier: BSD-3-Clause
#ifndef CONTORT_SCREEN__HXX
#define CONTORT_SCREEN__HXX

#include <cstdint>
#include <cstdio>
#include <string_view>
#include <variant>
#include <optional>
#include <chrono>
#include <functional>
#include <exception>
#include <termios.h>
#include <contort/defs.hxx>
#include <substrate/pipe>
#include <substrate/fixed_vector>

namespace contort
{
	namespace screen
	{
		using callback_t = void(const std::vector<int32_t> &, const std::vector<int32_t> &);
	}

	struct CONTORT_CLS_API ioError_t final : std::exception
	{
		[[nodiscard]] const char *what() const noexcept final { return "Error during IO operation"; }
	};

	struct CONTORT_CLS_API screen_t
	{
	private:
		bool started_{false};

	protected:
		constexpr screen_t(screen_t &&) noexcept = default;
		screen_t &operator =(screen_t &&) noexcept = default;

		virtual void start_() { }
		virtual void stop_() { }

	public:
		constexpr screen_t() noexcept = default;
		screen_t(const screen_t &) noexcept = delete;
		screen_t &operator =(const screen_t &) noexcept = delete;
		virtual ~screen_t() noexcept = default;

		void start();
		void stop();

		virtual void setMouseTracking(bool enable = true) = 0;
	};

	struct CONTORT_CLS_API rawTerminal_t final : screen_t
	{
	private:
		bool resized_{false};
		bool mouseTrackingEnabled_{false};
		bool setupG1Done_{false};
		std::optional<std::size_t> rowsUsed{};
		std::optional<termios> oldTermiosSettings{};
		std::optional<std::chrono::seconds> maxWait{};
		std::optional<std::chrono::seconds> nextTimeout{};

		int32_t termInput{-1};
		int32_t termOutput{-1};
		substrate::pipe_t resizePipe{};

		void start_() final;
		void stop_() final;

		void setInputNonBlock() const noexcept;
		void clearInputNonBlock() const noexcept;

		void startGPMTracking();
		void stopGPMTracking();
		std::vector<int32_t> getGPMCodes() const;
		void mouseTracking(bool enable);
		std::optional<char> tryReadChar() const;
		std::vector<int32_t> getKeyCodes() const;
		void sigwinchHandler(int32_t signum) noexcept;
		void sigcontHandler(int32_t signum) noexcept;

		void setupG1() noexcept;

	public:
		rawTerminal_t(FILE *inputFile = stdin, FILE *outputFile = stdout);
		rawTerminal_t(const rawTerminal_t &) = delete;
		rawTerminal_t(rawTerminal_t &&) = default;
		~rawTerminal_t() noexcept final = default;
		rawTerminal_t &operator =(const rawTerminal_t &) = delete;
		rawTerminal_t &operator =(rawTerminal_t &&) = default;

		[[nodiscard]] bool write(const std::string_view &data) const noexcept;

		void setMouseTracking(bool enable = true) final;
		[[nodiscard]] substrate::fixedVector_t<int32_t> inputDescriptors() const;
		[[nodiscard]] std::vector<int32_t> getAvailableRawInput() const;
		void parseInput(eventLoop_t *eventLoop, const std::function<screen::callback_t> &callback,
			std::vector<int32_t> codes, bool waitForMore = true);

		void signalInit() noexcept;
		void signalRestore() noexcept;
		void clear() noexcept;
	};
} // namespace contort

#endif /*CONTORT_SCREEN__HXX*/
