
#pragma once

#include "CoreMinimal.h"
#include "ReHttpDebugType.h"

namespace ReHttpDebugJson
{
	void ReHttpDEBUG_API HotHttpDebugFileInfoToJson(const FHotHttpDebugFileInfo& InFileInfo, FString& OutJsonString);
	bool ReHttpDEBUG_API JsonToHotHttpDebugFileInfo(const FString& InJsonString, FHotHttpDebugFileInfo& OutFileInfo);
}