#pragma once

#include <windows.h>

#include <mutex>
#include <functional>

#include "httpClient/httpClient.h"

namespace NetherNet {
	class WebSocket {
		
		void Disconnect();
		bool IsConnected();
		void Send(std::string const& message, std::function<void(std::error_code)>&& onComplete);

		static void DeallocateSocketAsync(HCWebsocketHandle handle);

	private:
		std::mutex	mOperationGuard;		//this+0x18
		HCWebsocketHandle mWebSocketHandle;		//this+0x68

	};
}