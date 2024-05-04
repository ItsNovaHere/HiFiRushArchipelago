#include "Engine.h"
#include <Unreal/AActor.hpp>
#include <Unreal/UClass.hpp>
#include <Unreal/UObject.hpp>
#include <Unreal/UAssetRegistry.hpp>
#include <Unreal/UAssetRegistryHelpers.hpp>
#include <Unreal/UObjectArray.hpp>
#include <Unreal/UFunction.hpp>
#include <Unreal/UEngine.hpp>
#include <Unreal/FProperty.hpp>
#include <Unreal/FFrame.hpp>
#include <Unreal/UActorComponent.hpp>
#include <Unreal/Property/NumericPropertyTypes.hpp>
#include <Unreal/Property/FMulticastInlineDelegateProperty.hpp>
#include <Unreal/UScriptStruct.hpp>
#include <stdlib.h>
#include "Mod.h"
#include "Game/EHbkPlayerAppendAbilityType.h"

using namespace RC;
using namespace Unreal;

extern UnrealScriptFunction Engine::LearnAbility_internal;
extern UnrealScriptFunction Engine::UseItem_internal;
extern UnrealScriptFunction Engine::ActorDied_internal;

extern std::unordered_map<std::wstring, FGameplayTag> Engine::placementAssets;
extern Engine::FEngineCreateListener Engine::EngineCreateListener;
extern FPlayerTakeItem Engine::takeItem;
extern bool Engine::loadedPlacementAssets;

void LearnAbility_Hook(UObject* Context, FFrame& TheStack, void* Z_Param__Result) {
	if (HibikiMod::Instance->client.IsConnected()) {
		auto Stack = (FFrame_50_AndBelow &) TheStack;
		const FNativeFuncPtr* GNatives = (FNativeFuncPtr*) 0x147196eb0; // TODO: scan for gnatives
		if (Stack.Code) {
			// this is really ugly, but ue4/ue4ss have forced me to do this.
			auto reset = Stack.Code;
			UObject* obj;
			auto B = *Stack.Code++;
			(GNatives[B])(TheStack.Object(), Stack, &obj);

			EHbkPlayerAppendAbilityType ability;
			B = *Stack.Code++;
			(GNatives[B])(TheStack.Object(), Stack, &ability);

			static std::list <EHbkPlayerAppendAbilityType> AbilitiesToDisableLearning = {
					EHbkPlayerAppendAbilityType::Action_Magnet,
			};

			for (auto a: AbilitiesToDisableLearning) {
				if (ability == a) {
					Log::Info("Game tried to learn ability %s, but it is in the block list, returning.", AbilityToString(ability));
					return;
				}
			}

			Stack.Code = reset;
		}
	}

	Engine::LearnAbility_internal(Context, TheStack, Z_Param__Result);
}

void UseItem_Hook(UObject* Context, FFrame& TheStack, void* Z_Param__Result) {
	static std::vector<const wchar_t*> itemsToHook = {L"LifeCoreItem", L"CircuitItem", L"ReverbPieceItem", L"LifeTankPieceItem"};

	if (HibikiMod::Instance->client.IsConnected()) {
		for (auto item: itemsToHook) {
			if (Context->GetName().rfind(item, 0) == 0) {
				Log::Info("%s (%s) tried to call Multicast_UseItem.", Util::WideToMultiByte(Context->GetName()).c_str(), Util::WideToMultiByte(Context->GetFullName()).c_str());
				HibikiMod::Instance->client.SendItem(Util::WideToMultiByte(Context->GetName()));
				return;
			}
		}
	}

	Engine::UseItem_internal(Context, TheStack, Z_Param__Result);
}

void ActorDied_Hook(UObject* Context, FFrame& TheStack, void* Z_Param__Result) {
	if (HibikiMod::Instance->client.IsConnected()) {
		HibikiMod::Instance->client.SendDeathLink();
	}

	Engine::ActorDied_internal(Context, TheStack, Z_Param__Result);
}

