#pragma warning(disable:4583)
#pragma warning(disable:4582)

#include <stdio.h>

#include "Mod.h"
#include "Engine.h"
#include "Game/EHbkPlayerAppendAbilityType.h"
#include "log.h"

using namespace RC;
using namespace Unreal;

HibikiMod* HibikiMod::Instance;

HibikiMod::HibikiMod() {
	Instance = this;
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

			if (ImGui::Button("Give grapple")) {
				Engine::GiveAbility(EHbkPlayerAppendAbilityType::Action_Magnet);
			}
		});
}

auto HibikiMod::on_unreal_init() -> void {
	Hook::RegisterLoadMapPostCallback(&Engine::OnMapLoad);

	Engine::SetupHooks();
}

#define HIBIKI_API __declspec(dllexport)
extern "C" {
	HIBIKI_API CppUserModBase* start_mod() {
		return new HibikiMod();
	}

	HIBIKI_API void uninstall_mod(CppUserModBase* mod){
		delete mod;
	}
}