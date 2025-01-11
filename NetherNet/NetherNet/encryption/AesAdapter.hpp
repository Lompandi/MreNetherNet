#pragma once

#include <memory>
#include <system_error>

#include <rtc_base/socket.h>
#include <rtc_base/async_socket.h>

#include "../types/Errors.hpp"

namespace NetherNet {
	//TODO
	class AesAdapter : public rtc::AsyncSocketAdapter {
	public:
		virtual NetherNet::ErrorOr<int, std::error_code> SetKey(uint64_t id) = 0;

		static ErrorOr<std::unique_ptr<AesAdapter>, std::error_code>
			Create(std::unique_ptr<rtc::Socket>&& socket);
	};
}