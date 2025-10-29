#pragma once

#include "CoreMinimal.h"
#include "RePakType.generated.h"

USTRUCT(BlueprintType)
struct REPAK_API FRePakConfig
{
	GENERATED_USTRUCT_BODY()

	FRePakConfig();

	/*32-bit AES key */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Config)
	FString AES;

	/*Is the key read from the local configuration */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Config)
	bool bConfiguration;
};