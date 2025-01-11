
#include <json/json.h>

#include "parse_config.hpp"
#include "../../HNetherNet.hpp"

namespace NetherNet {
	//lambda_38eb4f9820abafb4ecc241422427c4cf_::operator()
	void parseConfig(const std::string& config) {
		::NetherNet::NetherNetTransport_LogMessage(4, "Parsing message contents from config: '%s'", config.data());
		Json::Reader confReader;
		Json::Value confVal(0);

		if (confReader.parse(config.data(), confVal)) {
			//TODO: check here
			auto& expirationSec = confVal["ExpirationInSeconds"];
			int expTimeSec = expirationSec.asInt();
			::NetherNet::NetherNetTransport_LogMessage(4, "Found expiration time in seconds %d", expTimeSec);

			Json::Value turn_auth_servers;

			auto& turnAuthServers = confVal["TurnAuthServers"];
			if (turnAuthServers.isNull()
				|| (turn_auth_servers = confVal["TurnAuthServers"], !turn_auth_servers.isArray())) {
				::NetherNet::NetherNetTransport_LogMessage(
					3,
					"Did not find TurnAuthServers, so we have the old services data contract");
				
				//TODO: TurnAuthServerVector

				auto userName = confVal["Username"].asString();
				auto passWord = confVal["Password"].asString();

				//TODO: processing

				::NetherNet::NetherNetTransport_LogMessage(4, "Found username/password: '%s' / '%s'", userName.data(), passWord.data());
			}
			else {
				::NetherNet::NetherNetTransport_LogMessage(4, "Found TurnAuthServers, so we have the new services data contract");
				auto& turnAuthServers = confVal["TurnAuthServers"];
				
				for (const auto& auth_server : turnAuthServers) {
					auto userName = auth_server["Username"].asString();
					auto passWord = auth_server["Password"].asString();

					NetherNet::NetherNetTransport_LogMessage(4, "Found username/password: '%s' / '%s'", userName.data(), passWord.data());
					auto& Urls = auth_server["Urls"];

					Json::Value url;
					if (Urls.isNull() || (url = confVal["Urls"], !url.isArray())) {
						::NetherNet::NetherNetTransport_LogMessage(2, "Failed to find Urls");
						//Error handling via setting values
					}
					else {
						for (const auto& curUrl : Urls) {
							auto cur_url = curUrl.asString();
							//Some more process logic
							if (cur_url.find("turn") != cur_url.npos) {
								NetherNet::NetherNetTransport_LogMessage(4, "Found turn server '%s'", cur_url.data());
							}
							else {
								if (cur_url.find("stun") != cur_url.npos) {
									NetherNet::NetherNetTransport_LogMessage(4, "Found stun server '%s'", cur_url.data());
								}
							}
						}
					}


				}
			}

		}
	}
}