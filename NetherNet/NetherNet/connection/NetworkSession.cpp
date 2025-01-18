
#include <utility>

#include <rtc_base/buffer.h>
#include <rtc_base/copy_on_write_buffer.h>

#include "../HNetherNet.hpp"
#include "NetworkSession.hpp"
#include "MyDataChannelObserver.hpp"
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
				mNetworkSessionMgr->mSimpleNetworkInterface->mReceiverId
			);
		}
	}

	void NetworkSession::TrySendSessionResponse() {
		UpdateSessionActivity();

		std::string str_connection_id = std::to_string(mConnectionId);
		::NetherNet::NetherNetTransport_LogMessage(
			4,
			"[%llu] Queueing connection response : [RemoteID: %llu] [ConnectionID: %s]",
			mNetworkSessionMgr->mSimpleNetworkInterface->mReceiverId,
			mRemoteID,
			mConnectionId
		);

		auto signal_thread = getSignalThread();
		auto response_thread = signal_thread->LoadRtcThread();

		/*response_thread->PostTask(
			//TODO
		);*/
	}

	void NetworkSession::ApplyConnectionFlags(
		webrtc::PeerConnectionInterface::RTCConfiguration* config,
		uint32_t flag) {
		if ((flag & 1) != 0)
			config->type = webrtc::PeerConnectionInterface::IceTransportsType::kRelay;
		if ((flag & 2) != 0)
			config->candidate_network_policy = webrtc::PeerConnectionInterface::CandidateNetworkPolicy::kCandidateNetworkPolicyLowCost;
		mConnectionFlag = flag;
	}

	void NetworkSession::CheckSendDeferredData(webrtc::DataChannelInterface* iface) {
		if (mReliableChannelInterface != 0) {
			std::string connectionId = std::to_string(mConnectionId);
			::NetherNet::NetherNetTransport_LogMessage(
				4,
				"[%llu] Sending %d packets of deferred reliable data. [RemoteID: %llu] [ConnectionID: %s]",
				mNetworkSessionMgr->mSimpleNetworkInterface->mReceiverId,
				mReliablePackets.size(),
				mRemoteID,
				connectionId.data()
			);

			for (const auto& pkt : mReliablePackets) {
				webrtc::DataBuffer buffer(pkt, true);
				mReliableChannelInterface->Send(buffer);
			}
			mReliablePackets.clear();
		}
		if (mUnreliableChannelInterface != 0) {
			std::string connectionId = std::to_string(mConnectionId);
			NetherNet::NetherNetTransport_LogMessage(
				4,
				"[%llu] Sending %d packets of deferred unreliable data. [RemoteID: %llu] [ConnectionID: %s]",
				mNetworkSessionMgr->mSimpleNetworkInterface->mReceiverId,
				mUnreliablePackets.size(),
				mRemoteID,
				connectionId.data());
			for (const auto& pkt : mUnreliablePackets) {
				webrtc::DataBuffer buffer(pkt, true);
				mReliableChannelInterface->Send(buffer);
			}
		}
		mUnreliablePackets.clear();
	}

	void NetworkSession::CheckUpdateStats() {
		//TODO
	}

	bool NetworkSession::GetSessionState(SessionState* outState) {
		//TODO
	}

	void NetworkSession::InitializeIncoming(
		NetworkID remoteId,
		uint64_t const& connectionId,
		std::unique_ptr<webrtc::SessionDescriptionInterface> pSessionDesc,
		SignalingChannelId  preference
	) {
		//TODO
	}

	bool NetworkSession::IsConnectionAlive() const {
		return ((mConnectionStat - 4) & 0xFFFFFFFD) != 0;
	}

	ESessionError NetworkSession::IsDeadSession(std::chrono::seconds negotiationTimeout) {
		if (IsConnectionAlive()) {
			if (mConnNegotiationState == ENegotiationState::ICEProcessing)
				return ESessionError::ESessionErrorNone;

			//...TODO
		}
	}

	void NetworkSession::UpdateDataChannelStates() {
		if (mReliableChannelInterface != 0)
			mReliableChannelState = mReliableChannelInterface->state();
		else
			mReliableChannelState = webrtc::DataChannelInterface::DataState::kClosed;

		if (mUnreliableChannelInterface != 0)
			mUnreliableChannelState = mUnreliableChannelInterface->state(); 
		else
			mUnreliableChannelState = webrtc::DataChannelInterface::DataState::kClosed;
		
		if (mReliableChannelState == webrtc::DataChannelInterface::DataState::kOpen
			&& mUnreliableChannelState == webrtc::DataChannelInterface::DataState::kOpen)
			mNetworkSessionMgr->mSimpleNetworkInterface->NotifyOnSessionOpen(mRemoteID);
	}

	void NetworkSession::OnIceCandidate(webrtc::IceCandidateInterface const* iface) {
		cricket::Candidate candidate = iface->candidate(); //invoke vindex 4
		bool need_create = true;

		if ((mConnectionFlag & 4) != 0) {
			if (candidate.type() ==  webrtc::IceCandidateType::kHost)
				need_create = false;
		}
		if ((mConnectionFlag & 8) != 0) {
			if (candidate.type() == webrtc::IceCandidateType::kSrflx)
				need_create = false;
		}
		if ((mConnectionFlag & 16) != 0) {
			if (candidate.type() == webrtc::IceCandidateType::kPrflx)
				need_create = false;
		}
		if ((mConnectionFlag & 32) == 0 || 
			candidate.type() != webrtc::IceCandidateType::kRelay){
			if (need_create) {
				auto candidate_create = CandidateAdd::TryCreate(mConnectionId, *iface);
				if (candidate_create) {
					SendToSignalingChannel(candidate_create.value());
				}
			}
		}
	}

	void  NetworkSession::OnConnectionChange(webrtc::PeerConnectionInterface::PeerConnectionState new_state) {
		mPeerConnectionState = new_state;
		std::string connId = std::to_string(mConnectionId);

		NetherNet::NetherNetTransport_LogMessage(
			4,
			"[%llu] Peer Connection State Change: [State: %d] [RemoteID: %llu] [ConnectionId: %s]",
			mNetworkSessionMgr->mSimpleNetworkInterface->mReceiverId,
			new_state,
			mRemoteID,
			connId.data());
	}

	void NetworkSession::OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> iface) {
		auto label = iface->label();
		bool is_reliable = label.size() == 19 && label == "ReliableDataChannel";

		std::unique_ptr<MyDataChannelObserver> observer;
		if (is_reliable) {
			mReliableChannelInterface = iface;
			observer = std::make_unique<MyDataChannelObserver>();
		}
		else {
			mUnreliableChannelInterface = iface;
			observer = std::make_unique<MyDataChannelObserver>();
		}

		iface->RegisterObserver(std::move(observer.get()));
		UpdateDataChannelStates();
		CheckSendDeferredData(iface.get());
	}

	void NetworkSession::OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) {
		mIceConnectionState = new_state;
		std::string connId = std::to_string(mConnectionId);
		NetherNet::NetherNetTransport_LogMessage(
			4,
			"[%llu] Ice Connection State Change: [State: %d] [RemoteID: %llu] [ConnectionId: %s]",
			mNetworkSessionMgr->mSimpleNetworkInterface->mReceiverId,
			new_state,
			mRemoteID,
			connId.data()
		);

		if (new_state - 1 <= 1) {
			CheckUpdateStats();
		}
	}

	void NetworkSession::OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) {
		if ((new_state - 1) <= 1)
			ProcessIceCandidates();
	}

	void NetworkSession::OnStatsRequestComplete(std::vector<webrtc::StatsReport const*> const& reports) {
		//TODO
	}

	void NetworkSession::ProcessError(ESessionError err) {
		mNetworkSessionMgr->mSimpleNetworkInterface->NotifyOnSessionClose(mRemoteID, err);
	}

	void NetworkSession::ProcessIceCandidates() {
		//TODO
	}

	void NetworkSession::SendToSignalingChannel(std::variant<ConnectRequest, ConnectResponse, ConnectError, CandidateAdd> const& sigvar) {

	}
}