void Engine::FEngineCreateListener::NotifyUObjectCreated(const RC::Unreal::UObjectBase* object, RC::Unreal::int32 index) {
	auto cla = ((UObject*)object)->GetClassPrivate();
	if (cla->GetSuperStruct()->GetName() == L"HbkPlayerCharacterManager") {
		auto func = ((UObject*)object)->GetFunctionByNameInChain(L"LearnPlayerAbility");

		if (LearnAbility_internal == nullptr) {
			LearnAbility_internal = func->GetFuncPtr();
			func->SetFuncPtr(&LearnAbility_Hook);
		}

		if (func->GetFuncPtr() != &LearnAbility_Hook) {
			func->SetFuncPtr(&LearnAbility_Hook);
		}
	}

	if (cla->GetSuperStruct()->GetName() == L"HbkPlayerController") {
		auto func = ((UObject*)object)->GetFunctionByNameInChain(L"Receive_ActorDied");

		if (ActorDied_internal == nullptr) {
			ActorDied_internal = func->GetFuncPtr();
			func->SetFuncPtr(&ActorDied_Hook);
		}

		if (func->GetFuncPtr() != &ActorDied_Hook) {
			func->SetFuncPtr(&ActorDied_Hook);
		}
	}
}

void Engine::FEngineCreateListener::OnUObjectArrayShutdown() {
	UObjectArray::RemoveUObjectCreateListener(this);
}

void Engine::GiveItem(const wchar_t* name) {
	// TODO: maybe hook the popup to say who gave the item?
	auto obj = UObjectGlobals::FindFirstOf(L"HbkPlayerCharacterManager_BP_C");
	if (obj && Engine::placementAssets.contains(name)) {
		auto tag = placementAssets[name];
		Engine::takeItem(obj, tag, 3);
	} else {
		//TODO: log error
	}
}

void Engine::GiveAbility(EHbkPlayerAppendAbilityType ability) {
	auto obj = UObjectGlobals::FindFirstOf(L"HbkPlayerCharacterManager_BP_C");
	if (obj) {
		auto func = obj->GetFunctionByNameInChain(L"LearnPlayerAbility");
		if (func) {
			func->SetFuncPtr(LearnAbility_internal);

			struct params {
				UObject* obj;
				EHbkPlayerAppendAbilityType ability;
			} p {
				obj = obj,
				ability = ability
			};

			obj->ProcessEvent(func, (void*)&p);
			func->SetFuncPtr(&LearnAbility_Hook);
		}
	}
}

void Engine::ForceDeath() {
	auto obj = UObjectGlobals::FindFirstOf(L"HbkPlayerControllerBP_C");
	if (obj) {
		auto damage = (UObject*) obj->GetValuePtrByPropertyNameInChain(L"HbkDamage");
		if (damage) {
			auto func = damage->GetFunctionByNameInChain(L"SetDied");
			if (func) {
				obj->ProcessEvent(func, nullptr);
			}
		}
	}
}

void Engine::SetupHooks() {
	UObjectArray::AddUObjectCreateListener(&Engine::EngineCreateListener);
}

void Engine::LoadPlacementAssets() {
	TArray<FAssetData> AllAssets{nullptr, 0, 0};
	auto reg = (UAssetRegistry*) UAssetRegistryHelpers::GetAssetRegistry().ObjectPointer;
	reg->GetAllAssets(AllAssets, false);

	for (FAssetData& Asset : AllAssets) {
		if (Asset.ObjectPath().ToString().ends_with(L"_PLC_C")) {
			auto clas = (UClass*) UAssetRegistryHelpers::GetAsset(Asset); // _C is actually just a class, so we can cast and get default object.
			auto asset = clas->GetClassDefaultObject();
			Engine::placementAssets.insert({ Asset.AssetName().ToString(), *asset->GetValuePtrByPropertyNameInChain<FGameplayTag>(L"InventoryItemTag") });
		}
	}

	Engine::loadedPlacementAssets = true;
}

std::pair<bool, bool> Engine::OnMapLoad(UEngine* EngineInst, FWorldContext &WorldContext, FURL URL, UPendingNetGame* PendingGame, FString &Error) {
	std::vector < UObject * > objs{};
	UObjectGlobals::FindAllOf(L"PlaceableItem_BP_C", objs);

	for (auto obj: objs) {
		Log::Info("PlaceableItem_BP Found (%s)", Util::WideToMultiByte(obj->GetFullName()).c_str());

		UFunction* func = obj->GetFunctionByNameInChain(L"Multicast_UseItem");

		if (UseItem_internal == nullptr) {
			UseItem_internal = func->GetFuncPtr();
		}

		func->SetFuncPtr(&UseItem_Hook);
	}

	if (UObjectGlobals::FindFirstOf(L"HbkMainGameMode_C") != nullptr) {
		HibikiMod::Instance->client.SetState(APClient::ClientStatus::PLAYING);
	} else {
		HibikiMod::Instance->client.SetState(APClient::ClientStatus::READY);
	}

	if (!Engine::loadedPlacementAssets) {
		Engine::LoadPlacementAssets();
	}


	return std::make_pair(false, true);
}