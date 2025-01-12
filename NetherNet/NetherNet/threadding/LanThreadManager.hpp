#pragma once

#include <memory>
#include <rtc_base/async_packet_socket.h>
#include <rtc_base/async_udp_socket.h>
#include <api/jsep.h>

#include "../network/NetworkID.hpp"
#include "../encryption/OpenSSLAesAdapter.hpp"
#include "../network/packets/DiscoveryResponsePacket.hpp"

namespace NetherNet {
	class LanThreadManager : public rtc::Thread {
	public:
		LanThreadManager() = default;
		bool IsBroadcastDiscoveryEnabled(NetworkID id);
		void SendLanBroadcastRequest(NetherNet::NetworkID id);
		void SendLanBroadcastResponse(rtc::SocketAddress const& addr, DiscoveryResponsePacket const& packet);
		std::error_code SendToHelper(
			void const*											   data,
			size_t												   size,
			rtc::SocketAddress const&							   addr);
		void CreateEncryptedBroadcastSocket();
	private:
		rtc::AsyncPacketSocket*			mSocket;		//this + 0x180
		uint16_t						mEphemeralPort;	//this + 0x1E8
		std::unique_ptr<rtc::IPAddress> mConnAddress;	//this + 0x208
		int								mConnPort;		//this + 0xF40
	};

	LanThreadManager& getLanThread();

	extern LanThreadManager* g_LanThread;
}