
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
		NetherNet::View enc_data = enc_result.value();
		return rtc::AsyncSocketAdapter::SendTo(enc_data.data(), enc_data.get_len(), addr);
	}

	int OpenSSLAesAdapter::Send(void const* pv, uint64_t cb) {
		if (!mEncryptionKey)
			return 0;

		NetherNet::View send_data((uint8_t*)pv, cb);
		auto enc_result = mEncryptionKey->Seal(send_data);

		if (!enc_result.has_value())
			return 0;
		NetherNet::View enc_data = enc_result.value();
		return rtc::AsyncSocketAdapter::Send(enc_data.data(), enc_data.get_len());

	}

	int OpenSSLAesAdapter::Recv(void* pv, uint64_t cb, int64_t* timestamp) {
		int recv_size = rtc::AsyncSocketAdapter::Recv(pv, cb, timestamp);
		if (recv_size < 0)
			return -1;

		if (!mEncryptionKey)
			return 0;

		NetherNet::View recv_data((uint8_t*)pv, recv_size);
		auto decrypt_result = mEncryptionKey->Open(recv_data);
		if (!decrypt_result.has_value())
			return 0;

		NetherNet::View decrypted_data = decrypt_result.value();
		std::memcpy(pv, decrypted_data.data(), decrypted_data.get_len());
		return decrypted_data.get_len();
	}

	int OpenSSLAesAdapter::RecvFrom(void* pv, uint64_t cb, rtc::SocketAddress* paddr, int64_t* timestamp) {
		int max_size = 512;
		if ((cb / 4) < 512)
			max_size = cb / 4;
		int pad_offset = 4 * max_size;

		//fill up the buffer
		uint8_t* buffer = (uint8_t*)pv;
		auto pEnd = &buffer[pad_offset];
		auto fill_size = pad_offset / 4;
		if (pv > pEnd)
			fill_size = 0;
		if (fill_size) {
			uint8_t* pfill = buffer;
			for (int i = fill_size; i; --i) {
				*pfill = 0xBAADF00D;
				pfill += 4;
			}
		}

		auto recv_size = rtc::AsyncSocketAdapter::RecvFrom(pv, cb, paddr, timestamp);
		if (recv_size < 0)
			return -1;

		if (!mEncryptionKey)
			return 0;

		NetherNet::View envelope((uint8_t*)pv, recv_size);
		auto decrypt_result = mEncryptionKey->Open(envelope);
		if (!decrypt_result.has_value())
			return 0;

		NetherNet::View decrypted_data = decrypt_result.value();
		std::memcpy(pv, decrypted_data.data(), decrypted_data.get_len());
		return decrypted_data.get_len();
	}
}