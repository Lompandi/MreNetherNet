#pragma once
#include <system_error>

#include "AesAdapter.hpp"

namespace NetherNet {
	class OpenSSLAesAdapter : public AesAdapter {
	public:

		//Success actually return void, but we will make that later
		virtual ErrorOr<void, std::error_code> SetKey(uint64_t id = 0x00000000deadbeef) override;

		virtual int SendTo(void const* pv, uint64_t cb, rtc::SocketAddress const& addr) override;

	private:
		std::unique_ptr<AesContext> mEncryptionKey; //this + 0x150
	};
}