

#include <json/json.h>
#include <rtc_base/openssl_key_pair.h>

#include "../HNetherNet.hpp"
#include "../threadding/LanThreadManager.hpp"
#include "SimpleNetworkInterfaceImpl.hpp"
#include "../network/SignalingChannelId.hpp"
#include "../network/packets/ESessionError.hpp"
#include "../network/signaling/ConnectResponse.hpp"
#include "NetworkSession.hpp"

namespace NetherNet {
	void SimpleNetworkInterfaceImpl::EnableSignalingOverLAN() {
		if (mDisableLANSignaling) {
			NetherNet::NetherNetTransport_LogMessage(LogSeverity::Information, "Enabling LAN for both signaling and transport");
			mDisableLANSignaling = false;
		}
	}

	void SimpleNetworkInterfaceImpl::DisableSignalingOverLAN() {
		if (!mDisableLANSignaling) {
			NetherNet::NetherNetTransport_LogMessage(LogSeverity::Information, "Disabling LAN for both signaling and transport");
			mDisableLANSignaling = true;
		}
	}

	void SimpleNetworkInterfaceImpl::AcceptSessionWithUser(NetworkID id) {
		std::lock_guard<std::mutex> lock(mApiLevelMutex);
		mNetworkSessionMgr->AcceptSessionWithUser(id);
	}

	void SimpleNetworkInterfaceImpl::HandleDiscoveryPacketOnSignalThread(
		rtc::SocketAddress& addr,
		DiscoveryMessagePacket& packet
	) {
		if (packet.RecipientId() == mReceiverId) {
			auto message_data = packet.MessageData();

			std::string packet_data;
			packet_data.assign(message_data.data());

			std::string host_addr = addr.ToString();
			::NetherNet::NetherNetTransport_LogMessage(
				LogSeverity::Information,
				"Received discovery message from \"%s\" with network id \"%llu\" and message \"%s\"",
				host_addr.data(),
				packet.SenderId(),
				message_data.data()
			);

			ReceiveFromLanSignalingChannel(packet.SenderId(), packet_data);
		}
	}

	void SimpleNetworkInterfaceImpl::ClearPacketData(NetworkID id) {
		mNetworkSessionMgr->ClearPacketData(id);
	}

	uint32_t 
	SimpleNetworkInterfaceImpl::GetConnectionFlags(NetworkID remoteId) {
		if (!mCallBack)
			return 0;

		uint32_t flag = 0;
		mCallBack->OnSessionGetConnectionFlags(remoteId, &flag);
		return flag;
	}

	bool 
	SimpleNetworkInterfaceImpl::GetSessionState(NetworkID remoteId, SessionState* pConnState) {
		std::lock_guard lock(mApiLevelMutex);
		if (mReceiverId == remoteId)
			return false;
		return mNetworkSessionMgr->GetSessionState(remoteId, pConnState);
	}

	void SimpleNetworkInterfaceImpl::HandleDiscoveryPacketOnSignalThread(
		rtc::SocketAddress& addr,
		DiscoveryResponsePacket& packet
	) {
		if (mCallBack && packet.SenderId() != mReceiverId) {
			NetworkID senderId = packet.SenderId();
			auto sockAddr = addr.ToString();
			NetherNet::NetherNetTransport_LogMessage(
				LogSeverity::Verbose,
				"Recieved discovery response from \"%s\" with network id \"%llu\"",
				sockAddr,
				senderId);

			auto application_data = packet.ApplicationData();
			mCallBack->OnBroadcastResponseReceived(senderId, application_data.data(), packet.PacketLength());
		}
	}

