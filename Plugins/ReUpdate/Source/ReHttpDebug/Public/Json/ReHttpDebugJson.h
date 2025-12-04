
#pragma once

#include "CoreMinimal.h"
#include "ReHttpDebugType.h"

namespace ReHttpDebugJson
{
	void REHTTPDEBUG_API HotHttpDebugFileInfoToJson(const FHotHttpDebugFileInfo& InFileInfo, FString& OutJsonString);
	bool REHTTPDEBUG_API JsonToHotHttpDebugFileInfo(const FString& InJsonString, FHotHttpDebugFileInfo& OutFileInfo);
}