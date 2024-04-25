#define AP_PREFER_UNENCRYPTED

#include <apclient.hpp>
#include <apuuid.hpp>
#include <nlohmann/json.hpp>
#include <memory>
#include <list>
#include <string>

class Client {
public:
	char address[128];
	char name[128];
	char password[128];
	bool show_password;

	Client();
	void Connect();
	void Update();

private:
	std::unique_ptr<APClient> ap;

	void OnSocketConnected();
	void OnSocketDisconnected();
	void OnSocketError(const std::string& Error);
	void OnRoomInfo();
	void OnSlotConnected(const nlohmann::json&);
	void OnSlotDisconnected();
	void OnSlotRefused(const std::list<std::string>& errors);
	void OnItemsReceived(const std::list<APClient::NetworkItem>& items);
	void OnPrint(const std::string& msg);
	void OnPrintJson(const std::list<APClient::TextNode>& msg);
	void OnBounced(const nlohmann::json& cmd);
};