	void SimpleNetworkInterfaceImpl::ReceiveFromLanSignalingChannel(NetworkID remoteId, const std::string& message, bool a3, SignalingChannelId channelId) {
		::NetherNet::NetherNetTransport_LogMessage(
			LogSeverity::Information,
			"received signal from [%llu]: %s",
			remoteId,
			message.data()
		);

		ESessionError err = ESessionError::ESessionErrorNone;
		std::string message_data;

		if (a3) {
			Json::Value json_message;
			Json::Reader message_reader;

			auto success = message_reader.parse(message, json_message);
			if (!success) {
				err = ESessionError::ESessionErrorSignalingParsingFailure;
				ProcessSessionError(remoteId, err);
				return;
			}

			Json::Value msgCode = json_message["Code"];
			auto message_code = msgCode.asInt();
			if (message_code) {
				if (!(message_code - 1)) {
					err = ESessionError::ESessionErrorDestinationNotLoggedIn;
					ProcessSessionError(remoteId, err);
					return;
				}

				auto message_code_sub2 = message_code - 2;
				if (!message_code_sub2) {
					Json::Value msgMessage = json_message["Message"];
					message_data = msgMessage.asString();
					/* TODO: Some more shit processing logic */
				}

				message_code_sub2 -= 1;
				if (!message_code_sub2) {
					err = ESessionError::ESessionErrorSignalingTurnAuthFailed;
					ProcessSessionError(remoteId, err);
					return;
				}
				if (message_code_sub2 == 1) {
					err = ESessionError::ESessionErrorSignalingFallbackToBestEffortDelivery;
					ProcessSessionError(remoteId, err);
					return;
				}
			}
			else {
				err = ESessionError::ESessionErrorSignalingUnknownError;
				ProcessSessionError(remoteId, err);
				return;
			}
		}

		auto result = TryParseSignalingMessage(message_data);
		if (result.has_value()) {
			auto& var = result.value();
			if (std::holds_alternative<ConnectError>(var)) {
				mNetworkSessionMgr->ProcessSignal(remoteId, std::get<ConnectError>(var), channelId);
			}
			else if (std::holds_alternative<ConnectResponse>(var)) {
				mNetworkSessionMgr->ProcessSignal(remoteId, std::get<ConnectResponse>(var), channelId);
			}
			else if (std::holds_alternative<CandidateAdd>(var)) {
				mNetworkSessionMgr->ProcessSignal(remoteId, std::get<CandidateAdd>(var), channelId);
			}
			else if (std::holds_alternative<ConnectRequest>(var)) {
				mNetworkSessionMgr->ProcessSignal(remoteId, std::get<ConnectRequest>(var), channelId);
			}
		}
		else if (message.find("Username") != std::string::npos) {
			ProcessTurnConfig(message);
		}
	}

	void SimpleNetworkInterfaceImpl::HandleDiscoveryPacketOnSignalThread(
		rtc::SocketAddress const& addr,
		DiscoveryRequestPacket const& packet
	) {
		if (mCallBack && packet.SenderId() != mReceiverId) {
			NetworkID senderId = packet.SenderId();
			auto sockAddr = addr.ToString();

			NetherNet::NetherNetTransport_LogMessage(
				LogSeverity::Verbose,
				"Received discovery request from \"%s\" with network id \"%llu\"",
				sockAddr,
				senderId);

			int outMessageSizes[] = { 1148 };
			char outData[1152] = {};

			if (
				mCallBack->OnBroadcastDiscoveryRequestReceivedGetResponse(outData, outMessageSizes)
				) {
				std::string_view response_data(outData, outMessageSizes[0]);
				DiscoveryResponsePacket disc_resp_pkt(mReceiverId, response_data);
				auto send_thread = &getLanThread();
				send_thread->SendLanBroadcastResponse(addr, disc_resp_pkt);
			}
		}
	}

	void SimpleNetworkInterfaceImpl::InitializeConfiguration(
		const NetherNetTransportServerConfiguration* config,
		webrtc::PeerConnectionInterface::RTCConfiguration* outRtcConfig
	) {
		outRtcConfig->rtcp_mux_policy = webrtc::PeerConnectionInterface::kRtcpMuxPolicyRequire;
		outRtcConfig->tcp_candidate_policy = webrtc::PeerConnectionInterface::kTcpCandidatePolicyDisabled;

		outRtcConfig->servers.clear();
		if (config->StunRelayServerCount > 0) {
			for (int i = 0; i < config->StunRelayServerCount; i++) {
				webrtc::PeerConnectionInterface::IceServer ice_server;
				auto stun_server = config->StunRelayServers[i];

				ice_server.uri = stun_server.Uri;
				ice_server.username = stun_server.Username;
				ice_server.password = stun_server.Password;

				outRtcConfig->servers.push_back(ice_server);
			}
		}

		UpdateConfigWithRelayToken(outRtcConfig);
	}

