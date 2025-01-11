#pragma once

#include <cstdint>
#include <string>
#include <string_view>

#include "../packets/ESessionError.hpp"
#include <optional>

namespace NetherNet {
	//TODO
	class ConnectError {
	public:
		ConnectError() noexcept = default;

		std::string ToString();

		static ConnectError Create(uint64_t sessionId, ESessionError err);
		static std::optional<::NetherNet::ConnectError> TryParse(std::array<std::string_view, 3> const& tokens);

		//static varible
		static ::std::string_view const& kIdentifier();
	private:
		uint64_t		mSessionId; //this + 0x0
		ESessionError	mErr;		//this + 0x8
	};
}