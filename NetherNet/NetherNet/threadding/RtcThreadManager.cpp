
#include "RtcThreadManager.hpp"

namespace NetherNet {

	/*std::shared_ptr<rtc::Thread>*/ rtc::Thread* RtcThreadManager::GetRtcThread() {
		return LoadRtcThread();
	}

	/*std::shared_ptr<rtc::Thread>*/rtc::Thread* RtcThreadManager::LoadRtcThread() {
		return mThread.load(std::memory_order::memory_order_relaxed);
	}

	bool RtcThreadManager::IsOnThread() {
		auto RtcThread = LoadRtcThread()/*.get()*/;
		if (RtcThread) {
			return RtcThread->IsCurrent();
		}
		return false;
	}

	void RtcThreadManager::Initialize(const char* threadName, ThreadInit const& threadInit, std::unique_ptr<rtc::SocketServer>&& socketServer) {
		rtc::Thread ServerThread(std::move(socketServer));

		ServerThread.SetName(threadName, nullptr);
		mThread.store(&ServerThread, std::memory_order::memory_order_relaxed);
	}

	void RtcThreadManager::Shutdown() {
		auto thread = mThread.load();
		if (thread) {
			thread->Stop();
			mThread.store(nullptr);
		}
	}

	RtcThreadManager g_SignalThread;
	RtcThreadManager g_WorkerThread;

	RtcThreadManager* getSignalThread() { return &g_SignalThread; }
	RtcThreadManager* getWorkerThread() { return &g_WorkerThread; }
}