#pragma once
#include <system_error>

#include "AesContext.hpp"
#include "AesAdapter.hpp"

namespace NetherNet {
	class OpenSSLAesAdapter : public AesAdapter {
	public:

		OpenSSLAesAdapter(std::unique_ptr<rtc::AsyncSocketAdapter> socket)
			: AesAdapter(std::move(socket)){ }

		//Success actually return void, but we will make that later
		ErrorOr<void, std::error_code> SetKey(uint64_t id = 0x00000000deadbeef) override;

		int Send(void const* pv, uint64_t cb) override;

		int SendTo(void const* pv, uint64_t cb, rtc::SocketAddress const& addr) override;

		int Recv(void* pv, uint64_t cb, int64_t* timestamp) override;

		int RecvFrom(void* pv, uint64_t cb, rtc::SocketAddress* paddr, int64_t* timestamp) override;

	private:
		std::unique_ptr<AesContext> mEncryptionKey; //this + 0x150
	};
}