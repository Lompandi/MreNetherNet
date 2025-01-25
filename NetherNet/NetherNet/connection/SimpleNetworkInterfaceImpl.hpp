#ifndef SIMPLE_NETWORK_INTERFACE_IMPL_HPP
#define SIMPLE_NETWORK_INTERFACE_IMPL_HPP

#pragma once

#include <chrono>
#include <mutex>

#include <rtc_base/socket_address.h>
#include <api/peer_connection_interface.h>
#include <pc/peer_connection_factory.h>

#include "NetherNetTransportServerConfiguration.hpp"
#include "INetherNetTransportInterfaceCallbacks.hpp"

#include "../network/packets/DiscoveryMessagePacket.hpp"
#include "../network/packets/DiscoveryRequestPacket.hpp"
#include "../network/packets/DiscoveryResponsePacket.hpp"

#include "../network/SignalingChannelId.hpp"
#include "../network/packets/ESessionError.hpp"
#include "../network/NetworkSessionManager.hpp"
#include "IWebRTCSignalingInterface.hpp"
#include "INetherNetTransportInterface.hpp"
#include "../network/ESendType.hpp"

namespace NetherNet { struct StunRelayServer; }
namespace NetherNet { struct ERelayServerConfigurationResult; }
namespace NetherNet { struct SessionState; }
namespace NetherNet { class NetworkSessionManager; }

namespace NetherNet {
	struct TurnAuthServer {
		std::string userName;
		std::string password;
		std::string turnUri;
		std::string stunUri;
	};

	using SignalingMessage = std::variant<ConnectRequest, ConnectResponse, ConnectError, CandidateAdd>;

	struct SimpleNetworkInterfaceImpl : public INetherNetTransportInterface {
	public:

		void AcceptSessionWithUser(NetworkID id);
		
		void ClearPacketData(NetworkID id) const;
		
		bool CloseSessionWithUser(NetworkID id) override;
		
		void DisableBroadcastDiscovery() const;
		
		void DisableSignalingOverLAN();
		
		void EnableBroadcastDiscovery() const;
		
		void EnableSignalingOverLAN();
		
		uint32_t GetConnectionFlags(NetworkID remoteId);
		
		[[nodiscard]] std::chrono::seconds GetNegotiationTimeout() const { return mNegotiationTimeout; }
		
		bool GetSessionState(NetworkID remoteId, SessionState* pConnState);
		
		void HandleDiscoveryPacketOnSignalThread(rtc::SocketAddress& addr, DiscoveryMessagePacket& packet);
		
		void HandleDiscoveryPacketOnSignalThread(rtc::SocketAddress const& addr, DiscoveryRequestPacket const& packet);
		
		void HandleDiscoveryPacketOnSignalThread(rtc::SocketAddress& addr, DiscoveryResponsePacket& packet);
		
		void Initialize();

		void InitializeConfiguration(
			const NetherNetTransportServerConfiguration* config,
			webrtc::PeerConnectionInterface::RTCConfiguration* outRtcConfig
		);

		bool IsBroadcastDiscoveryEnabled() const;

		bool IsPacketAvailable(NetworkID remoteId, uint32_t* pcbMessageSize);

		bool IsSignedIntoSignalingService() const;

		void MakeDefaultSignalingChannel(NetherNetTransportServerConfiguration const*);

		void NotifyOnSessionClose(NetworkID id, ESessionError err);

		void NotifyOnSessionOpen(NetworkID id);
		
		void NotifyOnSessionRequested(NetworkID id);
		
		bool OpenSessionWithUser(NetworkID remoteId);
		
		void PeriodicUpdateOnSignalThread();
		
		void ProcessSessionError(NetworkID id, ESessionError err) const;
		
		void ProcessTurnConfig(const std::string& config);

		bool ReadPacket(NetworkID id, void* pubDest, unsigned int cbDest, unsigned int* pcbMessageSize);

		void ReceiveFromLanSignalingChannel(NetworkID remoteId, std::string const& data);

		void ReceiveFromSignalingChannel(NetworkID remoteId, const std::string& data, bool a3, SignalingChannelId channelId);

		void ReceiveFromWebSocketSignalingChannel(NetworkID remoteId, std::string const& data, bool a3);

		void RegisterRelayServerCredentialsReadyCallback(IWebRTCSignalingInterface::RelayServerConfigReadyCallback const& callback);

		void RegisterSignalingChannelCallbacks();

		int SendPacket(NetworkID remoteId, const char* pbdata, uint32_t cbData, ESendType sendType);

		ESessionError SendToSignalingChannel(NetworkID remoteId, SignalingMessage message, std::optional<SignalingChannelId> preference, std::function<void(std::error_code)>&& onComplete);

		void SetServerConfiguration(NetherNetTransportServerConfiguration const* config);

		void SetWebRTCSignalingInterface(std::shared_ptr<IWebRTCSignalingInterface> const& webrtc_interface);

		void SignIntoSignalingService(
			IWebRTCSignalingInterface::SignInCallback const& signInCallback,
			IWebRTCSignalingInterface::ConnectionStatusChangedCallback const& connectionStatusChangedCallback,
			IWebRTCSignalingInterface::SignalingConfiguration const& signalingConfig
		);

		void SignOutFromSignalingService();

		void UpdateConfigWithRelayToken(webrtc::PeerConnectionInterface::RTCConfiguration* pRtcConfig);

	public:
		NetworkID								mRemoteId;				// this + 0x08
		INetherNetTransportInterfaceCallbacks*  mCallBack;				// this + 0x10
		NetworkID								mReceiverId;			// this + 0x18

		std::unique_ptr<NetworkSessionManager>					mNetworkSessionMgr;		// this + 0x20
		rtc::scoped_refptr<webrtc::PeerConnectionFactory>	mpPeerConnectionFactory;	// this + 0x28 
		std::mutex								mApiLevelMutex;			// this + 0x30
		webrtc::PeerConnectionInterface::RTCConfiguration mRtcConfig;	// this + 0x80
		std::string								mUsernameCert;			// this + 0x1E8
		std::string								mPasswordCert;			// this + 0x208
		bool									mDisableLANSignaling;	// this + 0x228
		std::shared_ptr<IWebRTCSignalingInterface> mpWebRTCSignalingInterface; //this + 0x230
		std::chrono::seconds					mNegotiationTimeout;	// this + 0x288
	};
}

#endif