#pragma once
#include <chrono>

/**
@brief event structure for iocp event.
@author Gurnwoo Kim
*/
struct Event {
	size_t targetID; ///< target obj id.
	int eventType; ///< event type for iocp.
	std::chrono::high_resolution_clock::time_point wakeupTime; ///< after arriving this time, event will execute.

	constexpr bool operator<(const Event& other) const
	{
		return (wakeupTime > other.wakeupTime);
	}
};