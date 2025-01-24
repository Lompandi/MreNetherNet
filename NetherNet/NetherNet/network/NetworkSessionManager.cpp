
#include "../connection/NetworkSession.hpp"
#include "NetworkSessionManager.hpp"
#include "../HNetherNet.hpp"

namespace NetherNet {
	void NetworkSessionManager::ClearPacketData(NetworkID id) {
		std::lock_guard lock(mSessionCreationMutex);
		mPacketData.erase(id);
	}

	bool NetworkSessionManager::CloseSessionWithUser(NetworkID id) {
		std::lock_guard lock(mSessionCreationMutex);
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
		std::lock_guard lock(mSessionCreationMutex);
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
				LogSeverity::Information,
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
		std::lock_guard lock(mSessionCreationMutex);
		auto current_session = GetCurrentSession(id);
		if (current_session)
			return current_session->GetSessionState(outState);
		return false;
	}

	bool NetworkSessionManager::HasKnownConnection(NetworkID id) {
		std::lock_guard lock(mSessionCreationMutex);
		return mSessionList[id].mOwnSession != nullptr;
	}

	bool NetworkSessionManager::InitiateIncomingSession(NetworkID id, uint64_t const& connectionId, std::unique_ptr<webrtc::SessionDescriptionInterface> iface, SignalingChannelId sig_channel_id) {
		auto current_session = GetCurrentSession(id);
		if (current_session) {
			if (/*TODO*/ true) {
				if (connectionId == current_session->mConnectionId) {
					auto conn_id_str = std::to_string(connectionId);
					NetherNet::NetherNetTransport_LogMessage(
						LogSeverity::Error,
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

		//TODO: Fix NetworkSession constructor
		/*NetworkSession updated_session(this, std::nullopt);
		updated_session.InitializeIncoming(id, connectionId, std::move(iface), sig_channel_id);
		auto& updated_session_rec = mSessionList[id];
		updated_session_rec.mOwnSession = &updated_session;
		*/
		return true;
	}

	NetworkSession* NetworkSessionManager::InitiateOutgoingSession(NetworkID id) {
		auto it = mSessionList.lower_bound(id);
		if (it != mSessionList.end() && it->first == id)
			return it->second.mOwnSession;

		ClearPacketData(id);
		auto current_session = GetCurrentSession(id);
		if (!current_session) {
			/*
			TODO: Fix NetworkSession constructor

			NetworkSession new_session(this, std::nullopt);
			new_session.InitializeOutgoing(id);
			
			mSessionList[id].mOwnSession = &new_session;
			return mSessionList[id].mOwnSession;*/
		}
		return current_session;
	}

	bool NetworkSessionManager::FilterDeadSession(NetworkID id, NetworkSessionRecord& record) const {
		if (record.mOwnSession == nullptr)
			return false;

		auto session = record.mOwnSession;

		auto negotiationTimeOut = mSimpleNetworkInterface->GetNegotiationTimeout();
		if (!session->IsDeadSession(negotiationTimeOut))
			return false;

		auto conn_id_str = std::to_string(record.mConnectionId);
		NetherNet::NetherNetTransport_LogMessage(
			LogSeverity::Information,
			"[%llu] Deleting dead session: [RemoteID: %llu] [ConnectionId: %s]",
			mSimpleNetworkInterface->mReceiverId,
			id,
			conn_id_str.c_str());

		auto signal_thread = getSignalThread();
		if (signal_thread->IsOnThread()) {
			record.mOwnSession = nullptr;
		}
		else {
			// If not on the signal thread, we post the task to another thread
			auto rtc_thread = signal_thread->LoadRtcThread();
			if (rtc_thread) {
				rtc_thread->PostTask([&]() {
					record.mOwnSession = nullptr;
				});
			}
		}
		return true;
	}

	NetworkSession* 
	NetworkSessionManager::FindSpecificSession(NetworkID id, uint64_t const& connectionid) {
		auto it = mSessionList.find(id);
		if (it == mSessionList.end())
			return nullptr;

		if (it->second.mOwnSession != nullptr)
			if (it->second.mConnectionId == connectionid)
				return it->second.mOwnSession;
		return nullptr;
	}

	bool 
	NetworkSessionManager::IsPacketAvailable(NetworkID id, uint32_t* mcbMessage) {
		std::lock_guard lock(mSessionCreationMutex);
		auto data_elem = mPacketData.find(id);

		if (data_elem == mPacketData.end())
			return false;

		*mcbMessage = data_elem->second.size();
		return true;
	}

	void 
	NetworkSessionManager::OnRemoteMessageReceived(NetworkID id, void const* pdata, size_t size) {
		if (size)
			RemoteMessageReceived(id, pdata, size);
	}

	bool 
	NetworkSessionManager::OpenSessionWithUser(NetworkID id) {
		/*std::lock_guard lock(mQueuedPacketsMutex);
		NetherNet::NetherNetTransport_LogMessage(LogSeverity::Information, "Trying to open connection to %llu", id);

		auto record_elem = mSessionList.find(id);
		SessionState session_state;

		if (record_elem != mSessionList.end()) {
			auto session = record_elem->second;
			session_state is_established = session.mOwnSession->GetSessionState();
			if (is_established) {
				NetherNet::NetherNetTransport_LogMessage(
					2,
					"[%llu] Outgoing session already established or being established: [RemoteID: %llu]",
					mSimpleNetworkInterface->mReceiverId,
					id);
				return false;
			}
		}
		if (!is_established) {
			InitiateOutgoingSession(id);
			NetherNet::NetherNetTransport_LogMessage(LogSeverity::Information, "Successfully scheduled to open a connection to %llu", id);
			return true;
		}
		NetherNet::NetherNetTransport_LogMessage(
			2,
			"[%llu] Outgoing session has an error: [RemoteID: %llu]",
			mSimpleNetworkInterface->mReceiverId,
			id);
		return false;*/
	}

	void 
	NetworkSessionManager::ProcessError(NetworkID networkIDRemote, ESessionError err) {
		std::lock_guard lock(mSessionCreationMutex);
		auto cur_session = GetCurrentSession(networkIDRemote);
		if (cur_session)
			cur_session->ProcessError(err);
	}

	void 
	NetworkSessionManager::RemoteMessageReceived(NetworkID id, void const* pdata, size_t size) {

	}

	int 
	NetworkSessionManager::SendPacket(NetworkID id, const char* pbData, uint32_t cbData, ESendType send_type) {
		std::lock_guard lock(mSessionCreationMutex);
		auto current_session = GetCurrentSession(id);
		if (current_session)
			current_session->SendPacket(pbData, cbData, send_type);
		return current_session != nullptr;
	}
}