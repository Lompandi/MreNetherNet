
#include "Controller.hpp"

namespace NetherNet::RunLoop {
	bool Condition::StateRunning(std::unique_lock<std::mutex> lock) {
		if(lock.owns_lock())
			lock.unlock();
		return true;
	}

	bool Condition::StateStopped(std::unique_lock<std::mutex> lock) {
		if (lock.owns_lock())
			lock.unlock();
		return false;
	}

	bool Condition::StateSuspended(std::unique_lock<std::mutex> lock) {
		

	}
}