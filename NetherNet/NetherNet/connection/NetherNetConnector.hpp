#pragma once

#ifndef NETHER_NET_CONNECTOR_HPP  // Check if the symbol is not defined
#define NETHER_NET_CONNECTOR_HPP  // Define the symbol to avoid including the file multiple times

#include <mutex>
#include <gsl/gsl>
#include <functional>
#include <system_error>

#include "peer/WebRTCNetworkPeer.hpp"

#include "../network/NetworkID.hpp"
#include "../network/ESendType.hpp"
#include "../network/packets/ESessionError.hpp"

#include "NetherNetTransportFactory.hpp"

class NetworkIdentifier;

namespace NetherNet {
	//TODO
	enum ESignalingEvent {

	};

	//TODO:
	enum SessionState {

	};

	//TODO:
	struct ConnectionDefinition;

	class NetherNetConnector {
	public:
		//TODO:
		NetherNetConnector(NetherNetTransportFactory const& factory /*TODO*/);

		virtual void OnSignalingEvent(NetworkID id, ESignalingEvent event, std::error_code);

		// vIndex: 0 (+0)
		virtual void OnSessionGetConnectionFlags(NetworkID id, uint32_t* flags);

		virtual void OnSessionOpen(NetworkID id);

		// vIndex: 3 (+24)
		virtual void OnSessionClose(NetworkID id, ESessionError err);

		// vIndex: 5 (+40)
		virtual void OnBroadcastResponseReceived(NetworkID id, const void* pData, int size);

		// vIndex: 6 (+48)
		virtual bool OnBroadcastDiscoveryRequestReceivedGetResponse(void* pApplicationData, int* pSize);

		virtual void OnSessionRequested(NetworkID id);

		// vIndex: 11 (+88)
		virtual bool host(ConnectionDefinition const& definition);

		// vIndex: 15 (+120)
		virtual void runEvents();

		// vIndex: 18 (+148)
		virtual uint64_t getNetworkIdentifier() const;

	public:
		gsl::not_null<std::shared_ptr<WebRTCNetworkPeer>>
		_getOrCreatePeer(uint64_t remoteId, std::recursive_mutex const& mutex);

		void _prepareForNewSession();

		void clearPacketData(NetworkID id);

		void closeSessionWithUser(NetworkID id);

		void disconnect();

		bool getSessionState(NetworkID id, SessionState* state);

		bool isConnected(NetworkIdentifier const& ident);

		bool isPacketAvailable(NetworkID id, uint32_t* pcbMessageSize) const;

		bool readPacket(NetworkID id, void* pData, uint32_t cbDest, uint32_t* pcbMessageSize);

		bool sendPacket(NetworkID id, const char* pData, uint32_t cbData, ESendType sendType) const;

		void setBroadcastRequestCallback(std::function<bool(void*, int*)>&& callback);

		void setBroadcastResponseCallback(std::function<void(NetworkID const&, void const*, int)>&& callback);
	};
}

#endif