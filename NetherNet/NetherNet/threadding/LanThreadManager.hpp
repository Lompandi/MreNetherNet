#pragma once

#include <set>
#include <memory>
#include <chrono>
#include <rtc_base/network.h>
#include <rtc_base/network/sent_packet.h>
#include <rtc_base/async_packet_socket.h>
#include <rtc_base/async_udp_socket.h>
#include <api/jsep.h>
#include <httpClient/httpClient.h>
#include <api/transport/network_types.h>
#include <rtc_base/physical_socket_server.h>

#include "../utils/Errors.hpp"
#include "../utils/Utils.hpp"
#include "../network/NetworkID.hpp"
#include "../network/PeerRecordTable.hpp"
#include "../encryption/OpenSSLAesAdapter.hpp"

namespace {
	const char* TryMakePacketStorage(const char* data);
	NetherNet::ErrorOr<rtc::PhysicalSocket*, std::system_error> TryCreateDualStackIPv6Socket(rtc::SocketServer* sock_server, uint16_t port);
	NetherNet::ErrorOr<rtc::PhysicalSocket*, std::system_error> TryCreateBroadcastSocket(rtc::SocketServer* sock_server, uint16_t port);
	NetherNet::ErrorOr<std::unique_ptr<NetherNet::AesAdapter>, std::error_code> TryCreateEncryptedBroadcastSocket(rtc::SocketServer* sock_server, uint64_t key, uint16_t port);
}

namespace NetherNet {

	extern in6_addr multi_cast_link_local;

	class LanThreadManager : public rtc::Thread,
							 public sigslot::has_slots<sigslot::single_threaded> {
	public:
		LanThreadManager() = default;
		void BeginNetworkDiscovery();
		void BroadcastTask();
		void CreateEncryptedBroadcastSocket();
		void DestroyLanResources();
		void DisableBroadcastDiscovery(NetworkID id);
		void EnableBroadcastDiscovery(NetworkID id);
		void Initialize(const char* thread_name, ThreadInit const& init, uint64_t id, uint16_t port, std::chrono::milliseconds interval);
		bool IsBroadcastDiscoveryEnabled(NetworkID id);
		bool IsNetworkIdOnLan(NetworkID id);
		void OnNetworkDiscoveryComplete();
		void OnPacket(rtc::AsyncPacketSocket* socket, const char* data, uint64_t id, rtc::SocketAddress const& addr, int64_t const& id2);
		void SendLanBroadcastRequest(NetherNet::NetworkID id);
		void SendLanBroadcastResponse(rtc::SocketAddress const& addr, DiscoveryResponsePacket const& packet);
		void SendSignalingMessageTo(NetworkID src, NetworkID dst, std::string const& data, std::function<void(std::error_code)>&& on_complete);
		std::error_code SendToHelper(
			void const* data,
			size_t size,
			rtc::SocketAddress const& addr);
		bool ShouldBeginNetworkDiscovery();
		void Shutdown();
		void Suspend();
	private:
		NetworkID									mRemoteId;			//this + 0x8
		rtc::SocketServer*							mSocketServer;		//this + 0x88
		PeerRecordTable*							mPeerRecordTable;	//this + 0x120
		rtc::AsyncPacketSocket*						mSocket;			//this + 0x180
		rtc::SocketAddress							mSocketAddress;		//this + 0x188
		int											mNetworkFamily;
		rtc::SocketAddress							mBindAddress;		//this + 0x1B0
		std::unique_ptr<rtc::BasicNetworkManager>	mBasicNetworkMgr;	//this + 0x1D8
		uint64_t									mEncryptionkey;		//this + 0x1E0
		uint16_t									mEphemeralPort;		//this + 0x1E8 default port: 7551
		std::set<NetworkID>							mIds;				//this + 0x1F8
		//TODO: std::set down here
		std::set<rtc::IPAddress>					mDiscoveredAddresses; //this + 0x208
		int											mConnPort;			//this + 0xF40
	};

	LanThreadManager& getLanThread();

	extern LanThreadManager* g_LanThread;
}
