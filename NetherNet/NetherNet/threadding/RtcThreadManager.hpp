#pragma once

#include <atomic>
#include <memory>
#include <rtc_base/thread.h>

namespace NetherNet {
	struct ThreadInit;

	class RtcThreadManager {
	public:
		RtcThreadManager() = default;

		void Initialize(const char* threadName, ThreadInit const& threadInit, std::unique_ptr<rtc::SocketServer>&& socketServer);
		void Shutdown();

		bool IsOnThread();
		[[nodiscard]] /*std::shared_ptr<rtc::Thread>*/ rtc::Thread* GetRtcThread();

		/*std::shared_ptr<rtc::Thread>*/ rtc::Thread* LoadRtcThread();

	private:
		std::atomic<rtc::Thread*> mThread;
	};

	extern RtcThreadManager g_SignalThread;
	extern RtcThreadManager g_WorkerThread;

	RtcThreadManager* getSignalThread();
	RtcThreadManager* getWorkerThread();
}