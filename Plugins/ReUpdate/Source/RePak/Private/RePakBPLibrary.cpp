#include "RePakBPLibrary.h"
#include "RePak.h"
#include "IPlatformFilePak.h"
#include "RePakMethod.hpp"
#include "Log/RePakLog.h"
#include "Blueprint/UserWidget.h"

FRePakConfig HotPakConfig;

FPakPlatformFile* URePakBPLibrary::GetPakPlatformFile()
{
	FRePakModule& SimpleUnrealPakModule = FModuleManager::LoadModuleChecked<FRePakModule>(TEXT("RePak"));
	return SimpleUnrealPakModule.GetPakPlatformFile();
}

void URePakBPLibrary::InitConfig(const FRePakConfig& InConfig)
{
	HotPakConfig = InConfig;
}

void URePakBPLibrary::GetFilenamesInPakFile(const FString& InPakFilename, TArray<FString>& OutFileList)
{
	GetPakPlatformFile()->GetPrunedFilenamesInPakFile(InPakFilename, OutFileList);
}

bool URePakBPLibrary::IsMounted(const FString& PakFilename)
{
	if (FPakPlatformFile* InPakFile = GetPakPlatformFile())
	{
		TArray<FString> PakFilenames;
		InPakFile->GetMountedPakFilenames(PakFilenames);

		return PakFilenames.Contains(PakFilename);
	}

	return false;
}

TArray<FString> URePakBPLibrary::GetMountedReturnPakFilenames()
{
	if (FPakPlatformFile* InPakFile = GetPakPlatformFile())
	{
		TArray<FString> PakFilenames;
		InPakFile->GetMountedPakFilenames(PakFilenames);

		return PakFilenames;
	}

	return TArray<FString>();
}

void URePakBPLibrary::GetMountedPakFilenames(TArray<FString>& OutPakFilenames)
{
	if (FPakPlatformFile* InPakFile = GetPakPlatformFile())
	{
		InPakFile->GetMountedPakFilenames(OutPakFilenames);
	}
}

bool URePakBPLibrary::FindFile(const FString& Directory, TArray<FString>& File, bool bRecursively)
{
	class FSimplePakFindFile :public IPlatformFile::FDirectoryVisitor
	{
	public:
		FSimplePakFindFile(TArray<FString>& VisitFiles)
			:VisitFiles(VisitFiles)
		{}

		/**
		 * Callback for a single file or a directory in a directory iteration.
		 * @param FilenameOrDirectory		If bIsDirectory is true, this is a directory (with no trailing path delimiter), otherwise it is a file name.
		 * @param bIsDirectory				true if FilenameOrDirectory is a directory.
		 * @return							true if the iteration should continue.
		**/
		virtual bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory)
		{
			if (!bIsDirectory)
			{
				VisitFiles.Add(FilenameOrDirectory);
			}

			return true;
		}

		TArray<FString>& VisitFiles;
	};

	FSimplePakFindFile Visitor(File);

	if (bRecursively)
	{
		return GetPakPlatformFile()->IterateDirectoryRecursively(*Directory, Visitor);
	}

	return GetPakPlatformFile()->IterateDirectory(*Directory, Visitor);
}

URePakBPLibrary::URePakBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

bool URePakBPLibrary::MountPak(const FString& PakFilename, int32 PakOrder, const FString& MountPath, bool bSelfAdaptionMountProjectPath)
{
	if (FPakPlatformFile *InPakFile = GetPakPlatformFile())
	{
		if (!IsMounted(PakFilename))
		{
			//解密
			RePak::AESDecrypt(HotPakConfig);

			//这个必须要这么做,因为我们希望不同的项目打包出来的pak依然可以安装到其他项目中
			FString NewMountPath = MountPath;
			if (NewMountPath.IsEmpty())
			{
				if (bSelfAdaptionMountProjectPath)
				{
#if (ENGINE_MAJOR_VERSION == 5 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION == 2 && ENGINE_PATCH_VERSION > 6))
					TRefCountPtr<FPakFile> PakFile = new FPakFile(InPakFile, *PakFilename, false, true);
					if (PakFile.GetReference()->IsValid())
					{
						//重新设置安装路径
						NewMountPath = RePak::GetShortMountPoint(PakFile->GetMountPoint());
					}

					PakFile.SafeRelease();

#else
					FPakFile *PakFile = new FPakFile(InPakFile, *PakFilename, false);
					if (PakFile->IsValid())
					{
						//重新设置安装路径
						NewMountPath = RePak::GetShortMountPoint(PakFile->GetMountPoint());
					}

					delete PakFile;
#endif		
				}
			}
			
			return InPakFile->Mount(*PakFilename, PakOrder, NewMountPath.Len() > 0 ? *NewMountPath : NULL);
		}
	}

	return false;
}

bool URePakBPLibrary::UnmountPak(const FString& PakFilename)
{
	if (FPakPlatformFile* InPakFile = GetPakPlatformFile())
	{
		return InPakFile->Unmount(*PakFilename);
	}

	return false;
}

UObject* URePakBPLibrary::GetObjectFromPak(const FString& Filename)
{
	return RePak::StaticLoadObjectFromPak<UObject>(Filename);;
}

UClass* URePakBPLibrary::GetClassFromPak(const FString& Filename,const FString & InSuffix, const FString& InPrefix)
{
	return RePak::StaticLoadObjectFromPak<UClass>(Filename,
		(!InSuffix.IsEmpty() ? *InSuffix : NULL),
		(!InPrefix.IsEmpty() ? *InPrefix : NULL));
}

UMaterial* URePakBPLibrary::GetMaterialFromPak(const FString& Filename)
{
	return Cast<UMaterial>(GetObjectFromPak(Filename));
}

UTexture2D* URePakBPLibrary::GetTexture2DFromPak(const FString& Filename)
{
	return Cast<UTexture2D>(GetObjectFromPak(Filename));
}

USkeletalMesh* URePakBPLibrary::GetSkeletalMeshFromPak(const FString& Filename)
{
	return Cast<USkeletalMesh>(GetObjectFromPak(Filename));
}

UStaticMesh* URePakBPLibrary::GetStaticMeshFromPak(const FString& Filename)
{
	return Cast<UStaticMesh>(GetObjectFromPak(Filename));
}

AActor* URePakBPLibrary::GetActorFromPak(UObject* WorldContextObject, const FString& Filename, const FVector& InLocation, const FRotator& InRotator)
{
	UWorld* World = Cast<UWorld>(WorldContextObject);
	if (!World)
	{
		World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	}

	if (World)
	{
		if (UClass* InGeneratedClass = GetClassFromPak(Filename,TEXT("_C"), TEXT("Blueprint")))
		{
			return World->SpawnActor<AActor>(InGeneratedClass, InLocation, InRotator);
		}
	}

	return nullptr;
}

UUserWidget* URePakBPLibrary::GetUMGFromPak(UObject* WorldContextObject, const FString& Filename)
{
	UWorld* World = Cast<UWorld>(WorldContextObject);
	if (!World)
	{
		World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	}

	if (World)
	{
		if (TSubclassOf<UUserWidget> InGeneratedClass = GetClassFromPak(Filename,TEXT("_C"),TEXT("WidgetBlueprint")))
		{
			return CreateWidget<UUserWidget>(World,InGeneratedClass);
		}
	}

	return nullptr;
}

