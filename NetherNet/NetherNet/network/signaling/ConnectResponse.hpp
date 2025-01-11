
#include <api/jsep_session_description.h>

namespace NetherNet {

    //TODO
    class ConnectResponse {
    public:
        std::unique_ptr<webrtc::SessionDescriptionInterface> GetSdp() const {
            webrtc::CreateSessionDescription(webrtc::SdpType::kAnswer, mSdp);
        };

        std::string ToString() const;

        static ::NetherNet::ConnectResponse Create(uint64_t sessionId, std::string sdp);
        static std::optional<::NetherNet::ConnectResponse>
            TryParse(std::array<std::string_view, 3> const& tokens);
        static std::string_view const& kIdentifier();
    private:
        uint64_t    mSessionId;     //this + 0x00
        std::string mSdp;           //this + 0x08
    };
} 