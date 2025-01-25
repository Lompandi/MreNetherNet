#pragma once

#include <variant>

#include <api/scoped_refptr.h>
#include <api/peer_connection_interface.h>

#include "../utils/Utils.hpp"
#include "MyStatsObserver.hpp"
#include "../network/ESendType.hpp"
#include "../network/NetworkID.hpp"
#include "../network/NetworkSessionManager.hpp"

namespace NetherNet {
	struct SessionState {
		bool   ConnectionActive;
		bool   Connecting;
		bool   HasSessionError;
		bool   UsingRelay;
		size_t CurrentRTT;
		size_t BytesSendBuffer;
		size_t TotalBytesSent;
		size_t TotalBytesSentPerSecond;
		size_t TotalBytesReceived;
		size_t TotalPacketsSent;
		size_t TotalPacketsLost;
		webrtc::PeerConnectionInterface::IceConnectionState   IceConnectionState;
	};

	class NetworkSession : public webrtc::PeerConnectionObserver {
		friend class NetworkSessionManager;
	public:
		enum class ENegotiationState : int {
			None = 0,
			WaitingForResponse = 1,
			WaitingForAccept = 2,
			ICEProcessing = 3,
		};

		NetworkSession(NetworkSessionManager* mgr, std::optional<SignalingChannelId> channel_id) :
			webrtc::PeerConnectionObserver() {};

		void AcceptSession();
		
		void ApplyConnectionFlags(webrtc::PeerConnectionInterface::RTCConfiguration* config, uint32_t flag);
		
		void CheckSendDeferredData();
		
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
		
		bool IsDeadSession(std::chrono::seconds negotiationTimeout);
		
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

		void ProcessSignal(CandidateAdd const& signal);
		
		void ProcessSignal(ConnectResponse const& signal);

		void SendPacket(const char* pdata, uint32_t cbdata, ::NetherNet::ESendType send_type);
		
		void SendToSignalingChannel(std::variant<ConnectRequest, ConnectResponse, ConnectError, CandidateAdd> const& sigvar);
		
		void TrySendSessionResponse();

		void UpdateDataChannelStates();
		
		void UpdateSessionActivity();
	private:
		ENegotiationState						mConnNegotiationState;					//this + 0x08
		Utils::TimePoint						mNegotiationStateTimer;					//this + 0x10
		bool									mIsOutGoingSession;
		bool									mHasSentAcceptSession;					//this + 0x1A
		bool									mStatsRequestInProgress;				//this + 0x1B
		NetworkID								mConnectionId;							//this + 0x20
		NetworkID								mRemoteID;								//this + 0x28
		rtc::scoped_refptr<webrtc::PeerConnectionInterface> mpPeerConnection;			//this + 0x30
		std::vector<std::unique_ptr<webrtc::IceCandidateInterface>> mDeferredIceCandidates; //this + 0x38
		NetworkSessionManager*					mNetworkSessionMgr;						//this + 0x50
		webrtc::PeerConnectionInterface::IceConnectionState	 mIceConnectionState;		//this + 0x58
		webrtc::PeerConnectionInterface::PeerConnectionState mPeerConnectionState;		//this + 0x5C
		uint32_t								mConnectionStat;
		MyStatsObserver							mStatsObserver;							//this + 0x60
		rtc::scoped_refptr<webrtc::DataChannelInterface> mUnreliableChannelInterface;	//this + 0xD8
		std::vector<rtc::CopyOnWriteBuffer>		mUnreliablePackets;						//this + 0xE0
		webrtc::DataChannelInterface::DataState mUnreliableChannelState;				//this + 0xF8
		rtc::scoped_refptr<webrtc::DataChannelInterface>		mReliableChannelInterface;	//this + 0x100
		std::vector<rtc::CopyOnWriteBuffer>		mReliablePackets;						//this + 0x108
		rtc::SocketAddress						mLocalSocketAddress;					//this + 0x100 (lin)
		rtc::SocketAddress						mRemoteSocketAddress;					//this + 0x148 (lin)
		bool									mIsUsingRelay;							//this + 0x1C8
		size_t									mCurrentRTT;							//this + 0x1D0 
		size_t									mTotalBytesSent;						//this + 0x1E0
		size_t									mTotalBytesReceived;					//this + 0x1F0 
		size_t									mTotalPacketLost;						//this + 0x1F8	
		bool									mIsDeleting;							//this + 0x1E8 (lin)
		webrtc::DataChannelInterface::DataState	mReliableChannelState;
		Utils::TimePoint						mLastStatsUpdate;						//this + 0x200
		uint32_t								mConnectionFlag;						//this + 0x208
		std::atomic<std::chrono::steady_clock::time_point> mLastSessionActivity;		//this + 0x210
		std::optional<SignalingChannelId>		mSignalingPreference;					//this + 0x228
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