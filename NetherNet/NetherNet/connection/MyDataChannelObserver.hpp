#pragma once

#include "NetworkSession.hpp"

#include <api/peer_connection_interface.h>

namespace NetherNet {
	class MyDataChannelObserver : public ::webrtc::DataChannelObserver {
	public:
        MyDataChannelObserver() = default;

        void OnStateChange() override;
        void OnMessage(::webrtc::DataBuffer const& message) override;
    public:
        MyDataChannelObserver(
            ::NetherNet::NetworkSession* pSession,
            rtc::scoped_refptr<::webrtc::DataChannelInterface> pDataChannel
        ) :
            mpSession(pSession), mChannelInterface(pDataChannel) {}
    private:
        NetworkSession* mpSession;
        rtc::scoped_refptr<webrtc::DataChannelInterface> mChannelInterface;
	};
}