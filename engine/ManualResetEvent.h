#pragma once

#include <Disruptor/ClockConfig.h>
#include "ResetEvent.h"

class ManualResetEvent : public ResetEvent
{
	public:
		explicit ManualResetEvent(bool signaled = false);

		bool isSet() const override;

		void set(bool ensureWaitIsTriggered = false) override;
		void reset() override;

		bool waitOne();
		bool wait(Disruptor::ClockConfig::Duration timeDuration) override;
};

