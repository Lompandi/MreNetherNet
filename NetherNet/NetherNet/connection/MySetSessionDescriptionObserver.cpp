
#include "MySetSessionDescriptionObserver.hpp"

namespace NetherNet {
    void 
    MySetSessionDescriptionObserver::OnSetRemoteDescriptionComplete(webrtc::RTCError error) {
        if (!error.ok()) {
            if (onFailureCallback_)
                onFailureCallback_(error);
            return;
        }
        if (onSuccessCallback_)
            onSuccessCallback_();
    }
}