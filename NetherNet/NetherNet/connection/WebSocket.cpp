
#include "WebSocket.hpp"

namespace NetherNet {
	bool WebSocket::IsConnected() const {
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

		auto async_block = std::make_unique<XAsyncBlock>();
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
		if (HCWebSocketGetEventFunctions(socket, nullptr, nullptr, nullptr, nullptr) >= 0) {
			HCWebSocketCloseHandle(socket);
		}
	} 

	long WebSocket::DeallocateSocket(HCWebsocketHandle handle) {
		if(!handle)
			return E_INVALIDARG;

		HCWebSocketMessageFunction msgFunc = nullptr;
		HCWebSocketBinaryMessageFunction binMsgFunc = nullptr;
		HCWebSocketCloseEventFunction closeEventFunc = nullptr;

		auto fetch_result = HCWebSocketGetEventFunctions(handle, &msgFunc, &binMsgFunc, &closeEventFunc, nullptr);
		if (fetch_result >= 0) {
			auto close_result = HCWebSocketCloseHandle(handle);
			return close_result;
		}

		return fetch_result;
	}

	void WebSocket::Disconnect() {
		mOperationGuard.lock();
		auto m_handle = mWebSocketHandle;
		mWebSocketHandle = nullptr;
		mOperationGuard.unlock();

		if (m_handle) {
			HCWebSocketDisconnect(mWebSocketHandle);
			WebSocket::DeallocateSocketAsync(mWebSocketHandle);
		}
	}

	HCWebsocketHandle WebSocket::AllocateSocket() {
		//TODO
	}

	//Second value: pair<header name, header value>
	void WebSocket::Connect(std::string const& uri, std::vector<std::pair<std::string, std::string>> const& headers, std::function<void(std::error_code)>&& onComplete) {
		mWebSocketHandle = AllocateSocket();
		if (!mWebSocketHandle) {
			if (!onComplete)
				return;
			//TODO: make error code
			std::error_code err;
			onComplete(err);
			return;
		}

		if (headers.empty()) {

		}
		else {
			for (auto& hdr : headers) {
				auto result = HCWebSocketSetHeader(mWebSocketHandle, hdr.first.data(), hdr.second.data());
				if (result < 0)
					break;
			}
		}
	}
}