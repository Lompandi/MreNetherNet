
#include "ConnectResponse.hpp"

namespace NetherNet {
    ::NetherNet::ConnectResponse  ConnectResponse::Create(uint64_t sessionId, std::string sdp) {
        ConnectResponse response;
        response.mSessionId = sessionId;
        response.mSdp = sdp;

        return response;
    }
}