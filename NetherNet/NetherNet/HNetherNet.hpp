#pragma once

#include <vector>
#include <memory>
#include <system_error>
#include <variant>

#include "utils/Errors.hpp"
#include "encryption/AesContext.hpp"

#include "threadding/RtcThreadManager.hpp"

#include "network/signaling/ConnectRequest.hpp"
#include "network/signaling/ConnectResponse.hpp"
#include "network/signaling/ConnectError.hpp"
#include "network/signaling/CandidateAdd.hpp"

#include "logging/LogSeverity.hpp"

#include "network/packets/DiscoveryMessagePacket.hpp"
#include "network/packets/DiscoveryRequestPacket.hpp"
#include "network/packets/DiscoveryResponsePacket.hpp"

namespace NetherNet {
	ErrorOr<std::vector<uint8_t>, std::error_code> CreateKey(uint64_t id);
	ErrorOr<std::unique_ptr<::NetherNet::AesContext>, std::error_code> CreateEnvelope(uint64_t id);

	void NetherNetTransport_LogMessage(NetherNet::LogSeverity a1, const char* a2, ...);
	
	std::optional<std::variant<
		::NetherNet::ConnectRequest,
		::NetherNet::ConnectResponse,
		::NetherNet::ConnectError,
		::NetherNet::CandidateAdd>> 
	TryParseSignalingMessage(const std::string& message);

	void HandleDiscoveryPacketOnSignalThread(rtc::SocketAddress& addr, DiscoveryMessagePacket const& packet);
	void HandleDiscoveryPacketOnSignalThread(rtc::SocketAddress& addr, DiscoveryRequestPacket const& packet);
	void HandleDiscoveryPacketOnSignalThread(rtc::SocketAddress& addr, DiscoveryResponsePacket& packet);

	extern LogSeverity g_LogServerity;
}