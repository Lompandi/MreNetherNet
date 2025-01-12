#pragma once

#include <chrono>
#include <mutex>

#include <rtc_base/socket_address.h>
#include <api/peer_connection_interface.h>

#include "NetherNetTransportServerConfiguration.hpp"
#include "INetherNetTransportInterfaceCallbacks.hpp"

#include "../network/NetworkSessionManager.hpp"
#include "../network/packets/DiscoveryMessagePacket.hpp"
#include "../network/packets/DiscoveryRequestPacket.hpp"
#include "../network/packets/DiscoveryResponsePacket.hpp"
#include "../network/SignalingChannelId.hpp"

namespace NetherNet { struct SessionState; }
namespace NetherNet { struct StunRelayServer; }
namespace NetherNet { struct ERelayServerConfigurationResult; }


namespace NetherNet {
	struct SimpleNetworkInterfaceImpl {

	public:
		[[nodiscard]] std::chrono::seconds GetNegotiationTimeout() { return mNegotiationTimeout; }

		void AcceptSessionWithUser(NetworkID id);
		void ClearPacketData(NetworkID id);
		void CloseSessionWithUser(NetworkID id);
		void DisableBroadcastDiscovery();
		void DisableSignalingOverLAN();
		void EnableBroadcastDiscovery();
		void EnableSignalingOverLAN();
		uint32_t GetConnectionFlags(NetworkID remoteId);
		std::chrono::seconds GetNegotiationTimeout();
		bool GetSessionState(NetworkID remoteId, uint64_t connectionId, SessionState* pConnState);
		void HandleDiscoveryPacketOnSignalThread(rtc::SocketAddress* addr, DiscoveryMessagePacket* packet);
		void HandleDiscoveryPacketOnSignalThread(rtc::SocketAddress* addr, DiscoveryRequestPacket* packet);
		void HandleDiscoveryPacketOnSignalThread(rtc::SocketAddress* addr, DiscoveryResponsePacket* packet);
		void ReceiveFromLanSignalingChannel(NetworkID remoteId, const std::string& data, bool a3, SignalingChannelId channelId);
		void ProcessSessionError(NetworkID remoteId, ESessionError err);
		void ProcessTurnConfig(const std::string& config);
		void Initialize();
		void InitializeConfiguration(
			const NetherNetTransportServerConfiguration* config,
			webrtc::PeerConnectionInterface::RTCConfiguration* outRtcConfig
		);
		bool IsBroadcastDiscoveryEnabled() const;
		bool IsPacketAvailable(NetworkID remoteId, uint32_t* pcbMessageSize);
		bool IsSignedIntoSignalingService();
		void NotifyOnSessionOpen();
		void NotifyOnSessionClose(NetworkID id, ESessionError err);

		void ReceiveFromLanSignalingChannel(NetworkID remoteId, std::string const& data);
		void ReceiveFromWebSocketSignalingChannel(NetworkID remoteId, std::string const& data, bool a3);

		void UpdateConfigWithRelayToken(webrtc::PeerConnectionInterface::RTCConfiguration* pRtcConfig);
	public:
		NetworkID								mRemoteId;				// this + 0x008
		INetherNetTransportInterfaceCallbacks*  mRecvCallBack;			// this + 0x010
		NetworkID								mReceiverId;			// this + 0x018
		std::shared_ptr<NetworkSessionManager>	mNetworkSessionMgr;		// this + 0x020
		std::mutex								mConnectionMtx;			// this + 0x030
		std::string								mUsernameCert;			// this + 0x1E8
		std::string								mPasswordCert;			// this + 0x208
		bool									mDisableLANSignaling;	// this + 0x228
		std::chrono::seconds					mNegotiationTimeout;	// this + 0x288
	};
}