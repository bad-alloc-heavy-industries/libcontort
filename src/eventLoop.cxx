//SPDX-License-Identifier: BSD-3-Clause
#include <sys/select.h>
#include <contort/eventLoop.hxx>
#include <substrate/fixed_vector>

namespace contort
{
	int32_t selectEventLoop_t::watchFile(const int32_t fd, const event::callback_t callback)
	{
		watchFiles_[fd] = callback;
		return fd;
	}

	void selectEventLoop_t::run()
	{
		didSomething_ = true;
		while (true)
		{
			loop();
		}
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

	void selectEventLoop_t::loop()
	{
		const auto watchFDs{gatherFDs(watchFiles_)};
		fd_set readFDs{};
		fd_set errorFDs{};
		fdSet(readFDs, watchFDs);
		fdSet(errorFDs, watchFDs);

		if (didSomething_)
		{
			timeval timeout{};
			::select(FD_SETSIZE, &readFDs, nullptr, &errorFDs, &timeout);
		}
		else
		{
			::select(FD_SETSIZE, &readFDs, nullptr, &errorFDs, nullptr);
		}
	}
} // namespace contort
