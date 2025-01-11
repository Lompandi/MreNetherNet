
#include <rtc_base/async_udp_socket.h>

#include "../HNetherNet.hpp"
#include "LanThreadManager.hpp"

namespace NetherNet {

	LanThreadManager g_LanThread;

	std::error_code LanThreadManager::SendToHelper(
		std::unique_ptr<rtc::AsyncPacketSocket>& socket,
		void const* data,
		size_t size,
		rtc::SocketAddress const& addr) {

		rtc::PacketOptions packetOpt;
		auto ip6Addr = mClientAddr.AsIPv6Address();
		auto sendPort = addr.port();

		rtc::SocketAddress ip6SocksAddr;
		ip6SocksAddr.FromString(ip6Addr.ToString());

		//Currently for dev stage, we will hardecode UDP here.
		//TODO: reverse ut sdp, etc...
		auto sendResult = mConnUdpSocket.SendTo(data, size, addr, packetOpt);
		if (sendResult == -1) {
			//Fetch error
		}


	}

	void LanThreadManager::SendLanBroadcastRequest(std::unique_ptr<rtc::AsyncPacketSocket>& socket, NetherNet::NetworkID id) {
		if (!true) {
			return;
		}

		while (true) {
			std::string ipAddrStr = mConnAddress->ToString();

			::NetherNet::NetherNetTransport_LogMessage(
				4,
				"[LAN] [%llu] sending broadcast request to %s",
				socket.get(),
				ipAddrStr
			);

			rtc::SocketAddress sockAddr(ipAddrStr, mConnPort);
			//Type = Social::GameConnectionInfo::getType();
			//TODO:
			SendToHelper(socket, );
		}
	}

	LanThreadManager& getLanThread() {
		return g_LanThread;
	}
}