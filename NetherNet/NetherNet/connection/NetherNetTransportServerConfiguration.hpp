#pragma once
#include <cstdint>

namespace NetherNet {
    struct StunRelayServer {
        const char* Uri;
        const char* Username;
        const char* Password;
    };

    struct KnownMappedAddress {
        int InternalPort;
        const char* ExternalAddress;
        int ExternalPort;
    };

    struct NetherNetTransportServerConfiguration {
    public:
        enum class SignalingHost : uint8_t {
            WebSocket = 0,
            Deferred = 1,
            TcpClientPrototype = 2,
            TcpServerPrototype = 3,
            None = 4,
        };

    public:
        StunRelayServer     StunRelayServers[16];
        size_t              StunRelayServerCount;

        KnownMappedAddresses;
        int KnownMappedAddressCount;

        std::string TitleControlsSignalingSignIn;
    };
}