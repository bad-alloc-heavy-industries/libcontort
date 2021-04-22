//SPDX-License-Identifier: BSD-3-Clause
#include <sys/select.h>
#include <variant>
#include <contort/eventLoop.hxx>
#include <substrate/fixed_vector>

namespace contort
{
	namespace event { struct idle_t final { }; }

	event::alarm_t selectEventLoop_t::addAlarm(std::chrono::microseconds waitFor, event::callback_t callback)
	{
		const auto time{std::chrono::steady_clock::now().time_since_epoch() + waitFor};
		const auto handle{event::alarm_t{std::chrono::duration_cast<std::chrono::microseconds>(time),
			tieBreak_++, callback}};
		alarms_.push(handle);
		return handle;
	}

	bool selectEventLoop_t::removeAlarm(const event::alarm_t &handle)
	{
		//alarms_.
		return false;
	}

	int32_t selectEventLoop_t::addWatchFile(const int32_t fd, const event::callback_t callback)
	{
		watchFiles_[fd] = callback;
		return fd;
	}

	bool selectEventLoop_t::removeWatchFile(const int32_t handle)
	{
		const auto entry{watchFiles_.find(handle)};
		if (entry == watchFiles_.end())
			return false;
		watchFiles_.erase(entry);
		return true;
	}

	size_t selectEventLoop_t::addEnterIdle(const event::callback_t callback)
	{
		const auto handle{idleHandle_++};
		idleCallbacks_.emplace(handle, callback);
		return handle;
	}

	bool selectEventLoop_t::removeEnterIdle(const size_t handle)
	{
		const auto entry{idleCallbacks_.find(handle)};
		if (entry == idleCallbacks_.end())
			return false;
		idleCallbacks_.erase(entry);
		return true;
	}

	void selectEventLoop_t::run()
	{
		didSomething_ = true;
		while (true)
		{
			loop();
		}
	}

	void selectEventLoop_t::enteringIdle() const
	{
		for (const auto &[_, callback] : idleCallbacks_)
			callback();
	}

	inline auto gatherFDs(const std::map<int32_t, event::callback_t> &watchFiles)
	{
		substrate::fixedVector_t<int32_t> fds{watchFiles.size()};
		size_t index{0};
		for (const auto &[fd, callback] : watchFiles)
			fds[index++] = fd;
		return fds;
	}

	inline void fdSet(fd_set &set, const substrate::fixedVector_t<int32_t> &fds) noexcept
	{
		FD_ZERO(&set);
		for (const auto fd : fds)
			FD_SET(fd, &set);
	}

	[[nodiscard]] inline bool anyFDSet(const fd_set &set, const substrate::fixedVector_t<int32_t> &fds) noexcept
	{
		for (const auto fd : fds)
		{
			if (FD_ISSET(fd, &set))
				return true;
		}
		return false;
	}

	constexpr bool operator >(const timeval &a, const timeval &b) noexcept
		{ return a.tv_sec > b.tv_sec || (a.tv_sec == b.tv_sec && a.tv_usec > b.tv_usec); }

	void selectEventLoop_t::loop()
	{
		const auto watchFDs{gatherFDs(watchFiles_)};
		fd_set readFDs{};
		fd_set errorFDs{};
		fdSet(readFDs, watchFDs);
		fdSet(errorFDs, watchFDs);

		std::variant<std::monostate, event::idle_t, std::chrono::microseconds> action{};
		std::optional<timeval> timeout{};
		if (!alarms_.empty() || didSomething_)
		{
			if (!alarms_.empty())
			{
				const auto waitUntil{std::get<0>(alarms_.top())};
				const auto time{waitUntil - std::chrono::steady_clock().now().time_since_epoch()};
				timeout = timeval{};
				if (time.count() >= 0)
				{
					const auto seconds{std::chrono::duration_cast<std::chrono::seconds>(time)};
					const auto microseconds{std::chrono::duration_cast<std::chrono::microseconds>(time - seconds)};
					timeout->tv_sec = seconds.count();
					timeout->tv_usec = microseconds.count();
				}
				action = waitUntil;
			}
			if (didSomething_ && (alarms_.empty() || (!alarms_.empty() && *timeout > timeval{})))
			{
				timeout = timeval{};
				action = event::idle_t{};
			}
		}
		// TODO: deal with failures
		const auto result{::select(FD_SETSIZE, &readFDs, nullptr, &errorFDs, timeout ? &*timeout : nullptr)};

		if (!anyFDSet(readFDs, watchFDs))
		{
			if (std::holds_alternative<event::idle_t>(action))
			{
				enteringIdle();
				didSomething_ = false;
			}
			else if (std::holds_alternative<std::chrono::microseconds>(action))
			{
				const auto [waitUntil, tieBreak, callback] = alarms_.top();
				alarms_.pop();
				callback();
				didSomething_ = true;
			}
		}

		for (const auto fd : watchFDs)
		{
			if (FD_ISSET(fd, &readFDs))
			{
				watchFiles_[fd]();
				didSomething_ = true;
			}
		}
	}
} // namespace contort
