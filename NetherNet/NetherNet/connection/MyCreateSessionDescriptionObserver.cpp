
#include "MyCreateSessionDescriptionObserver.hpp"

namespace NetherNet {
	void  
	MyCreateSessionDescriptionObserver::OnFailure(webrtc::RTCError err) {
		if (mErrCallback)
			mErrCallback(err);
	}

	void 
	MyCreateSessionDescriptionObserver::OnSuccess(webrtc::SessionDescriptionInterface* iface) {
		if (mSuccessCallback)
			mSuccessCallback(iface);
	}

	MyCreateSessionDescriptionObserver
	MyCreateSessionDescriptionObserver::Create(std::function<void(webrtc::SessionDescriptionInterface*)>&& on_success, std::function<void(webrtc::RTCError const&)>&& on_error) {
		MyCreateSessionDescriptionObserver observer;

		observer.mSuccessCallback = on_success;
		observer.mErrCallback = on_error;
	}

	webrtc::RefCountReleaseStatus
	MyCreateSessionDescriptionObserver::Release() const {
		webrtc::CreateSessionDescriptionObserver::Release();
	}

	void MyCreateSessionDescriptionObserver::AddRef() const {
		webrtc::CreateSessionDescriptionObserver::AddRef();
	}
}