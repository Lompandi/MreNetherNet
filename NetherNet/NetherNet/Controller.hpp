#pragma once

#include <mutex>

namespace NetherNet::RunLoop {
	class Condition {
		void AcquireLock();
		void KeepGoing();
		void StateDispatch(std::unique_lock<std::mutex>&& lock);
		bool StateRunning(std::unique_lock<std::mutex> lock);
		bool StateStopped(std::unique_lock<std::mutex> lock);
		bool StateSuspended(std::unique_lock<std::mutex> lock);
	};

	class Controller {

	};
}