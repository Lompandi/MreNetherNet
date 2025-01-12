#pragma once

#include <api/peer_connection_interface.h>
#include <rtc_base/ref_count.h>


namespace NetherNet {
    class MyStatsObserver : public ::webrtc::StatsObserver {
    public:
        virtual void AddRef() const override;

        virtual rtc::RefCountReleaseStatus Release() const override;

        virtual void OnComplete(::std::vector<::webrtc::StatsReport const*> const& reports) override;
    };
}