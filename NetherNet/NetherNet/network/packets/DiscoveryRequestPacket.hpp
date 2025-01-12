#pragma once

#include "../NetworkID.hpp"

#include "DiscoveryPacket.hpp"

namespace NetherNet {
	struct DiscoveryRequestPacket : public ::NetherNet::DiscoveryPacket {
		DiscoveryRequestPacket(NetworkID id)
			: DiscoveryPacket(id, DiscoveryPacketType::Request, 20) {
			mId = id;
		}

	public:
		//TODO
		NetworkID mId;
	};
}