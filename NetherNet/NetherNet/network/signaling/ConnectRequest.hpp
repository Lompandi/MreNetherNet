#pragma once

#include <api/jsep_session_description.h>

namespace NetherNet{

	//TODO
	class ConnectRequest {
	public:
		std::unique_ptr<webrtc::SessionDescriptionInterface> GetSdp() const
		{
			return webrtc::CreateSessionDescription(webrtc::SdpType::kOffer, mSdp);
		};

		std::string ToString();

		static ::NetherNet::ConnectRequest Create(uint64_t sessionId, std::string sdp);
		//basically the login packet reimplementation
		static std::optional<::NetherNet::ConnectRequest> TryParse(std::array<std::string_view, 3> const& tokens);
		static std::string_view const& kIdentifier();
	private:
		std::string mSdp; //this + 0x08
	};	
}