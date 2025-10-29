// Copyright Epic Games, Inc. All Rights Reserved.

#include "RePak.h"
#include "HAL/PlatformFilemanager.h"
#include "IPlatformFilePak.h"

#define LOCTEXT_NAMESPACE "FRePakModule"

void FRePakModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FRePakModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

FPakPlatformFile* FRePakModule::GetPakPlatformFile()
{
	if (!PakPlatformFile)
	{
		if (IPlatformFile* InPlatformFile = FPlatformFileManager::Get().FindPlatformFile(TEXT("PakFile")))
		{
			PakPlatformFile = static_cast<FPakPlatformFile*>(InPlatformFile);
		}
		else
		{
			PakPlatformFile = new FPakPlatformFile();
			if (PakPlatformFile->Initialize(&FPlatformFileManager::Get().GetPlatformFile(), TEXT("")))
			{
				//
				FPlatformFileManager::Get().SetPlatformFile(*PakPlatformFile);
			}
		}
	}

	return PakPlatformFile;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FRePakModule, RePak)