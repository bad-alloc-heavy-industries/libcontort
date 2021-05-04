//SPDX-License-Identifier: BSD-3-Clause
#ifndef CONTORT_EVENT_LOOP__HXX
#define CONTORT_EVENT_LOOP__HXX

#include <cstdint>
#include <vector>
#include <map>
#include <tuple>
#include <chrono>
#include <atomic>
#include <queue>
#include <functional>
#include <contort/defs.hxx>

namespace contort::event
{
	using callback_t = void();
	using alarm_t = std::tuple<std::chrono::microseconds, std::size_t, std::function<event::callback_t>>;
	template<typename T> using heap_t = std::priority_queue<T, std::vector<T>, std::greater<T>>;
} // namespace contort::event

namespace std
{
	template<> struct greater<contort::event::alarm_t>
	{
		using alarm_t = contort::event::alarm_t;

		[[nodiscard]] constexpr bool operator ()(const alarm_t &a, const alarm_t &b) noexcept
		{
			const auto &[aTime, aTieBreak, aCallback] = a;
			const auto &[bTime, bTieBreak, bCallback] = b;
			return aTime > bTime || (aTime == bTime && aTieBreak > bTieBreak);
		}
	};
} // namespace std

namespace contort
{
	struct CONTORT_CLS_API eventLoop_t
	{
	public:
		constexpr eventLoop_t() noexcept = default;
		eventLoop_t(const eventLoop_t &) noexcept = delete;
		eventLoop_t(eventLoop_t &&) noexcept = delete;
		eventLoop_t &operator =(const eventLoop_t &) noexcept = delete;
		eventLoop_t &operator =(eventLoop_t &&) noexcept = delete;
		virtual ~eventLoop_t() noexcept = default;
		virtual event::alarm_t addAlarm(std::chrono::microseconds waitFor,
			std::function<event::callback_t> callback) = 0;
		virtual bool removeAlarm(const event::alarm_t &handle) = 0;
		virtual int32_t addWatchFile(int32_t fd, std::function<event::callback_t> callback) = 0;
		virtual bool removeWatchFile(int32_t handle) = 0;
		virtual size_t addEnterIdle(std::function<event::callback_t> callback) = 0;
		virtual bool removeEnterIdle(size_t handle) = 0;
		virtual void run() = 0;
	};

	struct CONTORT_CLS_API selectEventLoop_t final : eventLoop_t
	{
	private:
		bool didSomething_{false};
		event::heap_t<event::alarm_t> alarms_{};
		std::map<int32_t, std::function<event::callback_t>> watchFiles_{};
		std::map<size_t, std::function<event::callback_t>> idleCallbacks_{};
		std::atomic<std::size_t> idleHandle_{0};
		std::atomic<std::size_t> tieBreak_{0};

		void enteringIdle() const;
		void loop();

	public:
		selectEventLoop_t() noexcept = default;
		selectEventLoop_t(const selectEventLoop_t &) noexcept = delete;
		selectEventLoop_t(selectEventLoop_t &&) noexcept = delete;
		selectEventLoop_t &operator =(const selectEventLoop_t &) noexcept = delete;
		selectEventLoop_t &operator =(selectEventLoop_t &&) noexcept = delete;
		~selectEventLoop_t() noexcept final = default;
		event::alarm_t addAlarm(std::chrono::microseconds waitFor,
			std::function<event::callback_t> callback) final;
		bool removeAlarm(const event::alarm_t &handle) final;
		int32_t addWatchFile(int32_t fd, std::function<event::callback_t> callback) final;
		bool removeWatchFile(int32_t handle) final;
		size_t addEnterIdle(std::function<event::callback_t> callback) final;
		bool removeEnterIdle(size_t handle) final;
		void run() final;
	};
} // namespace contort

#endif /*CONTORT_EVENT_LOOP__HXX*/
