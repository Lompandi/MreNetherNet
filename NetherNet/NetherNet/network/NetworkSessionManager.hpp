#ifndef NETWORK_SESSION_MANAGER_HPP
#define NETWORK_SESSION_MANAGER_HPP

#pragma once
#include "NetworkID.hpp"

#include "signaling/ConnectError.hpp"
#include "signaling/ConnectResponse.hpp"
#include "signaling/ConnectRequest.hpp"
#include "signaling/CandidateAdd.hpp"

#include "../connection/SimpleNetworkInterfaceImpl.hpp"

namespace NetherNet { class NetworkSession; }
namespace NetherNet { struct SessionState; }

namespace NetherNet {
	class NetworkSessionManager {
	public:
		struct NetworkSessionRecord {
		public:
			NetworkSession* mOwnSession;	//this + 0x0
			NetworkID		mConnectionId;  //this + 0x20
		};

		bool AcceptSessionWithUser(NetworkID id);

		void ClearPacketData(NetworkID id);

		void CloseSessionIfInactive(NetworkID id, NetworkSessionRecord& record);

		bool CloseSessionWithUser(NetworkID id);

		bool FilterDeadSession(NetworkID id, NetworkSessionRecord& record) const;

		NetworkSession* FindSpecificSession(NetworkID id, uint64_t const& connectionid);

		NetworkSession* GetCurrentSession(NetworkID id);

		bool GetSessionState(NetworkID id, SessionState* outState);

		bool HasKnownConnection(NetworkID id);

		bool InitiateIncomingSession(NetworkID id, uint64_t const& connectionId, std::unique_ptr<webrtc::SessionDescriptionInterface> iface, SignalingChannelId sig_channel_id);
		NetworkSession* InitiateOutgoingSession(NetworkID id);
		bool IsPacketAvailable(NetworkID id, uint32_t* mcbMessage);

		void OnRemoteMessageReceived(NetworkID id, void const* pdata, size_t size);

		bool OpenSessionWithUser(NetworkID id);

		void PeriodicDeadSessionCleaup();

		void ProcessSignal(
			NetworkID remoteId,
			ConnectError const& signal,
			SignalingChannelId id
		);

		void ProcessSignal(
			NetworkID remoteId,
			ConnectResponse const& signal,
			SignalingChannelId id
		);

		void ProcessSignal(
			NetworkID remoteId,
			CandidateAdd const& signal,
			SignalingChannelId id
		);

		void ProcessSignal(
			NetworkID remoteId,
			ConnectRequest const& signal,
			SignalingChannelId id
		);

		void ProcessError(NetworkID networkIDRemote, ESessionError err);

		bool ReadPacket(NetworkID remoteId, void* pubDest, unsigned int cbDest, uint32_t* pcbMessageSize);

		void RemoteMessageReceived(NetworkID id, void const* pdata, size_t size);

		int SendPacket(NetworkID id, const char* pbData, uint32_t cbData, ESendType send_type);
	public:
		std::map<NetworkID, std::vector<uint8_t>>		mPacketData;				//this + 0x0
		std::mutex										mSessionCreationMutex;		//this + 0x60
		std::map<NetworkID, NetworkSessionRecord>		mSessionList;				//this + 0xB0
		SimpleNetworkInterfaceImpl*						mSimpleNetworkInterface;	//this + 0xC0
	};
}

#endif