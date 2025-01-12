
#include <system_error>
#include <openssl/hmac.h>
#include <openssl/err.h>

#include "AesContext.hpp"
#include "OpenSSLInterface.hpp"

namespace NetherNet {
	size_t AesContext::GetBufferSizeForPlaintextSize(size_t size) {
		auto mDigest = GetDigest();
		size_t digestSize = EVP_MD_size(mDigest);
		return size + digestSize + 2 * EVP_CIPHER_CTX_block_size(mCipherCtx);
	}

	//TODO: check if this is working am figure out the MDs
	ErrorOr<::NetherNet::View, std::error_code>
	AesContext::Open(std::vector<uint8_t>& buffer) {
		auto mDigest = GetDigest();
		int md_size = EVP_MD_size(mDigest);

		if (buffer.size() >= md_size
			&& EVP_CIPHER_CTX_reset(mCipherCtx) == 1) {
			
			auto mCipher = GetCipher();
			if (EVP_DecryptInit_ex(mCipherCtx, mCipher, NULL, mKeyData.data(), mKeyData.data()) != 1) {
				auto err = ERR_get_error();
				return ErrorOr<View, std::error_code>::error(make_error_code(err));
			}

			auto mInputBuffer = buffer.data() + md_size;
			mOutputBuffer.resize(buffer.size());

			int outSize = 0;
			if (EVP_DecryptUpdate(mCipherCtx, (uint8_t*)mOutputBuffer.data(), &outSize, mInputBuffer, buffer.size() - md_size) != 1) {
				auto err = ERR_get_error();
				return ErrorOr<View, std::error_code>::error(make_error_code(err));
			}

			int finalOutSize = 0;
			if (EVP_DecryptFinal_ex(mCipherCtx, mOutputBuffer.data() + outSize, &finalOutSize) != 1) {
				auto err = ERR_get_error();
				return ErrorOr<View, std::error_code>::error(make_error_code(err));
			}

			//PLACEHOLDER 
			uint8_t* unknown_md_1 = nullptr;
			uint8_t* unknown_md_2 = nullptr;

			size_t outputDataLen = outSize + finalOutSize;
			mOutputBuffer.resize(outputDataLen);

			if (HMAC(mDigest, mKeyData.data(), mKeyData.size(), mOutputBuffer.data(), outputDataLen, unknown_md_1, 0)
				/* && !CRYPTO_memcmp(unknown_md_2, unknown_md_1, md_size)*/) {
				
				return ErrorOr<View, std::error_code>::success(
					::NetherNet::View(mOutputBuffer.data(), outputDataLen)
				);
			}
		}

		auto err = ERR_get_error();
		return ErrorOr<View, std::error_code>::error(make_error_code(err));
	}

	ErrorOr<::NetherNet::View, std::error_code>
	AesContext::Seal(NetherNet::View plaintext) {
		if (EVP_CIPHER_CTX_reset(mCipherCtx) == 1) {
			auto mCipher = GetCipher();

			std::vector<uint8_t> outCipherText(16, '\0');
			if (EVP_EncryptInit_ex(mCipherCtx, mCipher, NULL, mKeyData.data(), mKeyData.data()) != 1) {
				auto err = ERR_get_error();
				return ErrorOr<View, std::error_code>::error(make_error_code(err));
			}

			auto mDigest = GetDigest();
			int Md_size = EVP_MD_size(mDigest);

			auto BufferSizeForPlaintext =
				GetBufferSizeForPlaintextSize(plaintext.get_len());

			mOutputBuffer.resize(BufferSizeForPlaintext);

			if (!HMAC(mDigest, mKeyData.data(), mKeyData.size(), plaintext.data(), plaintext.get_len(), mOutputBuffer.data(), 0)) {
				auto err = ERR_get_error();
				return ErrorOr<View, std::error_code>::error(make_error_code(err));
			}

			int outLen = 0;
			int finalOutLen = 0;

			auto encryptDataOffset = mOutputBuffer.data() + Md_size;
			if (EVP_EncryptUpdate(mCipherCtx, encryptDataOffset, &outLen, plaintext.data(), plaintext.get_len()) == 1
				&& EVP_EncryptFinal_ex(mCipherCtx, mOutputBuffer.data() + Md_size + outLen, &finalOutLen) == 1) {

				int totalLength = outLen + finalOutLen;
				return ErrorOr<View, std::error_code>::success(
					::NetherNet::View(mOutputBuffer.data(), Md_size + totalLength)
				);
			}
		}

		auto err = ERR_get_error();
		return ErrorOr<View, std::error_code>::error(make_error_code(err));
	}
}