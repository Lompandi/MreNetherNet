
#include "ServerNetherNetConnector.hpp"

namespace NetherNet {
	bool ServerNetherNetConnector::host(ConnectionDefinition const& definition) {
		_prepareForNewSession();
		return true;
	}

	void ServerNetherNetConnector::OnSessionOpen(NetworkID id) {

	}

	void ServerNetherNetConnector::OnSessionRequested(NetworkID id) {

	}
}