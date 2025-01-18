
#include "NetherNetConnector.hpp"

namespace NetherNet {
	gsl::not_null<std::shared_ptr<WebRTCNetworkPeer>>
	NetherNetConnector::_getOrCreatePeer(uint64_t remoteId, std::recursive_mutex const& mutex) {
		//TODO
	}

	void NetherNetConnector::_prepareForNewSession() {
		//TODO
	}

	void NetherNetConnector::clearPacketData(NetworkID id) {
		//TODO
	}

	void NetherNetConnector::closeSessionWithUser(NetworkID id) {
		//TODO
	}

	void NetherNetConnector::disconnect() {
		//TODO
	}

	bool NetherNetConnector::getSessionState(NetworkID id, SessionState* state) {
		//TODO
	}

	bool NetherNetConnector::isConnected(NetworkIdentifier const& ident) {
		//TODO
	}

	bool NetherNetConnector::isPacketAvailable(NetworkID id, uint32_t* pcbMessageSize) const {
		//TODO
	}

	bool NetherNetConnector::readPacket(NetworkID id, void* pData, uint32_t cbDest, uint32_t* pcbMessageSize) {
		//TODO
	}

	bool NetherNetConnector::sendPacket(NetworkID id, const char* pData, uint32_t cbData, ESendType sendType) const {
		//TODO
	}

	void NetherNetConnector::setBroadcastRequestCallback(std::function<bool(void*, int*)>&& callback) {
		//TODO
	}

	void NetherNetConnector::setBroadcastResponseCallback(std::function<void(NetworkID const&, void const*, int)>&& callback) {
		//TODO
	}


}