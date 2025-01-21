
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#include "HNetherNet.hpp"
#include "encryption/OpenSSLInterface.hpp"
#include <cstdarg>

namespace NetherNet {
	LogSeverity g_LogSeverity = LogSeverity::CriticalOnly;

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
			return result_error{ make_error_code(err) };
		}

		if (EVP_DigestInit_ex(pMdCtx, mHashType, NULL) != 1) {
			auto err = ERR_get_error();
			EVP_MD_CTX_free(pMdCtx);
			return result_error{ make_error_code(err) };
		}

		uint64_t Id = id;
		if (EVP_DigestUpdate(pMdCtx, &Id, 8) != 1) {
			auto err = ERR_get_error();
			EVP_MD_CTX_free(pMdCtx);
			return result_error{ make_error_code(err) };
		}

		if (EVP_DigestFinal(pMdCtx, Key.data(), NULL) != 1) {
			auto err = ERR_get_error();
			EVP_MD_CTX_free(pMdCtx);
			return result_error{ make_error_code(err) };
		}

		EVP_MD_CTX_free(pMdCtx);
		return Key;
	}

	//TODO: finished up this thing
	ErrorOr<std::unique_ptr<::NetherNet::AesContext>, std::error_code> 
	CreateEnvelope(uint64_t id) {
		auto Key = ::NetherNet::CreateKey(id);
		if (!Key.has_value()) {
			return result_error{ Key.error() };
		}
		return std::make_unique<AesContext>(Key.value());
	}

	void NetherNetTransport_LogMessage(NetherNet::LogSeverity serverity, const char* msg, ...) {
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

	void HandleDiscoveryPacketOnSignalThread(rtc::SocketAddress& addr, DiscoveryMessagePacket const& packet) {
		//TODO
	}

	void HandleDiscoveryPacketOnSignalThread(rtc::SocketAddress& addr, DiscoveryRequestPacket const& packet) {
		//TODO
	}

	void HandleDiscoveryPacketOnSignalThread(rtc::SocketAddress& addr, DiscoveryResponsePacket& packet) {
		//TODO
	}
}