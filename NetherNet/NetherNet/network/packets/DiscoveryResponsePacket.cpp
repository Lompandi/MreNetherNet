
#include "DiscoveryResponsePacket.hpp"

namespace NetherNet {
	uint32_t GetResponsePacketLength(size_t data_len) { return std::min(data_len, 1148ULL); }
	uint32_t GetResponsePayloadSize(size_t size) { return std::min(size, 1148ULL); }
}