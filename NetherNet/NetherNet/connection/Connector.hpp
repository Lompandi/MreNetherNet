#pragma once
#include <string>

#include "NetherNetConnector.hpp"
#include "DisconnectFailReason.hpp"

class Connector {
public:
	struct ConnectionCallbacks {
	public:
        // vIndex: 1
        virtual bool onNewIncomingConnection(NetworkIdentifier const&, std::shared_ptr<NetworkPeer>&&) = 0;

        // vIndex: 2
        virtual bool onNewOutgoingConnection(NetworkIdentifier const&, std::shared_ptr<NetworkPeer>&&) = 0;

        // vIndex: 3
        virtual void onConnectionClosed(
            NetworkIdentifier const&,
            Connection::DisconnectFailReason const,
            std::string const&,
            bool
        ) = 0;
	};
};