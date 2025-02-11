
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
		NetherNet::NetherNetTransport_LogMessage(LogSeverity::Information, "[LAN] starting network discovery");
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

		if (mBindAddress.family() == AF_INET) {
			if (addr.family() == AF_INET6) {
				//Error
			}
		}

		if (mBindAddress.family() != AF_INET6 || addr.family() != AF_INET) {
			rtc::PacketOptions empty_opt;
			auto result = mSocket->SendTo(data, size, addr, empty_opt);
			//Handle error code
		}

		rtc::PacketOptions empty_opt;
		auto& ipaddr = addr.ipaddr();
		auto ip6addr = ipaddr.AsIPv6Address();
		uint16_t port = addr.port();
		rtc::SocketAddress dst_addr(ip6addr, port);
		auto send_result = mSocket->SendTo(data, size, dst_addr, empty_opt);
		//TODO
	}

	bool LanThreadManager::IsBroadcastDiscoveryEnabled(NetworkID id) {
		if (IsCurrent())
			return (mIds.contains(id));

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
		if (!mDiscoveredAddresses.empty()) {
			for (const auto& ip : mDiscoveredAddresses) {
				NetherNet::NetherNetTransport_LogMessage(
					NetherNet::LogSeverity::Information,
					"[LAN] [%llu] sending broadcast request to %s",
					id,
					ip.ToString().c_str());

				auto length = disc_req_pkt.PacketLength();
				//TODO
				rtc::SocketAddress send_addr(ip, mEphemeralPort);
				auto send_result = SendToHelper(&disc_req_pkt, length, send_addr);
				auto value = send_result.value();
				auto category = &send_result.category();
				if (!value) {
					//goto increment
				}
				NetherNet::NetherNetTransport_LogMessage(
					NetherNet::LogSeverity::Warning,
					"[LAN] sendto %s failed: %s (%d)",
					send_addr.ToString().c_str(),
					send_result.message().c_str(),
					value);
				//TODO
			}
		}
	}

	void LanThreadManager::OnPacket(rtc::AsyncPacketSocket* socket, const char* data, uint64_t size, rtc::SocketAddress const& addr, int64_t const& id2) {

		auto packet_hdr = (DiscoveryPacketHeader*)data;
		if (size >= 4 && size >= packet_hdr->PacketLength()) {
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
			
			for (const auto& addr : mIds) {
				SendLanBroadcastRequest(addr);
			}
			PostTask([&]() {
				BroadcastTask();
			});
		}
		else {
			NetherNet::NetherNetTransport_LogMessage(LogSeverity::Information, "[LAN] Stopping broadcast task, no more ids");
			//TODO
		}
	}

	void LanThreadManager::EnableBroadcastDiscovery(NetworkID id) {
		if (IsCurrent()) {
			NetherNet::NetherNetTransport_LogMessage(LogSeverity::Information, "[LAN] adding [%llu] to set of broadcast ids", id);
			if (!false /*TODO*/ ) {
				NetherNet::NetherNetTransport_LogMessage(LogSeverity::Information, "[LAN] starting broadcast task");

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
		if (!IsCurrent()) {
			PostTask([&]() {DisableBroadcastDiscovery(id); });
			return;
		}
		NetherNet::NetherNetTransport_LogMessage(LogSeverity::Information, "[LAN] removing [%llu] from set of broadcast ids", id);
		mIds.erase(id);
	}

	//IPv6 multicast address with link-local scope (FF02::1)
	rtc::IPAddress IPv6AllHostsLinkLocal(multi_cast_link_local);

	void LanThreadManager::OnNetworkDiscoveryComplete() {
		auto network_list = mBasicNetworkMgr->GetAnyAddressNetworks();
		
		std::set<rtc::IPAddress> broadcast_domain_list;

		for (const auto& network : network_list) {
			for (const auto& ip : network->GetIPs()) {
				if (!rtc::IPIsLoopback(ip)) {
					auto family = ip.family();
					if (family == AF_INET) {
						if (!rtc::IPIsLinkLocal(ip)) {
							auto ip6_flags = ip.ipv6_flags();
							in_addr ip4_addr;
							ip4_addr.S_un.S_addr = ip.ipv4_address().S_un.S_addr;
							rtc::IPAddress addr_add(_byteswap_ulong(ip4_addr.S_un.S_addr) | ~(-1 << -(char)ip6_flags));
							NetherNet::NetherNetTransport_LogMessage(
								NetherNet::LogSeverity::Information,
								"[LAN] adding %s to broadcast domain",
								addr_add.ToString().c_str());
							broadcast_domain_list.insert(addr_add);

						}
					}
					else if (family == AF_INET6 && mBindAddress.family() == AF_INET6) {
						NetherNet::NetherNetTransport_LogMessage(
							NetherNet::LogSeverity::Information,
							"[LAN] adding %s to broadcast domain",
							IPv6AllHostsLinkLocal.ToString().c_str());
						broadcast_domain_list.insert(IPv6AllHostsLinkLocal);
					}
					NetherNet::NetherNetTransport_LogMessage(
						NetherNet::LogSeverity::Information,
						"[LAN] %s not viable for broadcast domain",
						ip.ToString().c_str());
				}
			}

			if(broadcast_domain_list.empty())
				NetherNet::NetherNetTransport_LogMessage(
					NetherNet::LogSeverity::Warning,
					"[LAN] no viable networks found, we're likely offline");

			NetherNet::NetherNetTransport_LogMessage(NetherNet::LogSeverity::Information, "[LAN] network discovery complete!");
			std::swap(mDiscoveredAddresses, broadcast_domain_list);
			broadcast_domain_list.clear();
			/*

					if (family == AF_INET6) {
						if (mNetworkFamily != AF_INET6) {
							//goto process
						}

						auto addr_str = IPv6AllHostsLinkLocal.ToString();
						NetherNet::NetherNetTransport_LogMessage(LogSeverity::Information, "[LAN] adding %s to broadcast domain", addr_str.data());


					}
					else {
						if (family != AF_INET || rtc::IPIsLinkLocal(ip)) {
							auto addr_str = ip.ToString();
							NetherNet::NetherNetTransport_LogMessage(LogSeverity::Information, "[LAN] %s not viable for broadcast domain", addr_str.data());
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

						NetherNet::NetherNetTransport_LogMessage(LogSeverity::Information, "[LAN] adding %s to broadcast domain", addr_str.data());

						//TODO
					}
				}
			}*/
		}
	}

	void LanThreadManager::SendSignalingMessageTo(NetworkID src, NetworkID dst, std::string const& data, std::function<void(std::error_code)>&& on_complete) {
		if (IsCurrent()) {
			rtc::SocketAddress addr;
			if (mPeerRecordTable->Find(dst, &addr)) {
				auto addr_str = addr.ToString();
				NetherNet::NetherNetTransport_LogMessage(
					LogSeverity::Information,
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
				NetherNet::NetherNetTransport_LogMessage(LogSeverity::Error, "[%llu] signaling message to [%llu] dropped, unknown peer", src, dst);
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

			NetherNet::NetherNetTransport_LogMessage(LogSeverity::Information, "[LAN] socket failed: %s (%d)", code.what(), code.code().value());
			return result_error{ code };
		}

		auto set_opt_res = ip6_socket->SetOption(rtc::Socket::Option::OPT_IPV6_V6ONLY, false);
		if (set_opt_res) {
			auto errCode = ip6_socket->GetError();
			auto err = std::system_error(errCode, std::system_category());

			NetherNet::NetherNetTransport_LogMessage(
				LogSeverity::Information,
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
				LogSeverity::Information,
				"[LAN] setsockopt(SO_BROADCAST) failed: %s (%d)",
				errorCode.what(),
				errorCode.code().value());

			return result_error{ errorCode };
		}

		if (ip6_socket->SetOption(rtc::Socket::Option::OPT_RCVBUF, 0x8000)) {
			auto err = ip6_socket->GetError();
			auto errorCode = std::system_error(err, std::system_category());

			NetherNet::NetherNetTransport_LogMessage(
				LogSeverity::Information,
				"[LAN] setsockopt(SO_RCVBUF) failed: %s (%d)",
				errorCode.what(),
				errorCode.code().value());
		}
		
		rtc::SocketAddress sock_addr(NetherNet::IPv6Any, port);
		if (ip6_socket->Bind(sock_addr)) {
			auto err = ip6_socket->GetError();
			auto errorCode = std::system_error(err, std::system_category());

			NetherNet::NetherNetTransport_LogMessage(LogSeverity::Information, "[LAN] bind failed: %s (%d)", errorCode.what(), errorCode.code().value());
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
				LogSeverity::Information,
				"[LAN] setsockopt(IPV6_JOIN_GROUP) failed: %s (%d)",
				errorCode.what(),
				errorCode.code().value());
		}
		
		if (ip6_socket->SetOption(rtc::Socket::Option::OPT_IPV6_MULTICAST_LOOP, 0)) {
			auto err = ip6_socket->GetError();
			auto errorCode = std::system_error(err, std::system_category());
			
			NetherNet::NetherNetTransport_LogMessage(
				LogSeverity::Information,
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