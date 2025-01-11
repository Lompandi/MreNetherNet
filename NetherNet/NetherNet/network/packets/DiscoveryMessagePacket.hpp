#pragma once

#include <array>
#include <string_view>

#include "../NetworkID.hpp"
#include "DiscoveryPacket.hpp"

namespace NetherNet {
	struct DiscoveryMessagePacket : public ::NetherNet::DiscoveryPacket {
		//TODO
		DiscoveryMessagePacket(NetworkID senderId, NetworkID recipientId, std::string_view message)
		: DiscoveryPacket(senderId, NetherNet::DiscoveryPacketType::Message, message.size()) {
			mMessageSize = message.size();
			mRecipientId = recipientId;

			//TODO: add bound check
			std::memcpy(mMessageData.data(), message.data(), mMessageSize);
		}

		std::string_view MessageData() { 
			return std::string_view((const char*)mMessageData.data(), size_t(mMessageSize));
		}

		NetworkID RecipientId() const noexcept { return mRecipientId; }
	private:
		NetworkID					mRecipientId;	//this + 0x14
		uint32_t					mMessageSize;   //this + 0x1C
		std::array<uint8_t, 1140>	mMessageData;   //this + 0x20 size could be 1140 bytes
	};
}