
#include <functional>

namespace NetherNet {
	//TODO
	struct SignalingHostConnectionStatus {};

	class IWebRTCSignalingInterface {
	public:
		using RelayServerConfigReadyCallback = std::function<void(ERelayServerConfigurationResult)>;
		using SignInCallback = std::function<void(std::error_code)>;
		using ConnectionStatusChangedCallback = std::function<void(SignalingHostConnectionStatus, uint32_t)>;

		struct SignalingConfiguration {
			const char* SignalingHostUrl;
			const char* AuthToken;
			const char* StunServer;
			const char* TurnServer;
		};

		// vIndex: 0
		virtual void SignIn(SignInCallback const&, ConnectionStatusChangedCallback const&, SignalingConfiguration const&) = 0;

		// vIndex: 1
		virtual void SignOut() = 0;

		// vIndex: 2
		virtual bool IsSignedIn() const = 0;

		// vIndex: 3
		virtual void Update() = 0;
	};
}