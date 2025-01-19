
#include <rtc_base/thread.h>
#include <rtc_base/physical_socket_server.h>
#include <rtc_base/async_udp_socket.h>
#include <rtc_base/network.h>
#include <modules/rtp_rtcp/source/rtp_generic_frame_descriptor.h>

#include "../HNetherNet.hpp"
#include "LanThreadManager.hpp"

#include "../network/packets/DiscoveryMessagePacket.hpp"
#include "../network/packets/DiscoveryRequestPacket.hpp"
#include "../network/packets/DiscoveryResponsePacket.hpp"

namespace NetherNet {

	LanThreadManager* g_LanThread;
	in6_addr multi_cast_link_local;

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
		if (IsCurrent())
			return (mBroadcastIdList.contains(id));

		bool isEnabled = false;
		BlockingCall([&]() {
			isEnabled = IsBroadcastDiscoveryEnabled(id); 
		});
		return isEnabled;
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

	void LanThreadManager::OnPacket(rtc::AsyncPacketSocket* socket, const char* data, uint64_t size, rtc::SocketAddress const& addr, int64_t const& id2) {

		if (size >= 4 && size >= *reinterpret_cast<const uint16_t*>(data)) {
			auto dataStorage = TryMakePacketStorage(data);
			if (dataStorage[1176]) {
				//Fetch the network id
				if (*reinterpret_cast<const uint64_t*>(data + 4)) {
					// Fetch the packet id
					if (*reinterpret_cast<const uint16_t*>(data + 2)) {
						NetworkID id = *reinterpret_cast<const uint64_t*>(data + 4);
						if (mPeerRecordTable->AddOrUpdate(id, addr)) {
							//send response
							PostTask([&]() {
								//TODO: find callback
								SendSignalingMessageTo(0, mRemoteId, "Ping", nullptr);
							});
						}
					}
				}
				auto signal_thread = getSignalThread();
				rtc::SocketAddress sock_addr(addr);

				//TODO:
				//...

				auto rtc_thread = signal_thread->LoadRtcThread();
				if (rtc_thread) {
					rtc::SocketAddress sock_addr2(sock_addr);
					//TODO:
					//...
					rtc_thread->PostTask([&]() {
						char packetId = -1; //TODO
						/*std::variant<DiscoveryRequestPacket, DiscoveryResponsePacket, DiscoveryMessagePacket> packet_var;

						if (packetId) {
							if (packetId == 1)
								NetherNet::HandleDiscoveryPacketOnSignalThread(sock_addr2, std::get<DiscoveryResponsePacket>(packet_var));
							else
								NetherNet::HandleDiscoveryPacketOnSignalThread(sock_addr2, std::get<DiscoveryMessagePacket>(packet_var));
						}
						else
							NetherNet::HandleDiscoveryPacketOnSignalThread(sock_addr2, std::get<DiscoveryRequestPacket>(packet_var));*/
					});
				}

			}
		}
	}

	void LanThreadManager::Initialize(const char* thread_name, ThreadInit const& init, uint64_t id, uint16_t port, std::chrono::milliseconds interval) {

	}

	void LanThreadManager::SendLanBroadcastResponse(rtc::SocketAddress const& addr, DiscoveryResponsePacket const& packet) {
		//TODO
	}

	void LanThreadManager::CreateEncryptedBroadcastSocket() {

	}

