
#include "ConnectError.hpp"

namespace NetherNet {
	ConnectError ConnectError::Create(uint64_t sessionId, ESessionError err) {
		ConnectError connErr;
		connErr.mSessionId = sessionId;
		connErr.mErr = err;

		return connErr;
	}
}