#pragma once

#include "CoreMinimal.h"

namespace ReUnrealPakEditor
{
	struct FPak
	{
		bool PakFile(const TArray<FString>& InPaths);
		bool PakMapFile(const TArray<FString>& InPaths);
		void ListPak(const FString& InPakPath);
	};
}