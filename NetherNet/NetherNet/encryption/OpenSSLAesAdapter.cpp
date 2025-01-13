
#include "../HNetherNet.hpp"

#include "../utils/Strings.hpp"
#include "OpenSSLAesAdapter.hpp"

namespace NetherNet {
	ErrorOr<void, std::error_code> OpenSSLAesAdapter::SetKey(uint64_t id) {
		//Set encryption key, key normally would be 0xdeadbeef
		auto envelope = ::NetherNet::CreateEnvelope(id);
		if (!envelope.has_value()) {
			return result_error{ envelope.error() };
		}

		mEncryptionKey = std::move(envelope.value());
		return;
	}

	int OpenSSLAesAdapter::SendTo(void const* pv, uint64_t cb, rtc::SocketAddress const& addr) {
		if (!mEncryptionKey) {
			return 0;
		}

		NetherNet::View send_data((uint8_t*)pv, cb);
		auto enc_result = mEncryptionKey->Seal(send_data);

		if (!enc_result.has_value()) {
			return 0;
		}
		auto& enc_data = enc_result.value();

		return SendTo(enc_data.data(), enc_data.get_len(), addr);
	}
}