#include "Client.h"
#include <apuuid.hpp>
#include <string>
#include <functional>
#include "Engine.h"
#include "Game/EHbkPlayerAppendAbilityType.h"
#include "log.h"



using namespace std::placeholders;

Client::Client() {

}

void Client::SendItem(const std::string& Item) {
	ap->LocationChecks( { ap->get_location_id(Item) } );
	ap->Sync();
}

void Client::SetState(APClient::ClientStatus status) {
	if (ap) {
		ap->StatusUpdate(status);
	}
}

void Client::Update() {
	if (ap) {
		ap->poll();
	}
}

void Client::Connect() {
	if (ap) { ap->reset(); }
	ap.reset();
	std::string uri(address);

	auto uuid = ap_get_uuid("uuid");
	ap.reset(new APClient(uuid, "Hi-Fi RUSH", uri.empty() ? APClient::DEFAULT_URI : uri, ""));

	ap->set_socket_connected_handler(std::bind(&Client::OnSocketConnected, this));
	ap->set_socket_disconnected_handler(std::bind(&Client::OnSocketDisconnected, this));
	ap->set_socket_error_handler(std::bind(&Client::OnSocketError, this, _1));
	ap->set_room_info_handler(std::bind(&Client::OnRoomInfo, this));
	ap->set_slot_connected_handler(std::bind(&Client::OnSlotConnected, this, _1));
	ap->set_slot_disconnected_handler(std::bind(&Client::OnSlotDisconnected, this));
	ap->set_slot_refused_handler(std::bind(&Client::OnSlotRefused, this, _1));
	ap->set_items_received_handler(std::bind(&Client::OnItemsReceived, this, _1));
	ap->set_print_handler(std::bind(&Client::OnPrint, this, _1));
	ap->set_print_json_handler((std::function<void (const std::list<APClient::TextNode>&)>) std::bind(&Client::OnPrintJson, this, _1));
	ap->set_bounced_handler(std::bind(&Client::OnBounced, this, _1));
}

void Client::OnSocketConnected() {
	Log::Info("[APClient] Socket Connected");
	ap->ConnectSlot(name, password, 0b111);
}

void Client::OnSocketDisconnected() {
	Log::Info("[APClient] Socket Disconnected");
}

void Client::OnSocketError(const std::string &Error) {
	Log::Info("[APClient] Socket error: %s", Error.c_str());
}

void Client::OnRoomInfo() {
	ap->GetDataPackage();
}

void Client::OnSlotConnected(const nlohmann::json& _) {
	Log::Info("[APClient] Slot Connected");
	ap->StatusUpdate(APClient::ClientStatus::READY);
}

void Client::OnSlotDisconnected() {
	Log::Info("[APClient] Slot Disconnected");
}

void Client::OnSlotRefused(const std::list <std::string> &errors) {
	Log::Info("[APClient] Slot refused");
}

void Client::OnItemsReceived(const std::list <APClient::NetworkItem> &items) {
	static std::unordered_map<std::string, std::function<void()>> giveMap = {
			// Abilities
			{ "Magnet", 						[] { Engine::GiveAbility(EHbkPlayerAppendAbilityType::Action_Magnet); } 	},

			// Items
			{ "Life Gauge Piece", 				[] { Engine::GiveItem(L"ItemObj_LifeCorePiece_PLC_C"); } 					},
			{ "Electric Reverb Core Piece",	[] { Engine::GiveItem(L"ItemObj_ReverbPiece_PLC_C"); } 					},
			{ "Broken Piece of a Health Tank", [] { Engine::GiveItem(L"ItemObj_LifeTankPiece_PLC_C"); } 					},
	};

	for (auto item : items) {
		if (auto search = giveMap.find(ap->get_item_name(item.item)); search != giveMap.end()) {
			search->second();
		} else {
			Log::Info("Tried to give item %s, but is not in the give map.", ap->get_item_name(item.item).c_str());
		}
	}
}

void Client::OnPrint(const std::string &msg) {
	Log::Info("[APClient] %s", msg.c_str());
}

void Client::OnPrintJson(const std::list <APClient::TextNode> &msg) {

}

void Client::OnBounced(const nlohmann::json& cmd) {

}