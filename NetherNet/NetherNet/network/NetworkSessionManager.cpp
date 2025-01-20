
#include "NetworkSessionManager.hpp"
#include "../HNetherNet.hpp"

namespace NetherNet {
	void NetworkSessionManager::ClearPacketData(NetworkID id) {
		std::lock_guard lock(mQueuedPacketsMutex);
		mPacketData.erase(id);
	}

	bool NetworkSessionManager::CloseSessionWithUser(NetworkID id) {
		std::lock_guard lock(mQueuedPacketsMutex);
		auto session_elem = mSessionList.find(id);

		if (session_elem != mSessionList.end()) {
			auto session = session_elem->second;
			if (session.mOwnSession != nullptr)
				session.mOwnSession->Close();
			return true;
		}
		return false;
	}

	bool NetworkSessionManager::AcceptSessionWithUser(NetworkID id) {
		std::lock_guard lock(mQueuedPacketsMutex);
		auto session_elem = mSessionList.find(id);

		if (session_elem != mSessionList.end()) {
			auto session = session_elem->second;
			if (session.mOwnSession != nullptr) {
				session.mOwnSession->AcceptSession();
				return true;
			}
		}
		return false;
	}

	void NetworkSessionManager::CloseSessionIfInactive(NetworkID id, NetworkSessionRecord& record) {
		if (record.mOwnSession != nullptr
			&& !record.mOwnSession->IsConnectionAlive()
			&& record.mOwnSession->IsInactiveSession()) {

			auto conn_id_str = std::to_string(record.mConnectionId);
			NetherNet::NetherNetTransport_LogMessage(
				4,
				"[%llu] Closing inactive session: [RemoteID: %llu] [ConnectionId: %s]",
				mSimpleNetworkInterface->mReceiverId,
				id,
				conn_id_str.data());

			record.mOwnSession->ProcessError(ESessionError::ESessionErrorInactivityTimeout);
			record.mOwnSession->Close();
		}
	}

	NetworkSession* NetworkSessionManager::GetCurrentSession(NetworkID id) {
		if (FilterDeadSession(id, mSessionList[id]))
			return nullptr;
		return mSessionList[id].mOwnSession;
	}

	bool NetworkSessionManager::GetSessionState(NetworkID id, SessionState* outState) {
		std::lock_guard lock(mQueuedPacketsMutex);
		auto current_session = GetCurrentSession(id);
		if (current_session)
			return current_session->GetSessionState(outState);
		return false;
	}

	bool NetworkSessionManager::HasKnownConnection(NetworkID id) {
		std::lock_guard lock(mQueuedPacketsMutex);
		return mSessionList[id].mOwnSession != nullptr;
	}

	bool NetworkSessionManager::InitiateIncomingSession(NetworkID id, uint64_t const& connectionId, std::unique_ptr<webrtc::SessionDescriptionInterface> iface, SignalingChannelId sig_channel_id) {
		auto current_session = GetCurrentSession(id);
		if (current_session) {
			if (/*TODO*/ true) {
				if (connectionId == current_session->mConnectionId) {
					auto conn_id_str = std::to_string(connectionId);
					NetherNet::NetherNetTransport_LogMessage(
						2,
						"[%llu] Ignoring incoming connection because it overlaps with existing outgoing connection: [RemoteID: %llu] [C"
						"onnectionId: %s]",
						mSimpleNetworkInterface->mReceiverId,
						id,
						conn_id_str.data());
					return false;
				}
			}
		}

		ClearPacketData(id);
		if (mSessionList.contains(id)) {
			auto signal_thread = getSignalThread();
			if (!signal_thread->IsOnThread()) {
				auto& session_record = mSessionList[id];
				auto rtc_thread = signal_thread->LoadRtcThread();

				if (rtc_thread) {
					rtc_thread->PostTask([&]() {
						//TODO: cleanup here
					});
				}
			}

			auto& session_record = mSessionList[id];
			session_record.mOwnSession = nullptr;
		}

		//TODO
		NetworkSession updated_session(this, std::nullopt);
		updated_session.InitializeIncoming(id, connectionId, std::move(iface), sig_channel_id);
		auto& updated_session_rec = mSessionList[id];
		updated_session_rec.mOwnSession = &updated_session;

		return true;
	}
}