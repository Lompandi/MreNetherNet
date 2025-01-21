#pragma once

#include <functional>
#include <api/peer_connection_interface.h>

namespace NetherNet {
	class MyCreateSessionDescriptionObserver : public webrtc::CreateSessionDescriptionObserver {
	public:
		static MyCreateSessionDescriptionObserver
		Create(std::function<void(webrtc::SessionDescriptionInterface*)>&& on_success, std::function<void(webrtc::RTCError const&)>&& on_error);

		void OnFailure(webrtc::RTCError err) override;
		void OnSuccess(webrtc::SessionDescriptionInterface* iface) override;
		webrtc::RefCountReleaseStatus Release() const override;
		void AddRef() const override;

	public:
		std::function<void(webrtc::SessionDescriptionInterface*)>	mSuccessCallback;
		std::function<void(webrtc::RTCError const&)>				mErrCallback;
	};
}