	void SimpleNetworkInterfaceImpl::UpdateConfigWithRelayToken(webrtc::PeerConnectionInterface::RTCConfiguration* pRtcConfig) {
		if (!pRtcConfig->servers.empty()) {
			for (auto& server : pRtcConfig->servers) {
				server.username = mUsernameCert;
				server.password = mPasswordCert;
			}
		}
	}

	bool SimpleNetworkInterfaceImpl::IsBroadcastDiscoveryEnabled() const {
		auto lan_thread = &NetherNet::getLanThread();
		return lan_thread->IsBroadcastDiscoveryEnabled(mReceiverId);
	}

	bool SimpleNetworkInterfaceImpl::IsPacketAvailable(NetworkID remoteId, uint32_t* pcbMessageSize) {
		std::lock_guard<std::mutex> lock(mApiLevelMutex);
		return mNetworkSessionMgr->IsPacketAvailable(remoteId, pcbMessageSize);
	}

	void SimpleNetworkInterfaceImpl::ReceiveFromLanSignalingChannel(NetworkID remoteId, std::string const& data) {
		ReceiveFromLanSignalingChannel(remoteId, data, false, SignalingChannelId::Lan);
	}

	void SimpleNetworkInterfaceImpl::ReceiveFromWebSocketSignalingChannel(NetworkID remoteId, std::string const& data, bool a3) {
		ReceiveFromLanSignalingChannel(remoteId, data, a3, SignalingChannelId::WebSocket);
	}

	void SimpleNetworkInterfaceImpl::NotifyOnSessionOpen(NetworkID id) {
		mCallBack->OnSessionOpen(id);
	}

	void SimpleNetworkInterfaceImpl::NotifyOnSessionClose(NetworkID id, ESessionError err) {
		mCallBack->OnSessionClose(id, err);
	}

	bool SimpleNetworkInterfaceImpl::IsSignedIntoSignalingService() const {
		if (mpWebRTCSignalingInterface)
			return mpWebRTCSignalingInterface->IsSignedIn();
		return 0;
	}

