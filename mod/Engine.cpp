#include "Engine.h"
#include <Unreal/AActor.hpp>
#include <Unreal/UClass.hpp>
#include <Unreal/UObject.hpp>
#include <Unreal/UFunction.hpp>
#include <Unreal/UEngine.hpp>
#include <Unreal/UActorComponent.hpp>
#include <stdlib.h>
#include <Windows.h>

using namespace RC;
using namespace Unreal;

UObject* Engine::OnStaticConstructObject(const FStaticConstructObjectParameters& Params, UObject* ConstructedObject) {


	return ConstructedObject;
}

void Engine::OnProcessEvent(UObject* Context, UFunction* Function, void* Params) {

}

std::string GetMultiByte(std::wstring s) {
	auto size = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, NULL, 0, NULL, NULL);
	std::string r(size, 0);
	WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, &r[0], size, NULL, NULL);

	return r;
}

static UnrealScriptFunction old = nullptr;

void empty(UObject* Context, FFrame& TheStack, void* RESULT_DECL) {
	static std::vector<const wchar_t*> itemsToHook = {L"LifeCoreItem", L"CircuitItem", L"ReverbPieceItem", L"LifeTankPieceItem"};

	for (auto item : itemsToHook) {
		if (Context->GetName().rfind(item, 0) == 0) {
			Log::Info("%s (%s) tried to call Multicast_UseItem.", GetMultiByte(Context->GetName()).c_str(), GetMultiByte(Context->GetFullName()).c_str());
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

	return std::make_pair(false, true);
}