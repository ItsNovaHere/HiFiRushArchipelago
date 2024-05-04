#ifndef ENGINE_H
#define ENGINE_H

#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/UObjectArray.hpp>
#include <Unreal/UObject.hpp>
#include <Unreal/FFrame.hpp>
#include <Unreal/NameTypes.hpp>
#include <Unreal/Hooks.hpp>
#include <Unreal/FURL.hpp>
#include "Game/EHbkPlayerAppendAbilityType.h"
#include "Log.h"
#include "Util.h"

using namespace RC;
using namespace Unreal;

struct FGameplayTag {
	FName TagName;
};

typedef void (*FNativeFuncPtr)(UObject* Context, FFrame_50_AndBelow& TheStack, void* Z_Param__Result);
typedef bool (__thiscall *FPlayerTakeItem)(UObject* t, FGameplayTag tag, int count);

namespace Engine {
	struct FEngineCreateListener : public FUObjectCreateListener {
		void NotifyUObjectCreated(const UObjectBase* object, int32 index) override;
		void OnUObjectArrayShutdown() override;
	};
	inline static FEngineCreateListener EngineCreateListener{};

	static UnrealScriptFunction LearnAbility_internal = nullptr;
	static UnrealScriptFunction UseItem_internal = nullptr;
	static UnrealScriptFunction ActorDied_internal = nullptr;

	static std::unordered_map<std::wstring, FGameplayTag> placementAssets = {};
	static bool loadedPlacementAssets = false;
	static FPlayerTakeItem takeItem = (FPlayerTakeItem) 0x141f057f0; // TODO: scan for this address instead of hard coding it

	std::pair<bool, bool> OnMapLoad(UEngine* EngineInst, struct FWorldContext& WorldContext, FURL URL, UPendingNetGame* PendingGame, FString& Error);
	void SetupHooks();
	void GiveAbility(enum EHbkPlayerAppendAbilityType ability);
	void GiveItem(const wchar_t* name);
	void LoadPlacementAssets();
	void ForceDeath();
}

#endif