	void LanThreadManager::BroadcastTask() {
		if (true /* TODO */) {
			if (ShouldBeginNetworkDiscovery())
				BeginNetworkDiscovery();
			
			for (const auto& addr : mBroadcastIdList) {
				SendLanBroadcastRequest(addr);
			}
			PostTask([&]() {
				BroadcastTask();
			});
		}
		else {
			NetherNet::NetherNetTransport_LogMessage(4, "[LAN] Stopping broadcast task, no more ids");
			//TODO
		}
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

	//IPv6 multicast address with link-local scope (FF02::1)
	rtc::IPAddress IPv6AllHostsLinkLocal(multi_cast_link_local);

	void LanThreadManager::OnNetworkDiscoveryComplete() {
		auto network_list = mBasicNetworkMgr->GetAnyAddressNetworks();
		
		for (const auto& network : network_list) {
			for (const auto& ip : network->GetIPs()) {
				if (!rtc::IPIsLoopback(ip)) {
					auto family = ip.family();
					if (family == AF_INET6) {
						if (mNetworkFamily != AF_INET6) {
							//goto process
						}

						auto addr_str = IPv6AllHostsLinkLocal.ToString();
						NetherNet::NetherNetTransport_LogMessage(4, "[LAN] adding %s to broadcast domain", addr_str.data());


					}
					else {
						if (family != AF_INET || rtc::IPIsLinkLocal(ip)) {
							auto addr_str = ip.ToString();
							NetherNet::NetherNetTransport_LogMessage(4, "[LAN] %s not viable for broadcast domain", addr_str.data());
							//goto End
						}

						//TODO
						auto flagset = -1 << (32 - ip.ipv6_flags());
						auto ipv4_addr = ip.ipv4_address();

						auto netlong = ipv4_addr.S_un.S_addr;
						auto addr_host_order = ntohl(netlong);

						auto masked_ip = htonl(~flagset | flagset & addr_host_order);
						rtc::IPAddress addr(masked_ip);
						auto addr_str = addr.ToString();

						NetherNet::NetherNetTransport_LogMessage(4, "[LAN] adding %s to broadcast domain", addr_str.data());

						//TODO
					}
				}
			}
		}
	}

	void LanThreadManager::SendSignalingMessageTo(NetworkID src, NetworkID dst, std::string const& data, std::function<void(std::error_code)>&& on_complete) {
		if (IsCurrent()) {
			rtc::SocketAddress addr;
			if (mPeerRecordTable->Find(dst, &addr)) {
				auto addr_str = addr.ToString();
				NetherNet::NetherNetTransport_LogMessage(
					4,
					"[%llu] sending signal to [%llu](%s) over LAN: '%s'",
					src,
					dst,
					addr_str.data(),
					data.data());

				//Social::GameConnectionInfo::getType is dedup of GetLen for packets

				std::string_view data_view(data.data(), data.size());
				DiscoveryMessagePacket discovery_msg_pkt(src, dst, data_view);

				//TODO: serialize the packet to bytes before sending it instead of using `data.data()`
				auto err = SendToHelper(data.data(), discovery_msg_pkt.PacketLength(), addr);
				if (on_complete) {
					on_complete(err);
				}
			}
			else {
				NetherNet::NetherNetTransport_LogMessage(2, "[%llu] signaling message to [%llu] dropped, unknown peer", src, dst);
				if (on_complete) {
					//TODO
				}
			}
		}
		else {
			std::string data_send = data;

			PostTask([&]() {
				SendSignalingMessageTo(src, dst, data_send, std::move(on_complete));
			});

			//TODO
		}
	}

	bool LanThreadManager::IsNetworkIdOnLan(NetworkID id) {
		return mPeerRecordTable->Contains(id);
	}

	bool LanThreadManager::ShouldBeginNetworkDiscovery() {
		return !mBasicNetworkMgr; /* && TODO */
	}

	LanThreadManager& getLanThread() {
		return *g_LanThread;
	}

	//Global value specified for any address (::)
	rtc::IPAddress IPv6Any(in6addr_any);
}

namespace {
	using namespace NetherNet;

	std::system_error GetLastSystemError() {
		DWORD errCode = GetLastError();
		return std::system_error(errCode, std::system_category());
	}

	const char* TryMakePacketStorage(const char* data) {
		auto packetId = *reinterpret_cast<const uint16_t*>(data + 2);
		if (packetId) {
			if (packetId == 1) {

			}
		}
	}

