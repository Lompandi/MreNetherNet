#pragma once
#include <memory>
#include <functional>

#include "NetworkSystemToggles.hpp"
#include "../network/NetworkID.hpp"
#include "INetherNetTransportInterface.hpp"
#include "INetherNetTransportInterfaceCallbacks.hpp"

namespace NetherNet {
	class NetherNetTransportFactory {
    public:
        NetherNetTransportFactory(NetworkSystemToggles const& toggles) :
            mToggles(toggles) {};

        // vIndex: 0 (+0) TODO
        virtual std::unique_ptr<
            INetherNetTransportInterface,
            std::function<void(INetherNetTransportInterface*)>>
            createTransport(NetworkID const& id, INetherNetTransportInterfaceCallbacks* callbacks)
            const;

    public:
        NetworkSystemToggles mToggles;  //this + 0x8
	};
}