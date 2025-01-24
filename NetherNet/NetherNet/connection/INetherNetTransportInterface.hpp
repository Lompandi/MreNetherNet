#pragma once
#include "NetworkSession.hpp"

namespace NetherNet {

    class INetherNetTransportInterface {
    public:
        // vIndex: 0
        virtual ~INetherNetTransportInterface();

        // vIndex: 1
        virtual bool IsSignedIntoSignalingService() const = 0;

        // vIndex: 2
        virtual bool SendPacket(::NetherNet::NetworkID, uint64_t, char const*, uint32_t, ::NetherNet::ESendType) = 0;

        // vIndex: 3
        virtual bool IsPacketAvailable(::NetherNet::NetworkID, uint64_t, uint32_t*) = 0;

        // vIndex: 4
        virtual void ClearPacketData(::NetherNet::NetworkID, uint64_t) = 0;

        // vIndex: 5
        virtual bool ReadPacket(::NetherNet::NetworkID, uint64_t, void*, uint32_t, uint32_t*) = 0;

        // vIndex: 6
        virtual bool OpenSessionWithUser(::NetherNet::NetworkID) = 0;

        // vIndex: 7
        virtual bool CloseSessionWithUser(NetworkID) = 0;

        // vIndex: 8
        virtual bool GetSessionState(::NetherNet::NetworkID, uint64_t, ::NetherNet::SessionState*) = 0;

        // vIndex: 9
        virtual void SetWebRTCSignalingInterface(::std::shared_ptr<::NetherNet::IWebRTCSignalingInterface> const&) = 0;

        /*// vIndex: 10
        virtual void
            SignIntoSignalingService(::std::function<void(::std::error_code)> const&, ::std::function<void(::NetherNet::SignalingHostConnectionStatus, uint32_t)> const&, ::NetherNet::IWebRTCSignalingInterface::SignalingConfiguration const&) = 0;

        // vIndex: 11
        virtual void SignOutFromSignalingService() = 0;

        // vIndex: 12
        virtual void
            RegisterRelayServerCredentialsReadyCallback(::std::function<void(::NetherNet::ERelayServerConfigurationResult
                )> const&) = 0;*/

                // vIndex: 13
        virtual bool IsBroadcastDiscoveryEnabled() = 0;

        // vIndex: 14
        virtual void EnableBroadcastDiscovery() = 0;

        // vIndex: 15
        virtual void DisableBroadcastDiscovery() = 0;

        // vIndex: 16
        virtual void EnableSignalingOverLAN() = 0;

        // vIndex: 17
        virtual void DisableSignalingOverLAN() = 0;

        /*// vIndex: 19
        virtual ::Bedrock::PubSub::Subscription RegisterEventHandler(::NetherNet::ISignalingEventHandler*) = 0;

        // vIndex: 18
        virtual ::Bedrock::PubSub::Subscription RegisterEventHandler(::NetherNet::ILanEventHandler*) = 0;
        // NOLINTEND*/

    };
}
