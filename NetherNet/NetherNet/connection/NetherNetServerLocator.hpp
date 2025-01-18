#pragma once

#include <gsl/gsl>
#include <cstdint>
#include <chrono>

#include "../level/GameType.hpp"
#include "NetherNetConnector.hpp"

#include "PingedCompatibleServer.hpp"
#include "../utils/NonOwnerPointer.hpp"

class AppPlatform;			  //TODO
class PortPair;					//TODO

namespace NetherNet {
	class NetherNetServerLocator {
	public:
		struct ServerData {
			//TODO
		};

		// vIndex: 3 (+24)
		virtual void startAnnouncingServer(
			std::string const& playerName,
			std::string const& worldName,
			Bedrock::NonOwnerPointer<AppPlatform> platform,
			GameType game_type,
			int nume_players,
			int max_player_allowed,
			bool isJoinableThroughServerScreen,
			bool isEditorWorld
		);

		// vIndex: 4 (+32)
		virtual void stopAnnouncingServer(Bedrock::NonOwnerPointer<AppPlatform> appPlatform) /* override */;

		// vIndex: 5 (+40)
		virtual void startServerDiscovery(PortPair ports);

		// vIndex: 6 (+48)
		virtual void stopServerDiscovery() /*override*/;

		// vIndex: 9 (+72)
		std::vector<PingedCompatibleServer> getServerList() /*override*/;

		// vIndex: 10 (+80)
		virtual void clearServerList() /*override*/;

		// vIndex: 11 (+88)
		void update() /*override*/;

		void _cacheDiscoveryResponseData(ServerData const& data);

		void _onDiscoveryResponse(uint64_t id, gsl::span<const char> response_data);

		void _pruneStaleServers(std::chrono::seconds stale_after);

		void _setIsAnnouncing(bool is_announcing);

		void _setIsDiscovering(bool is_discovering);

		void setNetherNetConnector(Bedrock::NonOwnerPointer<NetherNetConnector>&& connector);

		static PingedCompatibleServer _transformFrom(uint64_t id, ServerData&& data);

	public:
		int									mUnk1;			//this + 0x8

		std::vector<PingedCompatibleServer> mServerList;	//this + 0x20
		bool								mAnnouncing;	//this + 0x68
		bool								mDiscovering;	//this + 0x69
		std::mutex							mLockGuard;		//this + 0x70
	};
}