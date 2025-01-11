
#include "CandidateAdd.hpp"

namespace NetherNet {
	webrtc::IceCandidateInterface* CandidateAdd::GetSdp() const {
		std::string sdp_mid;
		auto ice_candidate = webrtc::CreateIceCandidate(sdp_mid, 0, mSdp, NULL);
		return ice_candidate;
	}


}