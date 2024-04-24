#ifndef ENGINE_H
#define ENGINE_H

#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/Hooks.hpp>
#include <Unreal/FURL.hpp>
#include "log.h"

using namespace RC;
using namespace Unreal;

namespace Engine {
	UObject* OnStaticConstructObject(const FStaticConstructObjectParameters& Params, UObject* ConstructedObject);
	void OnProcessEvent(UObject* Context, UFunction* Function, void* Params);
	std::pair<bool, bool> OnMapLoad(UEngine* EngineInst, struct FWorldContext& WorldContext, FURL URL, UPendingNetGame* PendingGame, FString& Error);
}

#endif
