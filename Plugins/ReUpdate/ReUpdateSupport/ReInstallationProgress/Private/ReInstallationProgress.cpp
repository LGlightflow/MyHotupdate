// Copyright Epic Games, Inc. All Rights Reserved.

#include "SimpleInstallationProgress.h"
#include "RequiredProgramMainCPPInclude.h"
#include "StandaloneRenderer.h"
#include "Core/Widget/SMainScreen.h"
#include "ThreadManage.h"
#include "SimpleInstallationProgressType.h"
#include "Core/Installation/SimpleInstallation.h"
#include "Core/Log/SimpleInstallationProgressLog.h"
#include "Core/Style/SimpleInstallationProgressStyle.h"

IMPLEMENT_APPLICATION(SimpleInstallationProgress, "SimpleInstallationProgress");

#define LOCTEXT_NAMESPACE "SimpleInstallationProgress"

int32 RunSimpleInstallationProgress(const TCHAR* CommandLine)
{
	//I.引擎初始化
	GEngineLoop.PreInit(CommandLine);

	//II.命令初始化
	SimpleInstallation::InitCommandInstallationProgress();

	//III.UObject对象初始化
	ProcessNewlyLoadedUObjects();
	FModuleManager::Get().StartProcessingNewlyLoadedObjects();

	//V.应用程序渲染初始化
	FSlateApplication::InitializeAsStandaloneApplication(GetStandardStandaloneRenderer());

	//VI.DPI初始化
	FSlateApplication::InitHighDPI(true);

	//V.初始化我们的资源
	FSimpleInstallationProgressStyle::Initialize();
	FSimpleInstallationProgressStyle::ReloadTextures();

	//VI.生成自定义的画面
	SimpleInstallation::SpawnInstallationProgressUI();

	//VII.异步运行资源的拷贝
	GThread::Get()->GetProxy().CreateLambda([&]()
	{
		SimpleInstallation::Run();
	});

	//VII.渲染和Tick
	while (!IsEngineExitRequested())
	{
		BeginExitIfRequested();
		FTaskGraphInterface::Get().ProcessThreadUntilIdle(ENamedThreads::GameThread);
		FTicker::GetCoreTicker().Tick(FApp::GetDeltaTime());
		FSlateApplication::Get().PumpMessages();
		FSlateApplication::Get().Tick();
		GThread::Get()->Tick(FApp::GetDeltaTime());
		FPlatformProcess::Sleep(0.01);
		GFrameCounter++;
	}

	//VIII.结束引擎
	FSimpleInstallationProgressStyle::Shutdown();
	FCoreDelegates::OnExit.Broadcast();
	FSlateApplication::Shutdown();
	FModuleManager::Get().UnloadModulesAtShutdown();
	GThread::Destroy();

	GEngineLoop.AppPreExit();
	GEngineLoop.AppExit();

	return 0;
}


#undef LOCTEXT_NAMESPACE