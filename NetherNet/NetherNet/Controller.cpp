
#include "Controller.hpp"

namespace NetherNet::RunLoop {
	bool Condition::StateRunning(std::unique_lock<std::mutex> lock) {
		return true;
	}

	bool Condition::StateStopped(std::unique_lock<std::mutex> lock) {
		return false;
	}

	bool Condition::StateSuspended(std::unique_lock<std::mutex> lock) {
		cv.wait(lock);
		auto result = StateDispatch(std::move(lock));
		return result;
	}

	bool Condition::StateDispatch(std::unique_lock<std::mutex>&& lock) {
		if (mState == ThreadState::Stopped)
			return StateStopped(std::move(lock));
		else if (mState == ThreadState::Suspended)
			return StateSuspended(std::move(lock));
		else if (mState == ThreadState::Running)
			return StateRunning(std::move(lock));
		else
			return false;
	}

	bool Condition::KeepGoing() {
		auto lock = AcquireLock();
		auto result = StateDispatch(std::move(lock));
		return result;
	}

	std::unique_lock<std::mutex> Condition::AcquireLock() {
		std::unique_lock<std::mutex> lock(mLock);
		return lock;
	}

	Condition* Controller::GetCondition() const {
		return mCondition.get();
	}

	void Controller::RequestRun() const {
		UpdateSharedState(ThreadState::Running);
	}

	void Controller::RequestStop() const {
		UpdateSharedState(ThreadState::Stopped);
	}

	void Controller::RequestSuspend() const {
		UpdateSharedState(ThreadState::Suspended);
	}

	void Controller::UpdateSharedState(ThreadState state) const {
		auto lock = mCondition->AcquireLock();
		lock.lock();
		auto state_before = mCondition->mState;
		mCondition->mState = state;
		lock.unlock();
		if (state != state_before)
			mCondition->cv.notify_all();
	}
}