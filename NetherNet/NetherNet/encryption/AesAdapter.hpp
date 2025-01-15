#ifndef AES_ADAPTER_HPP
#define AES_ADAPTER_HPP

#pragma once

#include <memory>
#include <system_error>

#include <rtc_base/socket.h>
#include <rtc_base/async_socket.h>

#include "../utils/Errors.hpp"

namespace NetherNet {
	//TODO
	class AesAdapter : public rtc::AsyncSocketAdapter {
	public:
		AesAdapter(std::unique_ptr<rtc::AsyncSocketAdapter> socket) :
			rtc::AsyncSocketAdapter(socket.get()) {}

		virtual NetherNet::ErrorOr<void, std::error_code> SetKey(uint64_t id) = 0;

		static ErrorOr<std::unique_ptr<AesAdapter>, std::error_code>
			Create(std::unique_ptr<rtc::Socket>&& socket);
	};
}

#endif