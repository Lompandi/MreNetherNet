
#include "Utils.hpp"

namespace NetherNet::Utils {
	/*
	GetTimeNow - Return time in second since epoch
	*/
	std::chrono::steady_clock::time_point GetTimeNow() {
		return std::chrono::high_resolution_clock::now();
	}
}