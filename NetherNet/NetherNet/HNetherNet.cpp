
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#include "HNetherNet.hpp"
#include "encryption/OpenSSLInterface.hpp"
#include <cstdarg>

namespace NetherNet {
	int g_LogSeverity = 1;

	ErrorOr<std::vector<uint8_t>, std::error_code> 
	CreateKey(uint64_t id) {
		auto mHashType = EVP_sha256();
		int Md_size = EVP_MD_size(mHashType);

		std::vector<uint8_t> Key;

		if(Md_size)
			Key.resize(Md_size);

		EVP_MD_CTX* pMdCtx= EVP_MD_CTX_new();
		if (!pMdCtx) {
			auto err = ERR_get_error();
			EVP_MD_CTX_free(pMdCtx);
			return ErrorOr<std::vector<uint8_t>, std::error_code>::error(
				make_error_code(err)
			);
		}

		if (EVP_DigestInit_ex(pMdCtx, mHashType, NULL) != 1) {
			auto err = ERR_get_error();
			EVP_MD_CTX_free(pMdCtx);
			return ErrorOr<std::vector<uint8_t>, std::error_code>::error(
				make_error_code(err)
			);
		}

		uint64_t Id = id;
		if (EVP_DigestUpdate(pMdCtx, &Id, 8) != 1) {
			auto err = ERR_get_error();
			EVP_MD_CTX_free(pMdCtx);
			return ErrorOr<std::vector<uint8_t>, std::error_code>::error(
				make_error_code(err)
			);
		}

		if (EVP_DigestFinal(pMdCtx, Key.data(), NULL) != 1) {
			auto err = ERR_get_error();
			EVP_MD_CTX_free(pMdCtx);
			return ErrorOr<std::vector<uint8_t>, std::error_code>::error(
				make_error_code(err)
			);
		}

		EVP_MD_CTX_free(pMdCtx);
		return ErrorOr<std::vector<uint8_t>, std::error_code>::success(
			Key
		);
	}

	//TODO: finished up this thing
	ErrorOr<std::unique_ptr<::NetherNet::AesContext>, std::error_code> 
	CreateEnvelope(uint64_t id) {
		auto Key = ::NetherNet::CreateKey(id);
		if (Key.is_error()) {
			return ErrorOr<std::unique_ptr<AesContext>, std::error_code>::error(Key.error());
		}
		return ErrorOr<std::unique_ptr<AesContext>, std::error_code>::error(Key.error());
	}

	void NetherNetTransport_LogMessage(int serverity, const char* msg, ...) {
		va_list va;
		va_start(va, msg);

	}

	std::optional<std::variant<
		::NetherNet::ConnectRequest,
		::NetherNet::ConnectResponse,
		::NetherNet::ConnectError,
		::NetherNet::CandidateAdd>>
	TryParseSignalingMessage(const std::string& message) {

	}
}