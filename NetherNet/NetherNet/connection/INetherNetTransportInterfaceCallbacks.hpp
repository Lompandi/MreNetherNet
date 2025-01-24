#pragma once

#include "../network/NetworkID.hpp"
#include "../network/packets/ESessionError.hpp"

namespace NetherNet {
	class INetherNetTransportInterfaceCallbacks {
	public:
		// vIndex: 0 (+0)
		virtual void  OnSessionGetConnectionFlags(NetworkID remoteId, uint32_t* out_flag) = 0;

		// vIndex: 1 (+8)
		virtual bool OnSessionRequested(NetworkID remoteId) = 0;
		
		// vIndex: 2 (+16)
		virtual void  OnSessionOpen(::NetherNet::NetworkID networkId) = 0;

		// vIndex: 3 (+24)
		virtual void OnSessionClose(::NetherNet::NetworkID, ::NetherNet::ESessionError) = 0;

		// vIndex: 4 (+32)
		virtual void OnSpopViolation() = 0;

		// vIndex: 5 (+40)
		virtual void OnBroadcastResponseReceived(::NetherNet::NetworkID, void const*, int) = 0;

		// vIndex: 6 (+48)
		virtual bool OnBroadcastDiscoveryRequestReceivedGetResponse(void*, int*) = 0;
	};
}