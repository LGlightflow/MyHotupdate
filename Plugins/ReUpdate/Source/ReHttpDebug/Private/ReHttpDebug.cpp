// Copyright Epic Games, Inc. All Rights Reserved.

#include "ReHttpDebug.h"
#include "HttpModule.h"
#include "HttpManager.h"

#define LOCTEXT_NAMESPACE "FReHttpDebugModule"

void FReHttpDebugModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
		// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	double SoftLimitSeconds = 2.0;
	double HardLimitSeconds = 4.0;

	GConfig->GetDouble(TEXT("HTTP"), TEXT("FlushSoftTimeLimitDefault"), SoftLimitSeconds, GEngineIni);
	GConfig->GetDouble(TEXT("HTTP"), TEXT("FlushHardTimeLimitDefault"), HardLimitSeconds, GEngineIni);

	bool bReloadConfig = false;

	if (SoftLimitSeconds > 0.f && SoftLimitSeconds <= 6000.f)
	{
		GConfig->SetDouble(TEXT("HTTP"), TEXT("FlushSoftTimeLimitDefault"), 6000.f, GEngineIni);
		bReloadConfig = true;
	}

	if (HardLimitSeconds > 0.f && HardLimitSeconds <= 7000.f)
	{
		GConfig->SetDouble(TEXT("HTTP"), TEXT("FlushHardTimeLimitDefault"), 7000.f, GEngineIni);
		bReloadConfig = true;
	}

	if (bReloadConfig)
	{
		FHttpModule::Get().GetHttpManager().UpdateConfigs();
	}
}

void FReHttpDebugModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FReHttpDebugModule, ReHttpDebug)