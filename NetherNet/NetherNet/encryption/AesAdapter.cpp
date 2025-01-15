
#include "AesAdapter.hpp"
#include "OpenSSLAesAdapter.hpp"

namespace NetherNet {
	ErrorOr<std::unique_ptr<AesAdapter>, std::error_code>
		AesAdapter::Create(std::unique_ptr<rtc::Socket>&& socket) {
		auto adapter = std::make_unique<OpenSSLAesAdapter>(std::move(socket));

		return std::move(static_cast<std::unique_ptr<AesAdapter>>(std::move(adapter)));
	}
}