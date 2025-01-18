#pragma once

#include <variant>

#include <api/scoped_refptr.h>
#include <api/peer_connection_interface.h>

#include "MyStatsObserver.hpp"
#include "../network/ESendType.hpp"
#include "../network/NetworkID.hpp"
#include "SimpleNetworkInterfaceImpl.hpp"
#include "../network/NetworkSessionManager.hpp"

namespace NetherNet {
	class NetworkSession : public webrtc::PeerConnectionObserver {
	public:
		enum class ENegotiationState : int {
			None = 0,
			WaitingForResponse = 1,
			WaitingForAccept = 2,
			ICEProcessing = 3,
		};


		void AcceptSession();
		void ApplyConnectionFlags(webrtc::PeerConnectionInterface::RTCConfiguration* config, uint32_t flag);
		void CheckSendDeferredData(webrtc::DataChannelInterface* iface);
		void CheckUpdateStats();
		void Close();
		bool GetSessionState(SessionState* outState);

		void InitializeIncoming(
			NetworkID remoteId,
			uint64_t const& connectionId,
			std::unique_ptr<webrtc::SessionDescriptionInterface> pSessionDesc,
			SignalingChannelId  preference
		);
		void InitializeOutgoing(NetworkID remoteID);

		bool IsConnectionAlive() const;
		ESessionError IsDeadSession(std::chrono::seconds negotiationTimeout);
		bool IsInactiveSession();


		void OnConnectionChange(webrtc::PeerConnectionInterface::PeerConnectionState new_state) override;
		void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> iface) override;
		void OnDataChannelMessage(webrtc::DataChannelInterface* iface, webrtc::DataBuffer const& data) ;
		void OnDataChannelStateChange(webrtc::DataChannelInterface* iface);
		void OnIceCandidate(webrtc::IceCandidateInterface const* iface) override;
		void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) override;
		void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) override;
		void OnStatsRequestComplete(std::vector<webrtc::StatsReport const*> const& reports);;
		void ProcessError(ESessionError err);
		void ProcessIceCandidates();

		void SendPacket(const char* pdata, uint32_t cbdata, ::NetherNet::ESendType send_type);
		void SendToSignalingChannel(std::variant<ConnectRequest, ConnectResponse, ConnectError, CandidateAdd> const& sigvar);
		void TrySendSessionResponse();


		void UpdateDataChannelStates();
		void UpdateSessionActivity();
	private:
		//Exact type pending
		ENegotiationState					mConnNegotiationState;	 //this + 0x08
		std::chrono::seconds				mNegotiationStart;		 //this + 0x10
		NetworkID							mConnectionId;			 //this + 0x20
		NetworkID							mRemoteID;				 //this + 0x28
		NetworkSessionManager*				mNetworkSessionMgr;		 //this + 0x50
		webrtc::PeerConnectionInterface::IceConnectionState	 mIceConnectionState;	//this + 0x58
		webrtc::PeerConnectionInterface::PeerConnectionState mPeerConnectionState;	//this + 0x5C
		uint32_t							mConnectionStat;
		MyStatsObserver						mStatsObserver;			 //this + 0x60
		rtc::scoped_refptr<webrtc::DataChannelInterface> mUnreliableChannelInterface;	 //this + 0xD8
		std::vector<rtc::CopyOnWriteBuffer>	mUnreliablePackets;		 //this + 0xE0
		webrtc::DataChannelInterface::DataState mUnreliableChannelState; //this + 0xF8
		rtc::scoped_refptr<webrtc::DataChannelInterface>		mReliableChannelInterface;	//this + 0x100
		std::vector<rtc::CopyOnWriteBuffer>	mReliablePackets;				//this + 0x108
		webrtc::DataChannelInterface::DataState	mReliableChannelState;
		uint32_t								mConnectionFlag;		 //this + 0x208
		/* 
		Connection flag format: 
		Bits:
		6 -> relay
		5 -> prflx
		4 -> stun
		3 -> local
		*/
	};
}