// Copyright (C) RenZhai.2020.All Rights Reserved.

using UnrealBuildTool;

public class LocalObjectStorageClient : ModuleRules
{
	public LocalObjectStorageClient(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicIncludePaths.Add("Runtime/Launch/Public");

		PrivateIncludePaths.Add("Runtime/Launch/Private");      // For LaunchEngineLoop.cpp include

		PrivateDependencyModuleNames.Add("Core");
		PrivateDependencyModuleNames.Add("Projects");
		PrivateDependencyModuleNames.Add("CoreUObject");
		PrivateDependencyModuleNames.Add("ApplicationCore");
		PublicDependencyModuleNames.Add("ReObject");
	}
}
