#pragma once

namespace NetherNet {
	struct DiscoveryPacketHeader {
		DiscoveryPacketHeader(NetherNet::DiscoveryPacketType type, uint16_t len) {
			mPacketLen = len;
			mPacketType = type;
		}

	public:
		[[nodiscard]] uint16_t PacketLength() const noexcept { return mPacketLen; }
		[[nodiscard]] DiscoveryPacketType PacketType() const noexcept { return mPacketType; }

	private:
		uint16_t			mPacketLen;		//this + 0x0
		DiscoveryPacketType	mPacketType;	//this + 0x02
		
	};
}