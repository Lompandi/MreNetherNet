
#include <rtc_base/copy_on_write_buffer.h>
#include <rtc_base/task_utils/to_queued_task.h>

#include "../HNetherNet.hpp"
#include "NetworkSession.hpp"
#include "../threadding/RtcThreadManager.hpp"

namespace NetherNet {
	void NetworkSession::UpdateSessionActivity() {
		return;
	}


	//TODO 
	void NetworkSession::SendPacket(
		const char* pdata,
		uint32_t	cbdata,
		::NetherNet::ESendType send_type
	) {
		UpdateSessionActivity();
		auto signalThread = getSignalThread();

		rtc::CopyOnWriteBuffer mSendBuffer(cbdata, cbdata);
		
		auto thread = signalThread->LoadRtcThread();
		if (thread) {
			//...
		}
	}

	void NetworkSession::AcceptSession() {
		UpdateSessionActivity();

		if (mConnNegotiationState == ENegotiationState::WaitingForAccept) {
			mConnNegotiationState = ENegotiationState::ICEProcessing;

			TrySendSessionResponse();
		}
		else {
			NetherNet::NetherNetTransport_LogMessage(
				2,
				"[%llu] Error: Trying to accept when no accept pending.",
				0 /* TODO: unknown type pending */
			);
		}
	}

	void NetworkSession::TrySendSessionResponse() {
		UpdateSessionActivity();

		std::string str_connection_id = std::to_string(mConnectionId);
		::NetherNet::NetherNetTransport_LogMessage(
			4,
			"[%llu] Queueing connection response : [RemoteID: %llu] [ConnectionID: %s]",
			0, /* TODO: Currently unknown */
			mRemoteID,
			mConnectionId
		);

		auto signal_thread = getSignalThread();
		auto response_thread = signal_thread->LoadRtcThread();

		response_thread->PostTask(
			//TODO
		);
	}

	void NetworkSession::ApplyConnectionFlags(
		webrtc::PeerConnectionInterface::RTCConfiguration* config,
		uint32_t flag) {
		if ((flag & 1) != 0)
			config->type = webrtc::PeerConnectionInterface::IceTransportsType::kRelay;
		if ((flag & 2) != 0)
			config->enable_dtls_srtp = true;
		mConnectionFlag = flag;
	}
}