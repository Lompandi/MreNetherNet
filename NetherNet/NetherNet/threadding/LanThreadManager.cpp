
#include <rtc_base/async_udp_socket.h>

#include "../HNetherNet.hpp"
#include "LanThreadManager.hpp"
#include "../network/packets/DiscoveryRequestPacket.hpp"

namespace NetherNet {

	LanThreadManager* g_LanThread;

	std::error_code LanThreadManager::SendToHelper(
		void const* data,
		size_t size,
		rtc::SocketAddress const& addr) {

		auto sendResult = 0;

		//TODO:
		if (addr.ipaddr().family() != AF_INET) {
			//Other network layer protocol used
			rtc::PacketOptions packetOpt;
			sendResult = mSocket->SendTo(data, size, addr, packetOpt);
		}
		else {
			//Ipv6 processing
			rtc::PacketOptions packetOpt;
			auto ip6Addr = addr.ipaddr().AsIPv6Address();
			auto sendPort = addr.port();

			rtc::SocketAddress ip6SocksAddr;
			ip6SocksAddr.FromString(ip6Addr.ToString());
			sendResult = mSocket->SendTo(data, size, ip6SocksAddr, packetOpt);
		}

		if (sendResult == -1) {
			//TODO: process error;
		}
	}

	bool LanThreadManager::IsBroadcastDiscoveryEnabled(NetworkID id) {
		//TODO
		return true;
	}

	void LanThreadManager::SendLanBroadcastRequest(NetherNet::NetworkID id) {
		if (!true) {
			return;
		}

		DiscoveryRequestPacket disc_req_pkt(id);

		while (true) {
			std::string ipAddrStr = mConnAddress->ToString();

			::NetherNet::NetherNetTransport_LogMessage(
				4,
				"[LAN] [%llu] sending broadcast request to %s",
				id,
				ipAddrStr.data()
			);

			rtc::SocketAddress sockAddr(ipAddrStr, mConnPort);
			auto SenderId = disc_req_pkt.SenderId();
			/*TODO: SendToHelper(disc_req_pkt, )*/

		}
	}

	void LanThreadManager::SendLanBroadcastResponse(rtc::SocketAddress const& addr, DiscoveryResponsePacket const& packet) {
		//TODO
	}

	void LanThreadManager::CreateEncryptedBroadcastSocket() {

	}

	LanThreadManager& getLanThread() {
		return *g_LanThread;
	}
}