	ErrorOr<rtc::PhysicalSocket*, std::system_error> 
	TryCreateDualStackIPv6Socket(rtc::SocketServer* sock_server, uint16_t port) {
		auto ip6_socket = dynamic_cast<rtc::PhysicalSocket*>(sock_server->CreateSocket(AF_INET6, SOCK_DGRAM));
		if (!ip6_socket) {

			auto code = GetLastSystemError();

			NetherNet::NetherNetTransport_LogMessage(4, "[LAN] socket failed: %s (%d)", code.what(), code.code().value());
			return result_error{ code };
		}

		auto set_opt_res = ip6_socket->SetOption(rtc::Socket::Option::OPT_IPV6_V6ONLY, false);
		if (set_opt_res) {
			auto errCode = ip6_socket->GetError();
			auto err = std::system_error(errCode, std::system_category());

			NetherNet::NetherNetTransport_LogMessage(
				4,
				"[LAN] setsockopt(IPV6_V6ONLY) failed: %s (%d)",
				err.what(),
				err.code().value());
			return result_error{ err };
		}

		set_opt_res = ip6_socket->SetOption(rtc::Socket::Option::OPT_SO_BROADCAST, 1);
		if (set_opt_res) {
			auto err = ip6_socket->GetError();
			auto errorCode = std::system_error(err, std::system_category());

			NetherNet::NetherNetTransport_LogMessage(
				4,
				"[LAN] setsockopt(SO_BROADCAST) failed: %s (%d)",
				errorCode.what(),
				errorCode.code().value());

			return result_error{ errorCode };
		}

		if (ip6_socket->SetOption(rtc::Socket::Option::OPT_RCVBUF, 0x8000)) {
			auto err = ip6_socket->GetError();
			auto errorCode = std::system_error(err, std::system_category());

			NetherNet::NetherNetTransport_LogMessage(
				4,
				"[LAN] setsockopt(SO_RCVBUF) failed: %s (%d)",
				errorCode.what(),
				errorCode.code().value());
		}
		
		rtc::SocketAddress sock_addr(NetherNet::IPv6Any, port);
		if (ip6_socket->Bind(sock_addr)) {
			auto err = ip6_socket->GetError();
			auto errorCode = std::system_error(err, std::system_category());

			NetherNet::NetherNetTransport_LogMessage(4, "[LAN] bind failed: %s (%d)", errorCode.what(), errorCode.code().value());
			return result_error{ errorCode };
		}

		//TMP I'll initialize the in6_addr for link local here
		NetherNet::multi_cast_link_local.u.Byte[0] = 0xFF;
		NetherNet::multi_cast_link_local.u.Byte[1] = 0x02;
		NetherNet::multi_cast_link_local.u.Byte[15] = 0x01;

		rtc::IPAddress ip6_multicast_link_local_scope(
			NetherNet::IPv6AllHostsLinkLocal);

		if (ip6_socket->SetOption(rtc::Socket::Option::OPT_IPV6_JOIN_GROUP, (const void*)&ip6_multicast_link_local_scope, 20)) {
			auto err = ip6_socket->GetError();
			auto errorCode = std::system_error(err, std::system_category());

			NetherNet::NetherNetTransport_LogMessage(
				4,
				"[LAN] setsockopt(IPV6_JOIN_GROUP) failed: %s (%d)",
				errorCode.what(),
				errorCode.code().value());
		}
		
		if (ip6_socket->SetOption(rtc::Socket::Option::OPT_IPV6_MULTICAST_LOOP, 0)) {
			auto err = ip6_socket->GetError();
			auto errorCode = std::system_error(err, std::system_category());
			
			NetherNet::NetherNetTransport_LogMessage(
				4,
				"[LAN] setsockopt(IPV6_MULTICAST_LOOP) failed: %s (%d)",
				errorCode.what(),
				errorCode.code().value());
		}

		return ip6_socket;
	}

	ErrorOr<rtc::PhysicalSocket*, std::system_error> 
	TryCreateBroadcastSocket(rtc::SocketServer* sock_server, uint16_t port) {
		auto try_ip6_socket = TryCreateDualStackIPv6Socket(sock_server, port);
		if (try_ip6_socket.err()) {
			auto gernic_category = std::generic_category;

			//TODO: figure out what error this specify
			int errval = 100;

			try_ip6_socket.error();

			if (!try_ip6_socket.error().code() != errval
				/*&& TODO*/) {

			}
		}
		
		return try_ip6_socket;
	}

	ErrorOr<std::unique_ptr<AesAdapter>, std::error_code> 
	TryCreateEncryptedBroadcastSocket(rtc::SocketServer* sock_server, uint64_t key, uint16_t port) {
		auto socket_create = TryCreateBroadcastSocket(sock_server, port);
		if (socket_create.err()) {
			//return result_error{ socket_create.error() };
		}

		auto aes_adapter = AesAdapter::Create(std::unique_ptr<rtc::Socket>((rtc::Socket*)socket_create.value()));
		if (aes_adapter.err()) {
			return aes_adapter;
		}

		auto set_key = aes_adapter.value()->SetKey(key);
		if (set_key.err()) {
			return result_error{ set_key.error() };
		}
		else {

		}
	}
}