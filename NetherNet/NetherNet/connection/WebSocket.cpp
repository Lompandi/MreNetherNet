
#include "WebSocket.hpp"

namespace NetherNet {
	bool WebSocket::IsConnected() {
		std::lock_guard<std::mutex> lock(mOperationGuard);
		return mWebSocketHandle != nullptr;
	}

	void WebSocket::Send(std::string const& message, std::function<void(std::error_code)>&& onComplete) {
		std::lock_guard<std::mutex> lock(mOperationGuard);

		HCWebsocketHandle dupHandle = HCWebSocketDuplicateHandle(mWebSocketHandle);
		if (!dupHandle) {
			if(!onComplete)
				return;
			//make error code
			std::error_code err;
			if (onComplete) {
				onComplete(err);
				return;
			}
		}

		auto async_block = std::make_unique<XAsyncBlock>(0);
		async_block->context = &onComplete;
		//TODO: setting up callback lambda 
		// some more invoking logic

		auto send_result = HCWebSocketSendMessageAsync(dupHandle, message.data(), async_block.get());
		if (send_result < 0) {
			if (!onComplete) {
				//Error handling
			}

			//make error code
			std::error_code err;
			onComplete(err);
			//process buffers
			return;
		}

		//No error and invoke the callback
		if (onComplete) {
			//onComplete(0); TODO
		}

		if (dupHandle)
			HCWebSocketCloseHandle(dupHandle);
	}

	void WebSocket::DeallocateSocketAsync(HCWebsocketHandle socket) {

	}

	void WebSocket::Disconnect() {
		std::lock_guard<std::mutex> lock(mOperationGuard);

		//TODO: Clear data??

		if (mWebSocketHandle) {
			HCWebSocketDisconnect(mWebSocketHandle);
			WebSocket::DeallocateSocketAsync(mWebSocketHandle);
		}
	}
}