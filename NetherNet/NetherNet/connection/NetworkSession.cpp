
#include <utility>

#include <rtc_base/buffer.h>
#include <rtc_base/copy_on_write_buffer.h>

#include "../HNetherNet.hpp"
#include "NetworkSession.hpp"
#include "MyDataChannelObserver.hpp"
#include "MyCreateSessionDescriptionObserver.hpp"
#include "MySetSessionDescriptionObserver.hpp"
#include "../threadding/RtcThreadManager.hpp"
#include "../network/AsyncResolverFactory.hpp"
#include "../connection/SimpleNetworkInterfaceImpl.hpp"

namespace NetherNet {
	void NetworkSession::UpdateSessionActivity() {
		mLastSessionActivity = std::chrono::steady_clock::now();
		return;
	}

	void NetworkSession::SendPacket(
		const char* pdata,
		uint32_t	cbdata,
		::NetherNet::ESendType send_type
	) {
		UpdateSessionActivity();
		auto signalThread = getSignalThread();

		rtc::CopyOnWriteBuffer mSendBuffer(pdata, cbdata);
		
		auto thread = signalThread->LoadRtcThread();
		if (thread) {
			rtc::CopyOnWriteBuffer send_buf(mSendBuffer);
			
			auto rtc_thread = signalThread->LoadRtcThread();
			if (rtc_thread) {
				rtc_thread->PostTask([&]() {
					if (mpPeerConnection.get() != nullptr) {
						if (send_type == ESendType::eSendTypeReliable) {
							if (!mReliableChannelInterface
								|| mReliableChannelState != webrtc::DataChannelInterface::DataState::kOpen) {
								mReliablePackets.push_back(send_buf);
								return;
							}
							webrtc::DataBuffer send_buf_m(send_buf, true);
							mReliableChannelInterface->Send(send_buf_m);
							return;
						}
						if (mUnreliableChannelInterface.get() != nullptr
							&& mUnreliableChannelState == webrtc::DataChannelInterface::DataState::kOpen) {
							webrtc::DataBuffer send_buf_m(send_buf, true);
							mUnreliableChannelInterface->Send(send_buf_m);
							return;
						}
						mUnreliablePackets.push_back(send_buf);
						return;
					}
				});
			}
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
				LogSeverity::Error,
				"[%llu] Error: Trying to accept when no accept pending.",
				mNetworkSessionMgr->mSimpleNetworkInterface->mReceiverId
			);
		}
	}

	void NetworkSession::TrySendSessionResponse() {
		UpdateSessionActivity();

		std::string str_connection_id = std::to_string(mConnectionId);
		::NetherNet::NetherNetTransport_LogMessage(
			LogSeverity::Information,
			"[%llu] Queueing connection response : [RemoteID: %llu] [ConnectionID: %s]",
			mNetworkSessionMgr->mSimpleNetworkInterface->mReceiverId,
			mRemoteID,
			mConnectionId
		);

		auto signal_thread = getSignalThread();
		auto response_thread = signal_thread->LoadRtcThread();

		if (response_thread) {
			response_thread->PostTask([&]() {
				if (mConnNegotiationState != ENegotiationState::WaitingForAccept
					&& !mHasSentAcceptSession) {

					auto session_observer = MyCreateSessionDescriptionObserver::Create(
					[this](webrtc::SessionDescriptionInterface* desc) {
						auto conn_id_str = std::to_string(mConnectionId);
						NetherNet::NetherNetTransport_LogMessage(NetherNet::LogSeverity::Information, "CreateAnswer called back for %s", conn_id_str.c_str());
						//TODO
					},
					[this](webrtc::RTCError const& err) {
						auto conn_id_str = std::to_string(mConnectionId);
						auto err_msg = err.message();

						NetherNet::NetherNetTransport_LogMessage(
							NetherNet::LogSeverity::Error,
							"CreateAnswer failed for %s with error \"%s\"",
							conn_id_str.c_str(),
							err_msg);

						mNetworkSessionMgr->mSimpleNetworkInterface->NotifyOnSessionClose(
							mRemoteID,
							ESessionError::ESessionErrorFailedToCreateAnswer
						);
					});
				}
			});
		}
	}

	void NetworkSession::ApplyConnectionFlags(
		webrtc::PeerConnectionInterface::RTCConfiguration* config,
		uint32_t flag) {
		if ((flag & 1) != 0) {
			config->type = webrtc::PeerConnectionInterface::IceTransportsType::kRelay;
		}
		if ((flag & 2) != 0)
			config->tcp_candidate_policy = webrtc::PeerConnectionInterface::TcpCandidatePolicy::kTcpCandidatePolicyDisabled;
		mConnectionFlag = flag;
	}

	void NetworkSession::CheckSendDeferredData() {
		if (mReliableChannelInterface != 0) {
			std::string connectionId = std::to_string(mConnectionId);
			::NetherNet::NetherNetTransport_LogMessage(
				LogSeverity::Information,
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
				LogSeverity::Information,
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
		auto last_stats_update = mLastStatsUpdate;
		if (std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now()
			- last_stats_update
		) >= std::chrono::milliseconds(2000)
			&& !mStatsRequestInProgress) {
			auto signal_thread = getSignalThread();
			auto rtc_thread = signal_thread->LoadRtcThread();
			if (rtc_thread) {
				rtc_thread->PostTask([&]() {
					//TODO
				});
			}
			mStatsRequestInProgress = true;
			mLastStatsUpdate = std::chrono::steady_clock::now();
		}
	}

	bool NetworkSession::GetSessionState(SessionState* outState) {
		if (outState == nullptr)
			return false;

		outState->TotalPacketsSent = 0;
		outState->TotalBytesSentPerSecond = 0;
		outState->BytesSendBuffer = 0;
		outState->ConnectionActive = 0;
		outState->IceConnectionState = webrtc::PeerConnectionInterface::kIceConnectionNew;

		auto current_state = mIceConnectionState;
		if ((uint32_t)current_state <= webrtc::PeerConnectionInterface::kIceConnectionChecking)
			outState->Connecting = true;
		if (current_state == webrtc::PeerConnectionInterface::kIceConnectionCompleted
			|| current_state == webrtc::PeerConnectionInterface::kIceConnectionConnected
			|| current_state == webrtc::PeerConnectionInterface::kIceConnectionDisconnected) {
			outState->ConnectionActive =
				(mUnreliableChannelState == webrtc::DataChannelInterface::kOpen
				&& mReliableChannelState == webrtc::DataChannelInterface::kOpen);
		}
		if (((current_state - 4) & 0xFFFFFFFD) == 0)
			outState->HasSessionError == true;
		outState->UsingRelay = mIsUsingRelay;
		outState->CurrentRTT = mCurrentRTT;
		outState->TotalBytesSent = mTotalBytesSent;
		outState->TotalBytesReceived = mTotalBytesReceived;
		outState->TotalPacketsLost = mTotalPacketLost;
		outState->IceConnectionState = current_state;
		CheckUpdateStats();
		return true;
	}

	void NetworkSession::InitializeIncoming(
		NetworkID remoteId,
		uint64_t const& connectionId,
		std::unique_ptr<webrtc::SessionDescriptionInterface> pSessionDesc,
		SignalingChannelId  preference
	) {
		UpdateSessionActivity();
		mIsOutGoingSession = false;
		mConnectionId = connectionId;
		mRemoteID = remoteId;
		mSignalingPreference = preference;

		webrtc::PeerConnectionInterface::RTCConfiguration config(
			mNetworkSessionMgr->mSimpleNetworkInterface->mRtcConfig
		);

		auto connection_flag = mNetworkSessionMgr->mSimpleNetworkInterface
			->GetConnectionFlags(remoteId);

		ApplyConnectionFlags(&config, connection_flag);
		webrtc::PeerConnectionDependencies conn_deps(this);

		auto workerThread = getWorkerThread();
		auto rtcThread = workerThread->GetRtcThread();

		//TODO:
		auto resolver = std::make_unique<AsyncResolverFactory>();
		//

		conn_deps.async_dns_resolver_factory
			= std::move(resolver);

		auto peer_conn_factory =
			mNetworkSessionMgr->mSimpleNetworkInterface->mpPeerConnectionFactory.get();

		auto peer_conn_create =
			peer_conn_factory->CreatePeerConnectionOrError(config, std::move(conn_deps));

		if (!peer_conn_create.ok()) {
			auto err_msg = peer_conn_create.error().message();
			NetherNet::NetherNetTransport_LogMessage(
				NetherNet::LogSeverity::Error,
				"Failed to create incoming peer connection: %s",
				err_msg);

			mNetworkSessionMgr->mSimpleNetworkInterface
				->NotifyOnSessionClose(mRemoteID, ESessionError::ESessionErrorFailedToCreatePeerConnection);
			return;
		}
		
		mpPeerConnection = peer_conn_create.MoveValue();
		mConnNegotiationState = ENegotiationState::WaitingForAccept;
		mNegotiationStateTimer = std::chrono::steady_clock::now();

		auto conn_id_str = std::to_string(mConnectionId);
		NetherNet::NetherNetTransport_LogMessage(
			NetherNet::LogSeverity::Information,
			"[%llu] Received incoming connection request: [RemoteID: %llu] [ConnectionId: %s]",
			mNetworkSessionMgr->mSimpleNetworkInterface->mReceiverId,
			mRemoteID,
			conn_id_str.c_str());

		rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_conn_state 
			= std::move(mpPeerConnection);

		auto sess_desc_observer = MySetSessionDescriptionObserver::Create(
			[this]() {
				if (!mIsDeleting) {
					TrySendSessionResponse();
					ProcessIceCandidates();
				}
			},
			
			[this](webrtc::RTCError const& err) {
				mNetworkSessionMgr->mSimpleNetworkInterface
					->NotifyOnSessionClose(mRemoteID, ESessionError::ESessionErrorFailedToSetRemoteDescription);
			});
		rtc::scoped_refptr<MySetSessionDescriptionObserver> uniq_desc_observer(
			sess_desc_observer
		);

		peer_conn_state->SetRemoteDescription(
			std::move(pSessionDesc), std::move(uniq_desc_observer)
		);
	}

	bool NetworkSession::IsConnectionAlive() const {
		return ((mConnectionStat - 4) & 0xFFFFFFFD) != 0;
	}

	/*
	 Check if the session is beging inactive in the past minute
	*/
	bool NetworkSession::IsInactiveSession() {
		auto& last_sess_activity = mLastSessionActivity;
		return std::chrono::duration_cast<std::chrono::minutes>(std::chrono::steady_clock::now() - last_sess_activity.load()) > std::chrono::minutes(1);
	}

	bool NetworkSession::IsDeadSession(std::chrono::seconds negotiationTimeout) {
		ESessionError notify_state;
		if (IsConnectionAlive()) {
			if (mConnNegotiationState == ENegotiationState::ICEProcessing)
				return false;

			auto negotiation_time
				= mNegotiationStateTimer;

			if (std::chrono::duration_cast<std::chrono::milliseconds>(negotiationTimeout)
				>= std::chrono::duration_cast<std::chrono::milliseconds>(
					std::chrono::steady_clock::now()
					- negotiation_time
				))
				return false;

			/*
			here, `notify_state` will be either:
			ESessionErrorNegotiationTimeoutWaitingForResponse (14),
			ESessionErrorNegotiationTimeoutWaitingForAccept (15),
			depends on the `waiting-for` state
			*/
			notify_state = (ESessionError)((mConnNegotiationState == ENegotiationState::WaitingForResponse) ^ 0xF);
		}
		else {
			notify_state = ESessionError::ESessionErrorICE;
		}
		mNetworkSessionMgr->mSimpleNetworkInterface
			->NotifyOnSessionClose(mRemoteID, (ESessionError)notify_state);
		return true;
	}

	void NetworkSession::OnDataChannelStateChange(webrtc::DataChannelInterface* iface) {
		UpdateDataChannelStates();
		CheckSendDeferredData();
	}

	void NetworkSession::Close() {
		if (mpPeerConnection) {
			auto signal_thread = getSignalThread();
			auto rtc_thread = signal_thread->LoadRtcThread();
			if (rtc_thread) {
				rtc_thread->PostTask([&]() {
					mpPeerConnection->Close();
				});
			}
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
			LogSeverity::Information,
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
		CheckSendDeferredData();
	}

	void NetworkSession::OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) {
		mIceConnectionState = new_state;
		std::string connId = std::to_string(mConnectionId);
		NetherNet::NetherNetTransport_LogMessage(
			LogSeverity::Information,
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
		if (!reports.empty()) {
			for (const auto& report : reports) {
				if (report->id()->type() == webrtc::StatsReport::StatsType::kStatsReportTypeCandidatePair)
					continue;

				if (report->id()->type() == webrtc::StatsReport::kStatsReportTypeDataChannel) {
					auto bytesSent = report->FindValue(webrtc::StatsReport::kStatsValueNameBytesSent);
					if (bytesSent) {
						auto bytes_sent_str = bytesSent->ToString();
						//TODO
					}
				}
			}
		}
	}

	void NetworkSession::ProcessError(ESessionError err) {
		mNetworkSessionMgr->mSimpleNetworkInterface->NotifyOnSessionClose(mRemoteID, err);
	}

	//TODO: figure this out
	void NetworkSession::ProcessIceCandidates() {
		auto& network_interface = this->mNetworkSessionMgr->mSimpleNetworkInterface;
		auto remote_id = this->mRemoteID;
		auto connection_id = this->mConnectionId;

		for (auto it = mDeferredIceCandidates.begin(); it != mDeferredIceCandidates.end();)
		{
			auto& ice_candidate = *it;
			bool add_result = mpPeerConnection->AddIceCandidate(ice_candidate.get());
			std::string connection_id_str = std::to_string(connection_id);

			if (add_result) {
				NetherNet::NetherNetTransport_LogMessage(
					NetherNet::LogSeverity::Information,
					"[%llu] Processed deferred ice candidate: [RemoteID: %llu] [ConnectionId: %s]",
					network_interface->mReceiverId,
					remote_id,
					connection_id_str.c_str()
				);

				it = mDeferredIceCandidates.erase(it);
			}
			else {
				NetherNet::NetherNetTransport_LogMessage(
					NetherNet::LogSeverity::Information,
					"[%llu] Unable to process deferred ice candidate: [RemoteID: %llu] [ConnectionId: %s]",
					network_interface->mReceiverId,
					remote_id,
					connection_id_str.c_str()
				);
				++it;
			}
		}
	}

	void NetworkSession::SendToSignalingChannel(std::variant<ConnectRequest, ConnectResponse, ConnectError, CandidateAdd> const& sigvar) {
		auto remoteId = mRemoteID;
		auto pNetworkInterface = mNetworkSessionMgr->mSimpleNetworkInterface;

		auto send_reult = mNetworkSessionMgr->mSimpleNetworkInterface
			->SendToSignalingChannel(remoteId, sigvar, mSignalingPreference,
				[&](std::error_code) {
					mNetworkSessionMgr->mSimpleNetworkInterface
						->NotifyOnSessionClose(remoteId, ESessionError::ESessionErrorSignalingFailedToSend);
				});

		if (send_reult != ESessionError::ESessionErrorNone) {
			mNetworkSessionMgr->mSimpleNetworkInterface
				->NotifyOnSessionClose(
					remoteId,
					send_reult
				);
		}
	}

	void NetworkSession::OnDataChannelMessage(webrtc::DataChannelInterface* iface, webrtc::DataBuffer const& data) {
		UpdateSessionActivity();
		mNetworkSessionMgr->OnRemoteMessageReceived(mRemoteID, data.data.data(), data.size());
	}

	void 
	NetworkSession::InitializeOutgoing(NetworkID remoteID) {
		UpdateSessionActivity();
		mIsOutGoingSession = true;
		mConnectionId = CreateID();
		mRemoteID = remoteID;
		mConnNegotiationState = ENegotiationState::WaitingForResponse;
		mNegotiationStateTimer = std::chrono::steady_clock::now();
		NetherNet::NetherNetTransport_LogMessage(
			NetherNet::LogSeverity::Information,
			"[%llu] Scheduling initialization of outgoing connection on Signal Thread",
			mNetworkSessionMgr->mSimpleNetworkInterface->mReceiverId);

		auto signal_thread = getSignalThread();
		auto rtc_thread = signal_thread->LoadRtcThread();
		if (rtc_thread) {
			rtc_thread->PostTask([&]() {
				webrtc::PeerConnectionInterface::RTCConfiguration rtc_config(mNetworkSessionMgr->mSimpleNetworkInterface->mRtcConfig);
				auto conn_flag 
					= mNetworkSessionMgr->mSimpleNetworkInterface->GetConnectionFlags(remoteID);
				ApplyConnectionFlags(&rtc_config, conn_flag);
				webrtc::PeerConnectionDependencies conn_deps(this);
				auto worker_thread = getWorkerThread();
				auto wrtc_thread = worker_thread->GetRtcThread();
				auto async_resolve_factory
					= std::make_unique<AsyncResolverFactory>(wrtc_thread);
				
				auto peer_create = mNetworkSessionMgr->mSimpleNetworkInterface->mpPeerConnectionFactory->
					CreatePeerConnectionOrError(rtc_config, std::move(conn_deps));

				if (!peer_create.ok()) {
					NetherNet::NetherNetTransport_LogMessage(
						NetherNet::LogSeverity::Error,
						"Failed to create outgoing peer connection: %s",
						peer_create.error().message());
					mNetworkSessionMgr->mSimpleNetworkInterface
						->NotifyOnSessionClose(
							mRemoteID,
							ESessionError::ESessionErrorFailedToCreatePeerConnection
						);
				}
				else {
					mpPeerConnection = peer_create.MoveValue();
				}

				std::string channel_id = "&ReliableDataChannel";
				webrtc::DataChannelInit init;
				init.reliable = 256;
				auto create_data_channel = mpPeerConnection->CreateDataChannelOrError(channel_id, &init);
				if (!create_data_channel.ok()) {
					NetherNet::NetherNetTransport_LogMessage(
						NetherNet::LogSeverity::Error,
						"Failed to create reliable data channel: %s",
						create_data_channel.error().message());
					mNetworkSessionMgr->mSimpleNetworkInterface
						->NotifyOnSessionClose(
							mRemoteID,
							ESessionError::ESessionErrorFailedToCreatePeerConnection
						);
				}
				else {
					auto channel_created = 
						mReliableChannelInterface ? mReliableChannelInterface : create_data_channel.MoveValue();
					MyDataChannelObserver observer(this, channel_created);
					channel_created->RegisterObserver(&observer);
				}

				init.reliable = false;
				channel_id = "*UnreliableDataChannel";
				create_data_channel = mpPeerConnection->CreateDataChannelOrError(channel_id, &init);

				if (!create_data_channel.ok()) {
					NetherNet::NetherNetTransport_LogMessage(
						NetherNet::LogSeverity::Error,
						"Failed to create unreliable data channel: %s",
						create_data_channel.error().message());
					mNetworkSessionMgr->mSimpleNetworkInterface
						->NotifyOnSessionClose(
							mRemoteID,
							ESessionError::ESessionErrorFailedToCreatePeerConnection
						);
				}
				else {
					auto channel_created
						= mUnreliableChannelInterface ? mReliableChannelInterface : create_data_channel.MoveValue();
					MyDataChannelObserver observer(this, channel_created);
					channel_created->RegisterObserver(&observer);
				}

				mReliableChannelState
					= mReliableChannelInterface ? mReliableChannelInterface->state() : webrtc::DataChannelInterface::DataState::kClosed;

				if (mUnreliableChannelInterface) {
					auto unreliable_state = mUnreliableChannelInterface->state();
					auto reliable_state = mReliableChannelState;

					mUnreliableChannelState = unreliable_state;
					if (unreliable_state == webrtc::DataChannelInterface::kOpen
						&& reliable_state == webrtc::DataChannelInterface::kOpen) {
						mNetworkSessionMgr->mSimpleNetworkInterface
							->NotifyOnSessionOpen(remoteID);
					}
				}
				else {
					mUnreliableChannelState = webrtc::DataChannelInterface::kClosed;
				}

				auto conn_id_str = std::to_string(mConnectionId);
				NetherNet::NetherNetTransport_LogMessage(
					NetherNet::LogSeverity::Information,
					"[%llu] Send connection request (main thread): [RemoteID: %llu] [ConnectionId: %s]",
					mNetworkSessionMgr->mSimpleNetworkInterface->mReceiverId,
					remoteID,
					conn_id_str.c_str());

				auto desc_observer = MyCreateSessionDescriptionObserver::Create(
					[&](webrtc::SessionDescriptionInterface* iface) {
						if (!mIsDeleting) {
							//TODO
						}

					},
					[&](webrtc::RTCError const&) {
						mNetworkSessionMgr->mSimpleNetworkInterface
							->NotifyOnSessionClose(remoteID, ESessionError::ESessionErrorFailedToCreateOffer);
					});

				//TODO
			});
		}
	}

	//TODO
	void 
	NetworkSession::ProcessSignal(CandidateAdd const& signal) {
		UpdateSessionActivity();
		auto sdp = signal.GetSdp();
		if (sdp) {
			auto& candidate = sdp->candidate();
			auto processed = true;
			if ((mConnectionFlag & 4) != 0) {
				//TODO	
			}
		}
	}

	void 
	NetworkSession::ProcessSignal(ConnectResponse const& signal) {
		UpdateSessionActivity();
		auto conn_id_str = std::to_string(mConnectionId);
		NetherNet::NetherNetTransport_LogMessage(
			NetherNet::LogSeverity::Information,
			"[%llu] Received connection response: [RemoteID: %llu] [ConnectionId: %s]",
			mNetworkSessionMgr->mSimpleNetworkInterface->mReceiverId,
			mRemoteID,
			conn_id_str.c_str());

		mConnNegotiationState = ENegotiationState::ICEProcessing;
		mNegotiationStateTimer = std::chrono::steady_clock::now();
		//auto sdp = signal.GetSdp();
		auto observer = MySetSessionDescriptionObserver::Create(
			[this]() {
				if (!mIsDeleting)
					ProcessIceCandidates();
			},
			[this](const webrtc::RTCError& err) {
				mNetworkSessionMgr->mSimpleNetworkInterface
					->NotifyOnSessionClose(mRemoteID, ESessionError::ESessionErrorFailedToSetRemoteDescription);
			}
		);

		//TODO
		//mpPeerConnection->SetRemoteDescription(&observer);

	}
}