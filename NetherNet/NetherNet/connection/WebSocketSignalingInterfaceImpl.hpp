#pragma once

#include "WebSocket.hpp"
#include "IWebRTCSignalingInterface.hpp"

namespace NetherNet {
	class WebSocketSignalingInterfaceImpl : public IWebRTCSignalingInterface {
	public:
		WebSocketSignalingInterfaceImpl();

		void SignOut() override;

		bool IsSignedIn() const override;

	private:
		WebSocket mWebSocket;
		IWebRTCSignalingInterface::ConnectionStatusChangedCallback mConnectionStatusChangedCallback;
		IWebRTCSignalingInterface::ReceiveSignalCallback mReceivedSignalCallback;
		IWebRTCSignalingInterface::PingSignalCallback mPingSignalCallback;
		std::chrono::steady_clock::time_point mStartPingTimer;
		int	mTurnCredentialsExpirationInSeconds;
		std::chrono::steady_clock::time_point mTurnCredentialTimer;
	};
}