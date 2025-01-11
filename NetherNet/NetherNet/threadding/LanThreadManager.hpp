#pragma once

#include <memory>
#include <rtc_base/async_packet_socket.h>

#include "../network/NetworkID.hpp"
#include <rtc_base/async_udp_socket.h>

namespace NetherNet {
	class LanThreadManager : public rtc::Thread {
	public:
		bool IsBroadcastDiscoveryEnabled(NetworkID id);
		void SendLanBroadcastRequest(std::unique_ptr<rtc::AsyncPacketSocket>& socket, NetherNet::NetworkID id);
		std::error_code SendToHelper(std::unique_ptr<rtc::AsyncPacketSocket>& socket,
			void const*											   data,
			size_t												   size,
			rtc::SocketAddress const&							   addr);
	private:
		rtc::AsyncUDPSocket				mConnUdpSocket; //this + 0x180
		rtc::IPAddress					mClientAddr;
		int								mConnPort;		//this + 0xF40
		std::unique_ptr<rtc::IPAddress> mConnAddress;	//this + 0x1040
	};

	LanThreadManager& getLanThread();

	extern LanThreadManager g_LanThread;
}