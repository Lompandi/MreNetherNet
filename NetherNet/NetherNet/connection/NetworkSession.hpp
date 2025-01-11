#pragma once

#include <api/peer_connection_interface.h>

#include "../network/NetworkID.hpp"
#include "SimpleNetworkInterfaceImpl.hpp"


namespace NetherNet {
	enum ESessionError;
	enum ESendType;

	class NetworkSession : public webrtc::PeerConnectionObserver {
	public:
		enum class ENegotiationState : int {
			None = 0,
			WaitingForResponse = 1,
			WaitingForAccept = 2,
			ICEProcessing = 3,
		};

		void UpdateSessionActivity();

		void AcceptSession();
		void ApplyConnectionFlags(webrtc::PeerConnectionInterface::RTCConfiguration* config, uint32_t flag);
		void SendPacket(const char* pdata, uint32_t cbdata, ::NetherNet::ESendType send_type);
		void TrySendSessionResponse();

	private:
		//Exact type pending
		ENegotiationState	mConnNegotiationState;	//this + 0x08
		NetworkID			mConnectionId;			//this + 0x20
		NetworkID			mRemoteID;				//this + 0x28
		uint32_t			mConnectionFlag;		//this + 0x208
	};

	enum ESessionError {

	};

	enum ESendType {

	};
}