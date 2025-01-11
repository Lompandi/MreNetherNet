
#include "OpenSSLInterface.hpp"

namespace NetherNet {
	std::string OpenSSL_ErrorCategory::message(int ev) const {
		char buffer[120];
		ERR_error_string_n(ev, buffer, sizeof(buffer));
		return std::string(buffer);
	}

	std::error_code make_error_code(unsigned long openssl_error) {
		static OpenSSL_ErrorCategory category;
		return std::error_code(openssl_error, category);
	}
}