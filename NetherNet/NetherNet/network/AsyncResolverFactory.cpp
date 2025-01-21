
#include "AsyncResolverFactory.hpp"

namespace NetherNet {
	std::unique_ptr<webrtc::AsyncDnsResolverInterface> 
	AsyncResolverFactory::Create() {
		return std::make_unique<webrtc::AsyncDnsResolverInterface>(mWorkerThread);
	}

	bool 
	DnsCache::TryGet(std::string_view hostname, int family, std::vector<rtc::IPAddress>* outResult) {
		std::string hostname_(hostname);

		auto key_elem = std::make_pair(hostname_, family);
		auto fetch_result = mCacheObject.find(key_elem);

		if (fetch_result == mCacheObject.end())
			return false;

		Utils::TimePoint current_time
			= std::chrono::steady_clock::now();

		//Record time expired
		if (fetch_result->second.second < current_time) {
			mCacheObject.erase(fetch_result);
			return false;
		}

		for (const auto& ip : fetch_result->second.first) {
			outResult->push_back(ip);
		}
		return true;
	}

	void 
	DnsCache::Add(std::string_view hostname, int family, std::vector<rtc::IPAddress> const& result, Utils::TimePoint expiration) {
		std::string hostname_ (hostname);

		auto hostname_family =
			std::make_pair(hostname_, family);

		auto result_time =
			std::make_pair(result, expiration);

		mCacheObject.insert_or_assign(hostname_family, result_time);
	}

	void 
	DnsCache::Add(std::string_view hostname, int family, std::vector<rtc::IPAddress> const& result) {
		Utils::TimePoint current_time
			= std::chrono::steady_clock::now();

		Add(hostname, family, result, current_time + std::chrono::minutes(5));
	}

	void 
	AsyncResolver::Start(rtc::SocketAddress const& addr, int family) {
		mAddr = addr;
		auto worker_thread = mWorkerThread;
		worker_thread->PostTask([&]() {
			//TODO
		});
	}
}