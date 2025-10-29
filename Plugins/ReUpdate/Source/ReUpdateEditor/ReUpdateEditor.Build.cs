// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ReUpdateEditor : ModuleRules
{
	public ReUpdateEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);

        PublicDefinitions.Add("UE5_ENIGNE");//如果是UE5引擎 需要打开此选项，如果是UE4就把它注掉
        if (PublicDefinitions.Contains("UE5_ENIGNE"))
        {
            //如果使用UE4版本，把它注掉
            PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "EditorFramework",
            });
        }


        PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"InputCore",
				"EditorFramework",
				"UnrealEd",
				"ToolMenus",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "ContentBrowser",
                "Settings",
                "PropertyEditor",
                "ReUpdate",
                "ReObject",
                "RePak",
                "AssetRegistry",
                "PakFileUtilities"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
