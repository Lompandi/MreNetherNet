#pragma once
#include <system_error>

#include "AesAdapter.hpp"
#include "../types/Errors.hpp"

namespace NetherNet {
	class OpenSSLAesAdapter : public AesAdapter {
	public:

		//Success actually return void, but we will make that later
		virtual ErrorOr<int, std::error_code> SetKey(uint64_t id = 0x00000000deadbeef) override;
		virtual int SendTo(void const* pv, uint64_t cb, rtc::SocketAddress const& addr) override;

	private:
		std::unique_ptr<AesContext> mEncryptionKey; //this + 0x150
	};
}