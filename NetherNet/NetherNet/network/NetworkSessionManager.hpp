#pragma once

#include "NetworkID.hpp"
#include "signaling/ConnectRequest.hpp"

namespace NetherNet {
	class NetworkSessionManager {
	public:
		void AcceptSessionWithUser(NetworkID& id);

		void ProcessSignal(
			NetworkID remoteId,
			ConnectError const& signal,
			SignalingChannelId id
		);

		void ProcessSignal(
			NetworkID remoteId,
			ConnectResponse const& signal,
			SignalingChannelId id
		);

		void ProcessSignal(
			NetworkID remoteId,
			CandidateAdd const& signal,
			SignalingChannelId id
		);

		void ProcessSignal(
			NetworkID remoteId,
			ConnectRequest const& signal,
			SignalingChannelId id
		);

		bool IsPacketAvailable(NetworkID id, uint32_t* mcbMessage);

		SimpleNetworkInterfaceImpl* mSimpleNetworkInterface;	//this + 0xC0
	};
}