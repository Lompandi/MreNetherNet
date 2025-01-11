#pragma once

#include <cstdint>

namespace NetherNet {
    enum class DiscoveryPacketType : uint16_t {
        Request = 0,
        Response = 1,
        Message = 2,
    };
}