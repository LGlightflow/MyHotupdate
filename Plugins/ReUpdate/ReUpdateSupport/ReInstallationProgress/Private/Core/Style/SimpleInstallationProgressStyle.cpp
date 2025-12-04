// Copyright (C) RenZhai.2021.All Rights Reserved.
#include "SimpleInstallationProgressStyle.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"

TSharedPtr< FSlateStyleSet > FSimpleInstallationProgressStyle::StyleInstance = NULL;

void FSimpleInstallationProgressStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FSimpleInstallationProgressStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

void FSimpleInstallationProgressStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FSimpleInstallationProgressStyle::Get()
{
	return *StyleInstance;
}

FName FSimpleInstallationProgressStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("SimpleInstallationProgressStyle"));
	return StyleSetName;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )

const FVector2D Icon1920x1080(1920.0f, 1080.0f);
TSharedRef<class FSlateStyleSet> FSimpleInstallationProgressStyle::Create()
{
	FString ProgressPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("Resources"));
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("SimpleInstallationProgressStyle"));
	
	Style->SetContentRoot(ProgressPath);

	Style->Set("SimpleInstallationProgressStyle.Background", new IMAGE_BRUSH(TEXT("1080x1920"), Icon1920x1080));

	return Style;
}
#undef IMAGE_BRUSH