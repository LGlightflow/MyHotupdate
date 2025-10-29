// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FPakPlatformFile;

class FRePakModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	FPakPlatformFile* GetPakPlatformFile();

private:
	FPakPlatformFile* PakPlatformFile;
};
