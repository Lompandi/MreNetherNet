#pragma once
#include <cstdint>

namespace NetherNet {
    struct ConfigurationEntry {
        const char* url;
        const char* username;
        const char* password;
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
        ConfigurationEntry* mIceServerEntry;
        size_t              mIceServerEntryCount;
    };
}