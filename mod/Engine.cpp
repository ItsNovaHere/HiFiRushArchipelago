#include "Engine.h"
#include <Unreal/AActor.hpp>
#include <Unreal/UClass.hpp>
#include <Unreal/UObject.hpp>
#include <Unreal/UObjectArray.hpp>
#include <Unreal/UFunction.hpp>
#include <Unreal/UEngine.hpp>
#include <Unreal/FProperty.hpp>
#include <Unreal/FFrame.hpp>
#include <Unreal/UActorComponent.hpp>
#include <Unreal/Property/NumericPropertyTypes.hpp>
#include <stdlib.h>
#include <Windows.h>
#include "Mod.h"
#include "EHbkPlayerAppendAbilityType.h"

using namespace RC;
using namespace Unreal;

std::string GetMultiByte(std::wstring s) {
	auto size = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, NULL, 0, NULL, NULL);
	std::string r(size, 0);
	WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, &r[0], size, NULL, NULL);

	if (const auto pos = r.find('\0'); pos != std::string::npos){
		r.erase(pos);
	}

	return r;
}

UObject* Engine::OnStaticConstructObject(const FStaticConstructObjectParameters& Params, UObject* ConstructedObject) {


	return ConstructedObject;
}

void Engine::OnProcessEvent(UObject* Context, UFunction* Function, void* Params) {

}

static UnrealScriptFunction learnAbilityInternal = nullptr;
static FProperty* propStart = nullptr;


void LearnAbility(UObject* Context, FFrame& TheStack, void* RESULT_DECL) {
	// figure out how to get ability from this to see if we should let this happen or not
	
	for (auto i = 0; i < 256; i++) {
		auto ability = (EHbkPlayerAppendAbilityType) *(TheStack.Locals() + i);
		Log::Info("%d = %s", i, AbilityToString(ability));
	}

	learnAbilityInternal(Context, TheStack, RESULT_DECL);
}

void Engine::GiveAbility(EHbkPlayerAppendAbilityType ability) {
	// don't ask
	auto obj = UObjectGlobals::FindFirstOf(L"HbkPlayerCharacterManager_BP_C");
	auto func = obj->GetFunctionByNameInChain(L"LearnPlayerAbility");
	func->SetFuncPtr(learnAbilityInternal);

	struct params {
		UObject* obj;
		EHbkPlayerAppendAbilityType ability;
	} p {
		obj = obj,
		ability = ability
	};

	obj->ProcessEvent(func, (void*)&p);
	func->SetFuncPtr(&LearnAbility);
}


struct FEngineCreateListener : public FUObjectCreateListener {
	static FEngineCreateListener EngineCreateListener;

	void NotifyUObjectCreated(const UObjectBase* object, int32 index) override {
		auto cla = ((UObject*)object)->GetClassPrivate();
		if (cla->GetSuperStruct()->GetName() == L"HbkPlayerCharacterManager") {
			auto func = ((UObject*)object)->GetFunctionByNameInChain(L"LearnPlayerAbility");

			if (learnAbilityInternal == nullptr) {
				learnAbilityInternal = func->GetFuncPtr();
				propStart = func->GetFirstPropertyToInit();

				Log::Info("%p", (void*)learnAbilityInternal);

				func->SetFuncPtr(&LearnAbility);
			}

			if (func->GetFuncPtr() != &LearnAbility) {
				func->SetFuncPtr(&LearnAbility);
			}
		}
	}

	void OnUObjectArrayShutdown() override {
		UObjectArray::RemoveUObjectCreateListener(this);
	}
};
FEngineCreateListener FEngineCreateListener::EngineCreateListener{};

void Engine::SetupHooks() {
	UObjectArray::AddUObjectCreateListener(&FEngineCreateListener::EngineCreateListener);
}



static UnrealScriptFunction old = nullptr;

void empty(UObject* Context, FFrame& TheStack, void* RESULT_DECL) {
	static std::vector<const wchar_t*> itemsToHook = {L"LifeCoreItem", L"CircuitItem", L"ReverbPieceItem", L"LifeTankPieceItem"};

	for (auto item : itemsToHook) {
		if (Context->GetName().rfind(item, 0) == 0) {
			Log::Info("%s (%s) tried to call Multicast_UseItem.", GetMultiByte(Context->GetName()).c_str(), GetMultiByte(Context->GetFullName()).c_str());
			HibikiMod::Instance->client.SendItem(GetMultiByte(Context->GetName()));
			return;
		}
	}

	old(Context, TheStack, RESULT_DECL);
}

std::pair<bool, bool> Engine::OnMapLoad(UEngine* EngineInst, FWorldContext &WorldContext, FURL URL, UPendingNetGame* PendingGame, FString &Error) {
	std::vector<UObject*> objs{};
	UObjectGlobals::FindAllOf(L"PlaceableItem_BP_C", objs);

	for (auto obj : objs) {
		Log::Info("PlaceableItem_BP Found (%s)", GetMultiByte(obj->GetFullName()).c_str());

		UFunction* func = obj->GetFunctionByNameInChain(L"Multicast_UseItem");

		if (old == nullptr) {
			old = func->GetFuncPtr();
		}

		func->SetFuncPtr(&empty);
	}

	if (UObjectGlobals::FindFirstOf(L"HbkMainGameMode_C") != nullptr) {
		HibikiMod::Instance->client.SetState(APClient::ClientStatus::PLAYING);
	} else {
		HibikiMod::Instance->client.SetState(APClient::ClientStatus::READY);
	}

	return std::make_pair(false, true);
}