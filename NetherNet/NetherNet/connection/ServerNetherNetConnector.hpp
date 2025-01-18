#pragma once

#include "NetherNetConnector.hpp"
#include "NetherNetTransportFactory.hpp"

namespace NetherNet {
	class ServerNetherNetConnector : public NetherNetConnector {
		//TODO
	public:
		ServerNetherNetConnector(NetherNetTransportFactory const& factory /*TODO*/);

		bool host(ConnectionDefinition const& definition) override;

		void OnSessionOpen(NetworkID id) override;

		void OnSessionRequested(NetworkID id) override;


	};
}