	void SimpleNetworkInterfaceImpl::ProcessTurnConfig(const std::string& config) {
		auto signal_thread = getSignalThread();
		auto rtc_thread = signal_thread->LoadRtcThread();
		
		if (rtc_thread == nullptr)
			return;

		rtc_thread->PostTask([&]() {
			NetherNet::NetherNetTransport_LogMessage(LogSeverity::Information, "Parsing message contents from config: '%s'", config.data());
			Json::Reader confReader;
			Json::Value confVal(0);

			if (!confReader.parse(config.data(), confVal)) {
				return;
			}

			if (mpWebRTCSignalingInterface.get()) {
				auto& expirationSec = confVal["ExpirationInSeconds"];
				int expTimeSec = expirationSec.asInt();
				::NetherNet::NetherNetTransport_LogMessage(LogSeverity::Information, "Found expiration time in seconds %d", expTimeSec);
			}

			std::vector<TurnAuthServer> turn_auth_server_list;

			auto& turnAuthServers = confVal["TurnAuthServers"];
			if (!turnAuthServers.isNull()) {
				if (turnAuthServers.isArray()) {
					NetherNet::NetherNetTransport_LogMessage(
						NetherNet::LogSeverity::Information,
						"Found TurnAuthServers, so we have the new services data contract");

					for (const auto& service_data : turnAuthServers) {
						TurnAuthServer turnAuthServer;
						auto& user_name = service_data["Username"];
						turnAuthServer.userName = user_name.asString();

						auto& passwd = service_data["Password"];
						turnAuthServer.password = passwd.asString();

						NetherNet::NetherNetTransport_LogMessage(
							NetherNet::LogSeverity::Information,
							"Found username/password: '%s' / '%s'",
							turnAuthServer.userName.c_str(),
							turnAuthServer.password.c_str());

						auto& urls = service_data["Urls"];
						if (urls.isNull() || !urls.isArray()) {
							NetherNet::NetherNetTransport_LogMessage(NetherNet::LogSeverity::Error, "Failed to find Urls");
						}

						for (const auto& current_url : urls) {
							auto url_str = current_url.asString();
							if (url_str.find("turn") != std::string::npos) {
								NetherNet::NetherNetTransport_LogMessage(
									NetherNet::LogSeverity::Information,
									"Found turn server '%s'",
									url_str.c_str());
								turnAuthServer.turnUri = url_str;
							}
							if (url_str.find("stun") != std::string::npos) {
								NetherNet::NetherNetTransport_LogMessage(
									NetherNet::LogSeverity::Information,
									"Found stun server '%s'",
									url_str.c_str());
								turnAuthServer.stunUri = url_str;
							}
						}

						turn_auth_server_list.push_back(turnAuthServer);
					}
				}
			}

			TurnAuthServer turnAuthServer;
			NetherNet::NetherNetTransport_LogMessage(
				NetherNet::LogSeverity::Warning,
				"Did not find TurnAuthServers, so we have the old services data contract");

			turnAuthServer.userName = confVal["Username"].asString();
			turnAuthServer.password = confVal["Password"].asString();

			NetherNet::NetherNetTransport_LogMessage(
				NetherNet::LogSeverity::Information,
				"Found username/password: '%s' / '%s'",
				turnAuthServer.password.c_str(),
				turnAuthServer.password.c_str());

			turn_auth_server_list.push_back(turnAuthServer);

			mRtcConfig.servers.clear();
			mRtcConfig.servers.reserve(turn_auth_server_list.size());

			if (!mRtcConfig.servers.empty()) {
				for (const auto& tauth_server : turn_auth_server_list) {
					webrtc::PeerConnectionInterface::IceServer ice_server;
					ice_server.uri = tauth_server.stunUri;
					ice_server.username = tauth_server.userName;
					ice_server.password = tauth_server.password;

					mRtcConfig.servers.push_back(ice_server);
				}
			}

			NetherNet::NetherNetTransport_LogMessage(NetherNet::LogSeverity::Information, "IceServer list...");
			for (const auto& ice_server : mRtcConfig.servers) {
				NetherNet::NetherNetTransport_LogMessage(
					NetherNet::LogSeverity::Information,
					"uri/username/password: '%s' / '%s' / '%s'",
					ice_server.uri.c_str(),
					ice_server.username.c_str(),
					ice_server.password.c_str());
			}
		});
	}

	void SimpleNetworkInterfaceImpl::ProcessSessionError(NetworkID remoteId, ESessionError err) const {
		mNetworkSessionMgr->ProcessError(remoteId, err);
	}

	bool SimpleNetworkInterfaceImpl::CloseSessionWithUser(NetworkID id) {
		std::lock_guard lock(mApiLevelMutex);
		return mNetworkSessionMgr->CloseSessionWithUser(id);
	}

	void 
	SimpleNetworkInterfaceImpl::DisableBroadcastDiscovery() const {
		auto& lan_thread = getLanThread();
		lan_thread.DisableBroadcastDiscovery(mReceiverId);
	}

	void 
	SimpleNetworkInterfaceImpl::EnableBroadcastDiscovery() const {
		auto& lan_thread = getLanThread();
		lan_thread.EnableBroadcastDiscovery(mReceiverId);
	}

	void 
	SimpleNetworkInterfaceImpl::Initialize() {
		mNetworkSessionMgr
			= std::make_unique<NetworkSessionManager>(this);

		auto signal_thread = getSignalThread();
		auto rtc_thread = signal_thread->LoadRtcThread();

		if (rtc_thread) {
			//TODO
		}
	}
}