
#include <rtc_base/thread.h>
#include <rtc_base/async_udp_socket.h>
#include <rtc_base/network.h>

#include "../HNetherNet.hpp"
#include "LanThreadManager.hpp"
#include "../network/packets/DiscoveryRequestPacket.hpp"

namespace NetherNet {

	LanThreadManager* g_LanThread;

	void LanThreadManager::BeginNetworkDiscovery() {
		NetherNet::NetherNetTransport_LogMessage(4, "[LAN] starting network discovery");
		auto network_mgr = std::make_unique<rtc::BasicNetworkManager>(mSocketServer);
		mBasicNetworkMgr = std::move(network_mgr);

		//TODO

		auto on_complete_callback = &LanThreadManager::OnNetworkDiscoveryComplete;
		mBasicNetworkMgr->SignalNetworksChanged.connect(this, on_complete_callback);
		mBasicNetworkMgr->SignalError.connect(this, on_complete_callback);
		mBasicNetworkMgr->StartUpdating();
	}

	std::error_code LanThreadManager::SendToHelper(
		void const* data,
		size_t size,
		rtc::SocketAddress const& addr) {

		auto sendResult = 0;

		if (!IsCurrent()) {
			//BlockingCall();
		}

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
			//TODO: Dont know if it is always the first address
			std::string ipAddrStr = mAddressList.begin()->ToString();

			::NetherNet::NetherNetTransport_LogMessage(
				4,
				"[LAN] [%llu] sending broadcast request to %s",
				id,
				ipAddrStr.data()
			);

			rtc::SocketAddress sockAddr(ipAddrStr, mConnPort);
			auto SenderId = disc_req_pkt.SenderId();
			auto result = SendToHelper(&disc_req_pkt, disc_req_pkt.PacketLength(), sockAddr);

			if (!result) {
				//TODO: process
			}
		}
	}

	void LanThreadManager::Initialize(const char* thread_name, ThreadInit const& init, uint64_t id, uint16_t port, std::chrono::milliseconds interval) {

	}

	void LanThreadManager::SendLanBroadcastResponse(rtc::SocketAddress const& addr, DiscoveryResponsePacket const& packet) {
		//TODO
	}

	void LanThreadManager::CreateEncryptedBroadcastSocket() {
		//TODO
	}

	void LanThreadManager::BroadcastTask() {
		//TODO
	}

	void LanThreadManager::EnableBroadcastDiscovery(NetworkID id) {
		if (IsCurrent()) {
			NetherNet::NetherNetTransport_LogMessage(4, "[LAN] adding [%llu] to set of broadcast ids", id);
			if (!false /*TODO*/ ) {
				NetherNet::NetherNetTransport_LogMessage(4, "[LAN] starting broadcast task");

				this->PostTask([&]() {
					BroadcastTask();  //Start broadcasting
				});

				//TODO:
			}
		}
		else {
			this->PostTask([&]() {EnableBroadcastDiscovery(mRemoteId); });
		}
	}

	void LanThreadManager::DestroyLanResources() {
		/*TODO: destruct socket*/

		rtc::SocketAddress sock_addr;
		mSocketAddress = sock_addr;	//clear socket address

		//TODO:
	}

	void LanThreadManager::DisableBroadcastDiscovery(NetworkID id) {
		if (!IsCurrent())
			return;
		NetherNet::NetherNetTransport_LogMessage(4, "[LAN] removing [%llu] from set of broadcast ids", id);
		mBroadcastIdList.erase(id);
	}

	void LanThreadManager::OnNetworkDiscoveryComplete() {
		//TODO
	}

	bool LanThreadManager::IsNetworkIdOnLan(NetworkID id) {
		return mPeerRecordTable->Contains(id);
	}

	LanThreadManager& getLanThread() {
		return *g_LanThread;
	}
}