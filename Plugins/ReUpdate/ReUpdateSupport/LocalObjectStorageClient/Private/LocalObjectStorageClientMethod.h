

#pragma once

#include "CoreMinimal.h"
#include "Version/ReVersion.h"

namespace LocalObjectStorageClientMethod
{
	FString GetParseValue(const FString& InKey);
	bool LocalPut(const FString& InSource, const FString& InTarget);
}