#pragma once

#include <chrono>

#include <rtc_base/thread.h>
#include <api/packet_socket_factory.h>
#include <rtc_base/async_dns_resolver.h>

#include "../utils/Utils.hpp"
#include <rtc_base/containers/flat_map.h>

namespace NetherNet {
	struct DnsCache;

	class AsyncResolverFactory : public webrtc::AsyncDnsResolverFactoryInterface {
		AsyncResolverFactory(webrtc::TaskQueueBase* workerThread) : mWorkerThread(workerThread) {};

		std::unique_ptr<webrtc::AsyncDnsResolverInterface> Create();
	private:
		webrtc::TaskQueueBase const*	mWorkerThread;
		std::shared_ptr<DnsCache>		mCache;
	};

	struct DnsCache {
		bool TryGet(std::string_view hostname, int family, std::vector<rtc::IPAddress>* outResult);
		void Add(std::string_view hostname, int family, std::vector<rtc::IPAddress> const& result);
		void Add(std::string_view hostname, int family, std::vector<rtc::IPAddress> const& result, Utils::TimePoint expiration);
	private:
		webrtc::flat_map<std::pair<std::string, int>, std::pair<std::vector<rtc::IPAddress>, Utils::TimePoint>> mCacheObject;
	};

	namespace {
		class AsyncResolver {
		public:
			void Start(rtc::SocketAddress const& addr, int family);
			void Start(rtc::SocketAddress const& addr);
			void GetResolvedAddress();

		private:
			rtc::SocketAddress mAddr;
			webrtc::TaskQueueBase* mWorkerThread;

		};
	}
}