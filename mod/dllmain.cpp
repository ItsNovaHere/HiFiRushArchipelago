#include <stdio.h>
#include <Mod/CppUserModBase.hpp>
#include <Unreal/Hooks.hpp>

#include "Engine.h"
#include "log.h"

using namespace RC;
using namespace Unreal;

class HibikiMod : public CppUserModBase {
public:
	HibikiMod() {
		ModVersion = STR("0.0.1");
		ModName = STR("Hi-Fi RUSH Archipelago Support");
		ModAuthors = STR("ItsNovaHere");
		ModDescription = STR("Adds archipelago support for Hi-Fi RUSH.");
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