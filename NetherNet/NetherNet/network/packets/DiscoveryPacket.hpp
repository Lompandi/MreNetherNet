#pragma once


#include "../NetworkID.hpp"

#include "DiscoveryPacketType.hpp"
#include "DiscoveryPacketHeader.hpp"

namespace NetherNet {
	struct DiscoveryPacket : public ::NetherNet::DiscoveryPacketHeader {
		DiscoveryPacket(NetworkID senderId, DiscoveryPacketType type, uint16_t len)
		: DiscoveryPacketHeader(type, len){
			mSenderId = senderId;
		}

		NetworkID SenderId() const noexcept { return mSenderId; }
	private:
		NetworkID mSenderId;
	};
}