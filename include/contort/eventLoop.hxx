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

namespace contort
{
	namespace event
	{
		using callback_t = void (*)();
		using alarm_t = std::tuple<std::chrono::microseconds, std::size_t, callback_t>;
		template<typename T> using heap_t = std::priority_queue<T, std::vector<T>, std::greater<T>>;
	}

	struct CONTORT_CLS_API eventLoop_t
	{
	public:
		virtual ~eventLoop_t() noexcept = default;
		virtual event::alarm_t addAlarm(std::chrono::microseconds waitFor, event::callback_t callback) = 0;
		virtual int32_t addWatchFile(int32_t fd, event::callback_t callback) = 0;
		virtual size_t addEnterIdle(event::callback_t callback) = 0;
		virtual void run() = 0;
	};

	struct CONTORT_CLS_API selectEventLoop_t final : eventLoop_t
	{
	private:
		bool didSomething_{false};
		event::heap_t<event::alarm_t> alarms_{};
		std::map<int32_t, event::callback_t> watchFiles_{};
		std::map<size_t, event::callback_t> idleCallbacks_{};
		std::atomic<std::size_t> idleHandle_{0};
		std::atomic<std::size_t> tieBreak_{0};

		void enteringIdle() const;
		void loop();

	public:
		~selectEventLoop_t() noexcept = default;
		event::alarm_t addAlarm(std::chrono::microseconds waitFor, event::callback_t callback) final;
		int32_t addWatchFile(int32_t fd, event::callback_t callback) final;
		size_t addEnterIdle(event::callback_t callback) final;
		void run() final;
	};
} // namespace contort

#endif /*CONTORT_EVENT_LOOP__HXX*/
