#pragma once

#include <mutex>

namespace NetherNet::RunLoop {
	enum ThreadState {
		Running = 0,
		Suspended,
		Stopped,
	};

	class Condition {
	public:
		std::unique_lock<std::mutex> AcquireLock();
		bool KeepGoing();
		bool StateDispatch(std::unique_lock<std::mutex>&& lock);
		bool StateRunning(std::unique_lock<std::mutex> lock);
		bool StateStopped(std::unique_lock<std::mutex> lock);
		bool StateSuspended(std::unique_lock<std::mutex> lock);
	public:
		ThreadState				mState;
		std::mutex				mLock;
		std::condition_variable cv;
	};

	class Controller {
		Controller() = default;
		Condition* GetCondition() const;
		void RequestRun() const;
		void RequestStop() const;
		void RequestSuspend() const;
		void UpdateSharedState(ThreadState state) const;
	private:
		std::shared_ptr<Condition> mCondition;
	};
}