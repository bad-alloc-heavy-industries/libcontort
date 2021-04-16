//SPDX-License-Identifier: BSD-3-Clause
#ifndef CONTORT_EVENT_LOOP__HXX
#define CONTORT_EVENT_LOOP__HXX

#include <cstdint>
#include <vector>
#include <map>
#include <contort/defs.hxx>

namespace contort
{
	namespace event
	{
		using callback_t = void (*)();
	}

	struct CONTORT_CLS_API eventLoop_t
	{
	public:
		virtual int32_t watchFile(int32_t fd, event::callback_t callback) = 0;
		virtual void run() = 0;
	};

	struct CONTORT_CLS_API selectEventLoop_t final : eventLoop_t
	{
	private:
		bool didSomething_{false};
		std::map<int32_t, event::callback_t> watchFiles_{};

		void loop();

	public:
		int32_t watchFile(int32_t fd, event::callback_t callback) final;
		void run() final;
	};
} // namespace contort

#endif /*CONTORT_EVENT_LOOP__HXX*/
