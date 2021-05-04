//SPDX-License-Identifier: BSD-3-Clause
#include <contort/mainLoop.hxx>

namespace contort
{
	mainLoop_t::mainLoop_t(widget_t *const widget, std::unique_ptr<screen_t> screen,
		const bool handleMouse, std::unique_ptr<eventLoop_t> eventLoop) noexcept :
		widget_{widget}, screen_{screen ? std::move(screen) : std::make_unique<rawTerminal_t>()},
		eventLoop_{std::move(eventLoop)}, handleMouse_{handleMouse}
	{
		/*if (palette)
			screen.registerPalette(palette);*/

		if (!eventLoop_)
			eventLoop_ = std::make_unique<selectEventLoop_t>();
	}

	int32_t mainLoop_t::run()
	{
		start();
		try
			{ eventLoop_->run(); }
		catch (...)
		{
			screen_->stop();
			throw;
		}
		stop();
		return 0;
	}

	void mainLoop_t::start()
	{
		screen_->start();
		if (handleMouse_)
			screen_->setMouseTracking();

		screen_->unhookEventLoop(*eventLoop_);
		screen_->hookEventLoop(*eventLoop_,
			[this](const std::vector<int32_t> &keys, const std::vector<int32_t> &rawKeystrokes)
				{ update(keys, rawKeystrokes); }
		);
	}

	void mainLoop_t::stop()
	{
		screen_->unhookEventLoop(*eventLoop_);
		screen_->stop();
	}

	void mainLoop_t::update(const std::vector<int32_t> &keys, const std::vector<int32_t> &rawKeystrokes) noexcept
	{
		const auto filteredKeys{inputFilter(keys, rawKeystrokes)};
		if (!filteredKeys.empty())
		{
			processInput(filteredKeys);
		}
	}

	bool mainLoop_t::processInput(const std::vector<int32_t> &keys) noexcept
	{
		if (!screenSize_)
			screenSize_ = screen_->colsRows();
		bool result{false};
		for (const auto &key : keys)
		{
		}
		return result;
	}

	std::vector<int32_t> mainLoop_t::inputFilter(const std::vector<int32_t> &keys,
		const std::vector<int32_t> &/*rawKeystrokes*/) const noexcept
	{
		return keys;
	}
} // namespace contort
