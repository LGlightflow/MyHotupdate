#include "Pak/UnrealPakFile.h"
#include "Settings/ReUnrealPakSettingsEditor.h"
#include "Settings/ReHTTPServerSettingsEditor.h"
#include "Settings/ReOSSServerSettingsEditor.h"
#include "Settings/ReLocalSettingsEditor.h"
#include "Settings/ObjectStorageServerSettingsEditor.h"
#include "Misc/App.h"
#include "Misc/FileHelper.h"
#include "Interfaces/IPluginManager.h"
#include "RePakMethod.hpp"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/ScopedSlowTask.h"
#include "PakFileUtilities.h"


namespace ReUnrealPakEditor
{
	void RecursiveFindDependecies(const FString& PackageName, TArray<FString>& AllDependecies)
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

		TArray<FName> DependeciesArray;
		AssetRegistryModule.GetDependencies(*PackageName, DependeciesArray,
#if (ENGINE_MAJOR_VERSION == 5 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION == 2 && ENGINE_PATCH_VERSION == 7))
			UE::AssetRegistry::EDependencyCategory::Package);
#else
		EAssetRegistryDependencyType::Packages);
#endif
		for (auto& Tmp : DependeciesArray)
		{
			if (Tmp.IsValid())
			{
				FString DependeciesString = Tmp.ToString();
				if (!AllDependecies.Contains(DependeciesString))
				{
					AllDependecies.AddUnique(DependeciesString);
					RecursiveFindDependecies(DependeciesString, AllDependecies);
				}
			}
		}
	}

	void CreateAssetText(const TArray<FString>& ContentAssetPaths, FString& AssetString,const FString &InMountPoint, bool bCook = true)
	{
		FString EngineDir = FPaths::ConvertRelativePathToFull(FPaths::EngineContentDir());
		FString ProjectDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());

		auto IsDir = [](const FString& ConstTmpPaths, const FString& String) ->bool
		{
			return ConstTmpPaths.Find(String) >= 0;
		};

		//拼装路径
		auto AssemblyAssetPath = [&](const FString& InNewPath)->FString
		{
			//D:XXX/DDD/www.xx
			//D:XXX/DDD
			FString NewPath = FPaths::GetPath(InNewPath);

			return FString::Printf(TEXT("\"%s\" %s\r\n"),
				*InNewPath,
				*InMountPoint);
		};

		//文件存在再合并
		auto FileExistsAndCreate = [&AssetString, AssemblyAssetPath](const FString& InNewMyPath)
		{
				//路径存在磁盘时才把一行拼到AssetString
			if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*InNewMyPath))
			{
				AssetString += AssemblyAssetPath(InNewMyPath);
			}
		};

		//把原始字眼路径(如content)替换为Cooked的目录,得到对应的烘焙文件路径
		auto ReplaceContent = [&AssetString, FileExistsAndCreate](const FString& ConstTmpPaths, const FString& CookContent, const FString& Extenstion, bool bEngine = false)
		{
			FString UAssetPaths = ConstTmpPaths.Replace(*(bEngine ?
				FPaths::ConvertRelativePathToFull(FPaths::RootDir())
				: FPaths::ConvertRelativePathToFull(FPaths::ProjectDir())), *CookContent);

			//拼装 UAsset路径
			FileExistsAndCreate(UAssetPaths);
#if ENGINE_MAJOR_VERSION == 5
			FString UbulkPaths = UAssetPaths.Replace(*Extenstion,LexToString(EPackageExtension::BulkDataDefault));
#elif ENGINE_MAJOR_VERSION == 4
			FString UbulkPaths = UAssetPaths.Replace(*Extenstion, TEXT(".ubulk"));
#endif
			FileExistsAndCreate(UbulkPaths);
#if ENGINE_MAJOR_VERSION == 5
			FString UExpPaths = UAssetPaths.Replace(*Extenstion, LexToString(EPackageExtension::Exports));
#elif ENGINE_MAJOR_VERSION == 4
			FString UExpPaths = UAssetPaths.Replace(*Extenstion, TEXT(".uexp"));
#endif
			FileExistsAndCreate(UExpPaths);
		};

		for (const auto& ConstTmpPaths : ContentAssetPaths)
		{
			if (bCook)
			{ //先根据引擎版本和平台拼接 GameCookContent 与 EngineCookContent
#if ENGINE_MAJOR_VERSION == 5
			FString GameCookContent = ProjectDir + TEXT("Saved/Cooked") 
#if PLATFORM_WINDOWS
				/FString(TEXT("Windows"))
#elif PLATFORM_MAC
				/FString(TEXT("Mac"))
#endif // PLATFORM_WINDOWS
				/ FString(FApp::GetProjectName()) / TEXT("");

			FString EngineCookContent = ProjectDir + TEXT("Saved/Cooked")
#if PLATFORM_WINDOWS
				/ FString(TEXT("Windows/"));
#elif PLATFORM_MAC
				/ FString(TEXT("Mac/"));
#else
				/ FString(TEXT(""));
#endif // PLATFORM_WINDOWS

#elif ENGINE_MAJOR_VERSION == 4
			FString GameCookContent = ProjectDir + 
				TEXT("Saved/Cooked") 
#if PLATFORM_WINDOWS
				/ FString(TEXT("WindowsNoEditor"))
#elif PLATFORM_MAC
				/ FString(TEXT("MacNoEditor"))
#endif // PLATFORM_WINDOWS
				/ FString(FApp::GetProjectName()) / TEXT("");


			FString EngineCookContent = ProjectDir + TEXT("Saved/Cooked")
#if PLATFORM_WINDOWS
				/ FString(TEXT("WindowsNoEditor/"));
#elif PLATFORM_MAC
				/ FString(TEXT("MacNoEditor/"));
#else
				/ FString(TEXT(""));
#endif // PLATFORM_WINDOWS

#else
			FString GameCookContent;
			FString EngineCookContent;
#endif // ENGINE_MAJOR_VERSION == 5

				FString Extenstion = FPaths::GetExtension(ConstTmpPaths, true);
				if (IsDir(ConstTmpPaths, EngineDir)) // 引擎目录和项目目录区分开
				{
					ReplaceContent(ConstTmpPaths, EngineCookContent, Extenstion, true);
				}
				else if (IsDir(ConstTmpPaths, ProjectDir))
				{
					ReplaceContent(ConstTmpPaths, GameCookContent, Extenstion);
				}
				else
				{
					check(0);
				}
			}
			else
			{
				AssetString += ConstTmpPaths + TEXT("\r\n");
			}
		}
	}

	void CreateAssetText(const FString& ContentAssetPath, FString& AssetString, bool bCook = true)
	{
		FString ProjectDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());

		FString NewPoint = ContentAssetPath.Replace(TEXT("/Game/"), TEXT("/Content/"));
		FString AssetPath;
		if (bCook)
		{
			FString GameCookContent = ProjectDir + 
				FString(TEXT("Saved/Cooked")) / GetDefault<UReUnrealPakSettingsEditor>()->GetPlatform()
#if ENGINE_MAJOR_VERSION == 4
				+TEXT("NoEditor")
#endif
				/ FApp::GetProjectName();
			AssetPath = GameCookContent + NewPoint;
		}
		else
		{
			// 希望始终为cook = true?
			checkfSlow(0, TEXT("The code needs to check the cook. Check whether the cook is checked. The cook must be true."));
			
			AssetPath = ProjectDir + NewPoint;
			FPaths::NormalizeFilename(AssetPath);
		}

		AssetString = FString::Printf(TEXT("\"%s/*\" %s"),
			*AssetPath,
			*RePak::GetShortMountPoint(NewPoint));
	}

	void ConvertPackagenameToFilename(const TArray<FString>& PackageNames, TArray<FString>& OutFilename)
	{
		const TCHAR* ScriptStirng = TEXT("/Script/");
		const TCHAR* EngineStirng = TEXT("/Engine/");

		for (auto Tmp : PackageNames)
		{
			if (Tmp.StartsWith(EngineStirng))
			{
				continue;
			}
			else if (Tmp.StartsWith(ScriptStirng))
			{
				continue;
			}
			else
			{
				//.uaseet
				FString AssetPackageName = FPackageName::LongPackageNameToFilename(Tmp, FPackageName::GetAssetPackageExtension());
				Tmp = FPaths::ConvertRelativePathToFull(AssetPackageName);
			}

			OutFilename.AddUnique(Tmp);
		}

		TArray<FString> RemoveMap;
		for (const auto& Tmp : OutFilename)
		{
			FString Extension = FPaths::GetExtension(Tmp);
			if (FPackageName::GetMapPackageExtension().Contains(Extension))
			{
				RemoveMap.AddUnique(Tmp);
			}
		}

		//.uasset
		for (auto& Tmp : RemoveMap)
		{
			if (Tmp.RemoveFromEnd(*FPackageName::GetMapPackageExtension()))
			{
				OutFilename.Remove(Tmp + FPackageName::GetAssetPackageExtension());
			}
		}
	}

	FString GetUnrealPakPath()
	{
		FString UnrealPakDir = 
#if PLATFORM_WINDOWS
		TEXT("Engine/Binaries/Win64/UnrealPak.exe")
#elif PLATFORM_MAC
		TEXT("Engine/Binaries/Mac/UnrealPak.app")
#endif		
			;
		FString UnrealPakExePaths = FPaths::ConvertRelativePathToFull(FPaths::RootDir() / UnrealPakDir);
		if (!IFileManager::Get().FileExists(*UnrealPakExePaths))
		{
			UnrealPakExePaths = FPaths::ConvertRelativePathToFull(IPluginManager::Get().FindPlugin("ReHotUpdate")->GetBaseDir() / UnrealPakDir);
		}

		return UnrealPakExePaths;
	}

	bool GeneratedPak_Interior(const FString& ConstTmpPaths,const FString &AssetStringPaths)
	{
		FString SavePakFile = GetDefault<UReUnrealPakSettingsEditor>()->PakSavePaths.Path;

		FString PakNamePaths = SavePakFile / FPaths::GetCleanFilename(ConstTmpPaths) + TEXT(".pak ");

		//是否压缩
		FString CompressParm = "";
		if (GetDefault<UReUnrealPakSettingsEditor>()->bCompress)
		{
			CompressParm = TEXT(" -compress");
		}

		//是否AES加密
		FString AESParm = "";
		if (GetDefault<UReUnrealPakSettingsEditor>()->bAES)
		{
			AESParm = TEXT(" -encrypt -encryptindex -aes=") + GetDefault<UReUnrealPakSettingsEditor>()->AES.ToString();
		}

		//编码格式
		FString UTF8Parm = "";
		if (GetDefault<UReUnrealPakSettingsEditor>()->bForceUTF8)
		{
			UTF8Parm = TEXT(" -UTF8Output");
		}

		//log
		FString AbslogParm = "";
		{
			AbslogParm = TEXT(" -abslog=") + GetDefault<UReUnrealPakSettingsEditor>()->PakLogPaths.Path / TEXT("ReUnrealPakLog.txt");
		}

		FString OrderParm = "";
		if (GetDefault<UReUnrealPakSettingsEditor>()->bOrder)
		{
			OrderParm = TEXT(" -installed -order=") + FPaths::ProjectDir() / TEXT("Build")
#if PLATFORM_WINDOWS
				/ FString(TEXT("Windows/"))/
#elif PLATFORM_MAC
				/ FString(TEXT("Mac/"))/
#endif // PLATFORM_WINDOWS
				
				TEXT("FileOpenOrder/CookerOpenOrder.log");
		}

		//打包平台
		FString PakPlatformParam = TEXT(" -platform=") + GetDefault<UReUnrealPakSettingsEditor>()->GetPlatform();

		//创建参数的集合
		FString CreateParm = PakNamePaths + TEXT("-create=") + AssetStringPaths + AESParm + AbslogParm + CompressParm + UTF8Parm + PakPlatformParam + OrderParm + TEXT(" -multiprocess -patchpaddingalign=2048");

		if (GetDefault<UReUnrealPakSettingsEditor>()->bInternalExecutionPak)
		{
			ExecuteUnrealPak(*CreateParm);
		}
		else
		{
			FString UnrealPakExePaths = GetUnrealPakPath();

			FProcHandle Handle = FPlatformProcess::CreateProc(*UnrealPakExePaths, *CreateParm, false, false, false, nullptr, 0, nullptr, nullptr);
			FPlatformProcess::WaitForProc(Handle);
		}

		return true;
	}

	bool GeneratedPak(const TArray<FString>& ContentAssetPaths, const FString& ConstTmpPaths, bool bMap = false)
	{
		FString MountPoint = bMap ? RePak::GetShortMountPoint(FPaths::GetPath(ConstTmpPaths)) : RePak::GetShortMountPoint(ConstTmpPaths);
		MountPoint.ReplaceInline(TEXT("/Game/"), TEXT("/Content/"));

		//生成我们的资源表
		FString AssetString;
		CreateAssetText(ContentAssetPaths, AssetString, MountPoint, GetDefault<UReUnrealPakSettingsEditor>()->bCooked);
		FString AssetStringPaths = FPaths::ProjectSavedDir() / TEXT("AssetPaths") / FGuid::NewGuid().ToString() + TEXT(".txt");
		FFileHelper::SaveStringToFile(AssetString, *AssetStringPaths, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);

		return GeneratedPak_Interior(ConstTmpPaths, AssetStringPaths);
	}

	bool GeneratedPak(const FString& ConstTmpPaths)
	{
		//生成我们的资源表
		FString AssetString;
		CreateAssetText(ConstTmpPaths,AssetString, GetDefault<UReUnrealPakSettingsEditor>()->bCooked);
		FString AssetStringPaths = FPaths::ProjectSavedDir() / TEXT("AssetPaths") / FGuid::NewGuid().ToString() + TEXT(".txt");
		FFileHelper::SaveStringToFile(AssetString, *AssetStringPaths, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);

		return GeneratedPak_Interior(ConstTmpPaths, AssetStringPaths);
	}

	bool FPak::PakFile(const TArray<FString>& InPaths)
	{
		for (auto &Tmp : InPaths)
		{
			FFormatNamedArguments Args;
			Args.Add(TEXT("PakFile"), FText::FromString(FPaths::GetCleanFilename(Tmp)));
			
			FScopedSlowTask SlowTask(100, FText::Format(NSLOCTEXT("ReUnrealPakEditor", "PakFile_F", "Pak Asset: {PakFile}..."), Args));
			SlowTask.MakeDialog(true);

			SlowTask.EnterProgressFrame(48);
			FPlatformProcess::Sleep(0.2f);

			if (GetDefault<UReUnrealPakSettingsEditor>()->bReferenceAsset && 0)
			{
				check(0);//如果您的项目中非要用到这个功能 一定要检查一下前期的文件管理方面是否存在问题
				FString CoypeTmpPaths = Tmp;
				if (CoypeTmpPaths.RemoveFromStart(TEXT("/Game/")))
				{
					CoypeTmpPaths = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir()) / CoypeTmpPaths;
					
					//寻找该文件下的所有子文件
					TArray<FString> ContentAssetPaths;
					IFileManager::Get().FindFilesRecursive(ContentAssetPaths, *CoypeTmpPaths, TEXT("*"), true, false);

					TArray<FString> AllDependeciesAsset;
					for (auto& TmpString : ContentAssetPaths)
					{
						FString PackageName = FPackageName::FilenameToLongPackageName(TmpString);

						RecursiveFindDependecies(PackageName, AllDependeciesAsset);
					}

					ConvertPackagenameToFilename(AllDependeciesAsset, ContentAssetPaths);
				
					SlowTask.EnterProgressFrame(49);
					FPlatformProcess::Sleep(0.2f);

					if (!GeneratedPak(AllDependeciesAsset, Tmp))
					{

						return false;
					}
				}
			}
			else
			{
				SlowTask.EnterProgressFrame(50);
				FPlatformProcess::Sleep(0.2f);

				if (!GeneratedPak(Tmp))
				{

					return false;
				}
			}

			SlowTask.EnterProgressFrame(1);
			FPlatformProcess::Sleep(0.2f);
		}

		return true;
	}

	bool FPak::PakMapFile(const TArray<FString>& InPaths)
	{
		for (auto& Packagename : InPaths)
		{
			TArray<FString> MapDependeciesAsset;

			//Packagename = /Game/SS/TestMap
			//../../Game/SS/TestMap.umap 
			FString MapPackageName = FPackageName::LongPackageNameToFilename(Packagename, FPackageName::GetMapPackageExtension());

			MapDependeciesAsset.Add(FPaths::ConvertRelativePathToFull(MapPackageName));

			TArray<FString> AllDependeciesAsset;
			RecursiveFindDependecies(Packagename, AllDependeciesAsset);

			ConvertPackagenameToFilename(AllDependeciesAsset, MapDependeciesAsset);

			return GeneratedPak(MapDependeciesAsset, Packagename,true);
		}

		return false;
	}

	void FPak::ListPak(const FString& InPakPath)
	{
		//创建参数的集合
		FString CreateParm = TEXT("-list ") + InPakPath;
		
		if (GetDefault<UReUnrealPakSettingsEditor>()->bInternalExecutionPak)
		{
			ExecuteUnrealPak(*CreateParm);
		}
		else
		{
			FString UnrealPakExePaths = GetUnrealPakPath();

			//读取管线中的内容
			void* ReadPipe = nullptr;
			void* WritePipe = nullptr;
			FPlatformProcess::CreatePipe(ReadPipe, WritePipe);

			FProcHandle Handle = FPlatformProcess::CreateProc(*UnrealPakExePaths, *CreateParm, false, false, false, nullptr, 0, NULL, WritePipe, ReadPipe);
			FPlatformProcess::WaitForProc(Handle);

			FString OutStd = FPlatformProcess::ReadPipe(ReadPipe);
			UE_LOG(LogTemp, Warning, TEXT("%s"), *OutStd);

			FPlatformProcess::ClosePipe(ReadPipe, WritePipe);
		}	
	}
}

