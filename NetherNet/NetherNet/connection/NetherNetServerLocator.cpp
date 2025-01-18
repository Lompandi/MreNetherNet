
#include "NetherNetServerLocator.hpp"

namespace NetherNet {
	std::vector<PingedCompatibleServer> 
	NetherNetServerLocator::getServerList() {
		std::lock_guard<std::mutex> lock(mLockGuard);

		std::vector<PingedCompatibleServer> server_list = mServerList;
		return server_list;
	}

	void NetherNetServerLocator::update() {
		if (mUnk1 != 1)
			_pruneStaleServers(std::chrono::seconds(5));
	}

	void NetherNetServerLocator::_cacheDiscoveryResponseData(ServerData const& data) {
		//TODO
	}

	void NetherNetServerLocator::_onDiscoveryResponse(uint64_t id, gsl::span<const char> response_data) {
		//TODO
	}

	void NetherNetServerLocator::_pruneStaleServers(std::chrono::seconds stale_after) {
		//TODO
	}

	void NetherNetServerLocator::_setIsAnnouncing(bool is_announcing) {
		//TODO
	}

	void NetherNetServerLocator::_setIsDiscovering(bool is_discovering) {
		//TODO
	}

	void NetherNetServerLocator::setNetherNetConnector(Bedrock::NonOwnerPointer<NetherNetConnector>&& connector) {
		//TODO
	}

	PingedCompatibleServer NetherNetServerLocator::_transformFrom(uint64_t id, ServerData&& data) {
		//TODO
	}
}