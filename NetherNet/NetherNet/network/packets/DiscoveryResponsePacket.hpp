
#ifndef DISCOVERY_RESPONSE_PACKET_HPP
#define DISCOVERY_RESPONSE_PACKET_HPP

#include <array>
#include <string_view>

#include "DiscoveryPacket.hpp"

namespace NetherNet {
	uint32_t GetResponsePacketLength(size_t data_len);
	uint32_t GetResponsePayloadSize(size_t size);

	struct DiscoveryResponsePacket : public ::NetherNet::DiscoveryPacket {
	public:
		DiscoveryResponsePacket(NetworkID id, std::string_view data) :
			DiscoveryPacket(id, DiscoveryPacketType::Response, GetResponsePacketLength(data.size())) {

			mMessageSize = GetResponsePayloadSize(data.size());
			std::memcpy(mMessageData.data(), data.data(), mMessageSize);
		}

		std::string_view ApplicationData() const { return std::string_view((const char*)mMessageData.data(), mMessageSize); }
	public:
		uint32_t					mMessageSize;		//this + 0x14
		std::array<uint8_t, 1148>	mMessageData;		//this + 0x18
	};
}

#endif