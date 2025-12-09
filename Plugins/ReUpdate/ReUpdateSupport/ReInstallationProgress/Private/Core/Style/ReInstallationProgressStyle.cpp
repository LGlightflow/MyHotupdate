// Copyright (C) RenZhai.2021.All Rights Reserved.
#include "ReInstallationProgressStyle.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"

TSharedPtr< FSlateStyleSet > FReInstallationProgressStyle::StyleInstance = NULL;

void FReInstallationProgressStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FReInstallationProgressStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

void FReInstallationProgressStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FReInstallationProgressStyle::Get()
{
	return *StyleInstance;
}

FName FReInstallationProgressStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("ReInstallationProgressStyle"));
	return StyleSetName;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )

const FVector2D Icon1920x1080(1920.0f, 1080.0f);
TSharedRef<class FSlateStyleSet> FReInstallationProgressStyle::Create()
{
	FString ProgressPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("Resources"));
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("ReInstallationProgressStyle"));
	
	Style->SetContentRoot(ProgressPath);

	Style->Set("ReInstallationProgressStyle.Background", new IMAGE_BRUSH(TEXT("1080x1920"), Icon1920x1080));

	return Style;
}
#undef IMAGE_BRUSH