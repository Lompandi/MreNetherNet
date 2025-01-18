#pragma once

#include <set>
#include <map>

#include <pc/jsep_transport.h>
#include <rtc_base/socket_address.h>

#include "NetworkID.hpp"
#include "../utils/Utils.hpp"

namespace NetherNet {
	class PeerRecordTable {
	public:
        struct PeerRecord {
        public:
			//TODO
			NetworkID										mRemoteId;		 //this + 0x20
			std::chrono::high_resolution_clock::time_point  mLastActiveTime; //this + 0x28
			rtc::SocketAddress								mRemoteAddress;	 //this + 0x30
        };

		bool AddOrUpdate(NetworkID networkID, rtc::SocketAddress const& address);
		bool Contains(NetworkID id);
		bool Find(NetworkID id, rtc::SocketAddress* pOutAddr);

		static void RemoveExpiredRecords(NetherNet::Utils::ThreadSafe<std::map<NetworkID, PeerRecordTable::PeerRecord>>::View const& exclusiveTable);

	private:
		NetherNet::Utils::ThreadSafe<std::map<NetworkID, PeerRecordTable::PeerRecord>>  mExclusiveTable;  //this + 0x0
		std::map<NetworkID, PeerRecord>													mPeerRecordTable; //this + 0x50
	};
}