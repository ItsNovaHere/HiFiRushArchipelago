#ifndef HIBIKI_MOD_H
#define HIBIKI_MOD_H

#include <UE4SSProgram.hpp>
#include <Mod/CppUserModBase.hpp>
#include <Unreal/Hooks.hpp>
#include "Client.h"

class HibikiMod : public CppUserModBase {
public:
	static HibikiMod* Instance;
	Client client;

	HibikiMod();

	auto on_unreal_init() -> void override;
	auto on_update() -> void override {
		client.Update();
	}

	auto on_ui_init() -> void override {
		UE4SS_ENABLE_IMGUI()
	}

};

#endif
