

#include <json/json.h>
#include <rtc_base/openssl_key_pair.h>

#include "../HNetherNet.hpp"
#include "../threadding/LanThreadManager.hpp"
#include "SimpleNetworkInterfaceImpl.hpp"
#include "../network/SignalingChannelId.hpp"
#include "../network/packets/ESessionError.hpp"

namespace NetherNet {
	void SimpleNetworkInterfaceImpl::EnableSignalingOverLAN() {
		if (mDisableLANSignaling) {
			NetherNet::NetherNetTransport_LogMessage(4, "Enabling LAN for both signaling and transport");
			mDisableLANSignaling = false;
		}
	}

	void SimpleNetworkInterfaceImpl::DisableSignalingOverLAN() {
		if (!mDisableLANSignaling) {
			NetherNet::NetherNetTransport_LogMessage(4, "Disabling LAN for both signaling and transport");
			mDisableLANSignaling = true;
		}
	}

	void SimpleNetworkInterfaceImpl::AcceptSessionWithUser(NetworkID id) {
		std::lock_guard<std::mutex> lock(mConnectionMtx);
		mNetworkSessionMgr->AcceptSessionWithUser(id);
	}

	void SimpleNetworkInterfaceImpl::HandleDiscoveryPacketOnSignalThread(
		rtc::SocketAddress* addr,
		DiscoveryMessagePacket* packet
	) {
		if (packet->RecipientId() == mReceiverId) {
			auto message_data = packet->MessageData();

			std::string packet_data;
			packet_data.assign(message_data.data());

			std::string host_addr = addr->ToString();
			::NetherNet::NetherNetTransport_LogMessage(
				4,
				"Received discovery message from \"%s\" with network id \"%llu\" and message \"%s\"",
				host_addr.data(),
				packet->SenderId(),
				message_data.data()
			);

			ReceiveFromLanSignalingChannel(packet->SenderId(), packet_data);
		}
	}

	void SimpleNetworkInterfaceImpl::ReceiveFromLanSignalingChannel(NetworkID remoteId, const std::string& message, bool a3, SignalingChannelId channelId) {
		::NetherNet::NetherNetTransport_LogMessage(
			4,
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
		}else if(message.find("Username") != std::string::npos){
			ProcessTurnConfig(message);
		}
	}

	void SimpleNetworkInterfaceImpl::HandleDiscoveryPacketOnSignalThread(rtc::SocketAddress* addr, DiscoveryRequestPacket* packet) {

	}

	//...

	//TODO
	void SimpleNetworkInterfaceImpl::InitializeConfiguration(
		const NetherNetTransportServerConfiguration* config,
		webrtc::PeerConnectionInterface::RTCConfiguration* outRtcConfig
	) {
		outRtcConfig->servers.clear();

		if (config->mIceServerEntryCount > 0) {
			size_t counter = 0;
			ConfigurationEntry* p_config_ent = config->mIceServerEntry;

			while(counter < config->mIceServerEntryCount) {
				webrtc::PeerConnectionInterface::IceServer ice_server;

				ice_server.uri.assign(p_config_ent->url);
				ice_server.username.assign(p_config_ent->username);
				ice_server.password.assign(p_config_ent->password);

				outRtcConfig->servers.push_back(ice_server);

				p_config_ent++;
			}
		}

		UpdateConfigWithRelayToken(outRtcConfig);
	}
	//TODO
	void SimpleNetworkInterfaceImpl::UpdateConfigWithRelayToken(webrtc::PeerConnectionInterface::RTCConfiguration* pRtcConfig) {
		if (/**(this + 63)*/ true)
		{
			if (/**(this + 67)*/ true)
			{
				auto& ice_servers = pRtcConfig->servers;
				if (!ice_servers.empty()) {
					for (auto& it : ice_servers) {
						it.username = mUsernameCert;
						it.password = mPasswordCert;
					}
				}
			}
		}
	}

	bool SimpleNetworkInterfaceImpl::IsBroadcastDiscoveryEnabled() const {
		auto lan_thread = &NetherNet::getLanThread();
		return lan_thread->IsBroadcastDiscoveryEnabled(mReceiverId);
	}

	bool SimpleNetworkInterfaceImpl::IsPacketAvailable(NetworkID remoteId, uint32_t* pcbMessageSize) {
		std::lock_guard<std::mutex> lock(mConnectionMtx);
		return mNetworkSessionMgr->IsPacketAvailable(remoteId, pcbMessageSize);
	}

	void SimpleNetworkInterfaceImpl::ReceiveFromLanSignalingChannel(NetworkID remoteId, std::string const& data) {
		ReceiveFromLanSignalingChannel(remoteId, data, false, SignalingChannelId::Lan);
	}

	void SimpleNetworkInterfaceImpl::ReceiveFromWebSocketSignalingChannel(NetworkID remoteId, std::string const& data, bool a3) {
		ReceiveFromLanSignalingChannel(remoteId, data, a3, SignalingChannelId::WebSocket);
	}


}