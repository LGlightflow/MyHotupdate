// Copyright Epic Games, Inc. All Rights Reserved.

#include "ReUpdateEditorCommands.h"

#define LOCTEXT_NAMESPACE "FReUpdateEditorModule"

void FReUpdateEditorCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "ReUpdateEditor", "Bring up ReUpdateEditor window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
