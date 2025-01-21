#pragma once

#include <api/set_remote_description_observer_interface.h>

namespace NetherNet {
    class MySetSessionDescriptionObserver : public webrtc::SetRemoteDescriptionObserverInterface {
    public:
        using OnSuccessCallback = std::function<void()>;
        using OnFailureCallback = std::function<void(webrtc::RTCError const&)>;

        static MySetSessionDescriptionObserver* Create(
            OnSuccessCallback&& onSuccess,
            OnFailureCallback&& onFailure) {
            return new rtc::RefCountedObject<MySetSessionDescriptionObserver>(
                std::move(onSuccess), std::move(onFailure)
            );
        }

    protected:
        MySetSessionDescriptionObserver(
            OnSuccessCallback&& onSuccess,
            OnFailureCallback&& onFailure)
            : onSuccessCallback_(std::move(onSuccess)), onFailureCallback_(std::move(onFailure)) {
        }

        void OnSetRemoteDescriptionComplete(webrtc::RTCError error) override;

    private:
        OnSuccessCallback onSuccessCallback_;
        OnFailureCallback onFailureCallback_;
    };
}