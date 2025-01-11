#pragma once

#include <string>
#include <system_error>
#include <openssl/err.h>

namespace NetherNet {
	class OpenSSL_ErrorCategory : public std::error_category {
	public:
		const char* name() const noexcept override {
			return "OpenSSL";
		}

		std::string message(int ev) const override;
	};

	std::error_code make_error_code(unsigned long openssl_error);
}