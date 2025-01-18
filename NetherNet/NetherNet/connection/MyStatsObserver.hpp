#pragma once

#include <api/peer_connection_interface.h>
#include <rtc_base/ref_count.h>


namespace NetherNet {
    class MyStatsObserver : public ::webrtc::StatsObserver {
    public:
        void AddRef() const override;

        webrtc::RefCountReleaseStatus Release() const override;

        void OnComplete(::std::vector<::webrtc::StatsReport const*> const& reports) override;
    };
}