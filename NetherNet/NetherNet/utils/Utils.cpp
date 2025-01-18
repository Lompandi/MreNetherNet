
#include "Utils.hpp"

namespace NetherNet::Utils {
	/*
	GetTimeNow - Return time in second since epoch
	*/
	auto GetTimeNow() {
		return std::chrono::high_resolution_clock::now();
	}
}