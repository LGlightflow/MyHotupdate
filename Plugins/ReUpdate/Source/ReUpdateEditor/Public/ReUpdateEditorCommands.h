// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "ReUpdateEditorStyle.h"

class FReUpdateEditorCommands : public TCommands<FReUpdateEditorCommands>
{
public:

	FReUpdateEditorCommands()
		: TCommands<FReUpdateEditorCommands>(TEXT("ReUpdateEditor"), NSLOCTEXT("Contexts", "ReUpdateEditor", "ReUpdateEditor Plugin"), NAME_None, FReUpdateEditorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};