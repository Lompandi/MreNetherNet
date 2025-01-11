#pragma once

#include <api/jsep_ice_candidate.h>

namespace NetherNet {
	//TODO
	class CandidateAdd {
	public:
		webrtc::IceCandidateInterface* GetSdp() const;
		std::string ToString() const;

		static std::optional<::NetherNet::CandidateAdd>
			TryCreate(uint64_t sessionId, ::webrtc::IceCandidateInterface const& rtcCandidate);
		static std::optional<::NetherNet::CandidateAdd> TryParse(std::array<std::string_view, 3> const& tokens);

		//static var
		static std::string_view const& kIdentifier();
	private:
		std::string mSdp;
	};
}