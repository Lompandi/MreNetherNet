
#include "WebSocketSignalingInterfaceImpl.hpp"

namespace NetherNet {
	WebSocketSignalingInterfaceImpl::WebSocketSignalingInterfaceImpl()
		: mWebSocket() {
		mConnectionStatusChangedCallback = nullptr;
		mReceiveSignalCallback = nullptr;
		mPingSignalCallback = nullptr;
		mStartPingTimer = std::chrono::steady_clock::now();
		mTurnCredentialsExpirationInSeconds = 0;
		mTurnCredentialTimer = std::chrono::steady_clock::now();
	}

	void 
	WebSocketSignalingInterfaceImpl::SignOut() {
		mWebSocket.Disconnect();
	}

	bool 
	WebSocketSignalingInterfaceImpl::IsSignedIn() const {
		return mWebSocket.IsConnected();
	}


}