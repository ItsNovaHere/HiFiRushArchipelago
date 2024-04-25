#include <stdio.h>
#include <UE4SSProgram.hpp>
#include <Mod/CppUserModBase.hpp>
#include <Unreal/Hooks.hpp>

#include "Engine.h"
#include "Client.h"
#include "log.h"

using namespace RC;
using namespace Unreal;

class HibikiMod : public CppUserModBase {
	Client client;

public:
	HibikiMod() {
		ModVersion = STR("0.0.1");
		ModName = STR("Hi-Fi RUSH Archipelago Support");
		ModAuthors = STR("ItsNovaHere");
		ModDescription = STR("Adds archipelago support for Hi-Fi RUSH.");

		register_tab(STR("Archipelago Client"), [](CppUserModBase* instance) {
			ImGui::Text("AP Client Settings");

			auto mod = dynamic_cast<HibikiMod*>(instance);
			if (!mod) { return; }

			ImGui::Text("IP Address: ");
			ImGui::SameLine();
			ImGui::PushID("address");
			ImGui::InputText("", mod->client.address, IM_ARRAYSIZE(mod->client.address));
			ImGui::PopID();

			ImGui::Text("Password: ");
			ImGui::SameLine();
			ImGui::PushID("password");
			ImGui::InputText("", mod->client.password, IM_ARRAYSIZE(mod->client.password), !mod->client.show_password ? ImGuiInputTextFlags_Password : ImGuiInputTextFlags_None);
			ImGui::PopID();

			ImGui::Text("Show Password: ");
			ImGui::SameLine();
			ImGui::PushID("showpassword");
			ImGui::Checkbox("", &mod->client.show_password);
			ImGui::PopID();

			ImGui::Text("Name: ");
			ImGui::SameLine();
			ImGui::PushID("name");
			ImGui::InputText("", mod->client.name, IM_ARRAYSIZE(mod->client.name));
			ImGui::PopID();

			if (ImGui::Button("Connect")) {
				mod->client.Connect();
			}
		});
	}

	auto on_unreal_init() -> void override {
		// Look into AHbkPlaceableItem or AHbkItemBase
		// Separate the collecting and rewarding

		Hook::RegisterStaticConstructObjectPostCallback(&Engine::OnStaticConstructObject);
		Hook::RegisterProcessEventPreCallback(&Engine::OnProcessEvent);
		Hook::RegisterLoadMapPostCallback(&Engine::OnMapLoad);

		/*
		 * Items: Track 1
		 * Life Gauge 1 (A Fresh Start)
		 * Life Gauge 2 (A Fresh Start)
		 * Armstrong Circuit 1 (A Fresh Start) [THIS IS NEWGAME+]
		 * Armstrong Circuit 2 (A Fresh Start)
		 * Health Tank 1 (A Fresh Start)
		 * Reverb Core 1 (A Fresh Start)
		 * Armstrong Circuit 3 (A Fresh Start) [NEWGAME+]
		 * Graffiti 1 (A Fresh Start)
		 * Life Gauge 3 (A Fresh Start)
		 * Armstrong Circuit 4 (A Fresh Start)
		 * Graffiti 2 (A Fresh Start)
		 * QA-1MIL (A Fresh Start)
		 * */
	}

	auto on_update() -> void override {
		client.Update();
	}

	auto on_ui_init() -> void override {
		UE4SS_ENABLE_IMGUI()
	}

};

#define HIBIKI_API __declspec(dllexport)
extern "C" {
	HIBIKI_API CppUserModBase* start_mod() {
		return new HibikiMod();
	}

	HIBIKI_API void uninstall_mod(CppUserModBase* mod){
		delete mod;
	}
}