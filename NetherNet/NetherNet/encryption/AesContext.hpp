#pragma once

#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <vector>
#include <string>

#include "../types/Errors.hpp"
#include "../types/Strings.hpp"

namespace NetherNet {
	class AesContext {

	public:
		AesContext(std::vector<uint8_t>&& key) : mKeyData(key) {
			mCipherCtx = EVP_CIPHER_CTX_new();
		}

		AesContext(std::vector<uint8_t>& key) : mKeyData(key) {
			mCipherCtx = EVP_CIPHER_CTX_new();
		}

		[[nodiscard]] auto GetCipher() { return EVP_aes_256_ecb(); }
		[[nodiscard]] auto GetDigest() { return EVP_sha256(); }

		[[nodiscard]] size_t	GetBufferSizeForPlaintextSize(size_t plaintext_size);

		ErrorOr<::NetherNet::View, std::error_code>	Open(std::vector<uint8_t>& ciphertext);
		ErrorOr<::NetherNet::View, std::error_code>	Seal(NetherNet::View plaintext);

	private:
		std::vector<uint8_t>	mKeyData;
		std::vector<uint8_t>	mOutputBuffer;

		EVP_CIPHER_CTX* mCipherCtx;
	};
}