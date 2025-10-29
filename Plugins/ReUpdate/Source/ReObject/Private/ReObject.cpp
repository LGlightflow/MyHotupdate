// Copyright Epic Games, Inc. All Rights Reserved.

#include "ReObject.h"

#define LOCTEXT_NAMESPACE "FReObjectModule"

void FReObjectModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FReObjectModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FReObjectModule, ReObject)