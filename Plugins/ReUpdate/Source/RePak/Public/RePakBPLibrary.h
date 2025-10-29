#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "RePakType.h"
#include "RePakBPLibrary.generated.h"

class UUserWidget;
class FPakPlatformFile;
UCLASS()
class REPAK_API URePakBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	static FPakPlatformFile* GetPakPlatformFile();

public:
	/*
	* Initialize our configuration, which is related to our AES encryption.
	* @InConfig       It's about encrypted AES.
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetPakInfo", Keywords = "IFO"), Category = "ReUpdate")
	static void InitConfig(const FRePakConfig& InConfig);
	
	/*
	* Initialize our configuration, which is related to our AES encryption.
	* @InPakFilename According to the installation path of Pak, such as "../../yourpath/xxx.pak".
	* @OutFileList   Get the resource image path inside the Pak .
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetPakInfo", Keywords = "IFO"), Category = "ReUpdate")
	static void GetFilenamesInPakFile(const FString& InPakFilename, TArray<FString>& OutFileList);

	/*
	* Has the package been installed.
	* @PakFilename	  Name of the package.
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetPakInfo", Keywords = "IFO"), Category = "ReUpdate")
	static bool IsMounted(const FString& PakFilename);

	/*
	* You can get the names of all pak that have been installed so far .
	* @Return		Returns the name of the installation package.
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetMountedFilenames", Keywords = "PakInfo"), Category = "ReUpdate")
	static TArray<FString> GetMountedReturnPakFilenames();

	/*
	* You can get the names of all pak that have been installed so far .
	* @Return		Returns the name of the installation package.
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetMountedFilenames", Keywords = "PakInfo"), Category = "ReUpdate")
	static void GetMountedPakFilenames(TArray<FString> &OutPakFilenames);

	/*
	* Search sub files according to the specified path .
	* @Directory	Path, file path,such as ../../xxx/xxx1/ .
	* @File         Get the found file .
	* @bRecursively Recursive search .
	* return		If it's true, it's found .
	*/
	UFUNCTION(BlueprintCallable, meta = (Keywords = "FF"), Category = "ReUpdate")
	static bool FindFile(const FString& Directory, TArray<FString>& File, bool bRecursively = true);



	/*
	* Install a path that can be manually installed,It is usually based on maps or special props.
	* @PakFilename   According to the installation path of Pak, such as "../../yourpath/xxx.pak".
	* @PakOrder		 This is related to the Pak of the project itself
							1 ProjectSavedDir.
							2 EngineContentDir.
							3 ProjectContentDir.
							4 ../Content/Paks/ProjectName.
	* @MountPath	 Path of package installation  .
	*/
	UFUNCTION(BlueprintCallable, meta = (Keywords = "M"), Category = "ReUpdate")
	static bool MountPak(const FString& PakFilename, int32 PakOrder, const FString& MountPath,bool bSelfAdaptionMountProjectPath = false);

	/*
	* You can manually uninstall the path.
	* @PakFilename	Pak path ,such as "../../yourpath/xxx.pak".
	*/
	UFUNCTION(BlueprintCallable, meta = (Keywords = "UM"), Category = "ReUpdate")
	static bool UnmountPak(const FString& PakFilename);

	/*
	* Getting objects from Pak ,If the Pak is already Mounted .
	* @Filename				The file format is similar to the following ："/Game/Mesh/YourAssetName" .
	*/
	UFUNCTION(BlueprintPure, meta = (Keywords = "OB"), Category = "ReUpdate")
	static UObject* GetObjectFromPak(const FString& Filename);

	/*
	* Get the class from the Pak file  ,If the Pak is already Mounted .
	* @Filename				The file format is similar to the following ："/Game/Mesh/YourAssetName" .
	*/
	UFUNCTION(BlueprintPure, meta = (Keywords = "GC"), Category = "ReUpdate")
	static UClass* GetClassFromPak(const FString& Filename, const FString& InSuffix = TEXT(""), const FString& InPrefix = TEXT(""));
	
	/*
	* Getting Material from Pak  ,If the Pak is already Mounted .
	* @Filename				The file format is similar to the following ："/Game/Mesh/YourAssetName" .
	* @Type					Is the resource type patch or DLC ..
	*/
	UFUNCTION(BlueprintPure, meta = (Keywords = "GM"), Category = "ReUpdate")
	static UMaterial* GetMaterialFromPak(const FString& Filename);

	/*
	* Getting Texture 2D from Pak  ,If the Pak is already Mounted .
	* @Filename				The file format is similar to the following ："/Game/Mesh/YourAssetName" .
	* @Type					Is the resource type patch or DLC ..
	*/
	UFUNCTION(BlueprintPure, meta = (Keywords = "GT"), Category = "ReUpdate")
	static UTexture2D* GetTexture2DFromPak(const FString& Filename);
	
	/*
	* Getting Skeletal Mesh from Pak  ,If the Pak is already Mounted .
	* @Filename				The file format is similar to the following ："/Game/Mesh/YourAssetName" .
	* @Type					Is the resource type patch or DLC ..
	*/
	UFUNCTION(BlueprintPure, meta = (Keywords = "GS"), Category = "ReUpdate")
	static USkeletalMesh* GetSkeletalMeshFromPak(const FString& Filename);
	
	/*
	* Getting static mesh from Pak  ,If the Pak is already Mounted .
	* @Filename				The file format is similar to the following ："/Game/Mesh/YourAssetName" .
	* @Type					Is the resource type patch or DLC ..
	*/
	UFUNCTION(BlueprintPure, meta = (Keywords = "GSM"), Category = "ReUpdate")
	static UStaticMesh* GetStaticMeshFromPak(const FString& Filename);
	
	/*
	* Can be directly loaded into the Pak object and generated  ,If the Pak is already Mounted .
	* @WorldContextObject	If it is a blueprint, this parameter can be ignored. If it is C++,
							a world is needed because the current object needs to be generated in a world .
	* @Filename				The file format is similar to the following ："/Game/Mesh/YourAssetName" .
	* @InLocation			A character needs a place .
	* @InRotator			A character needs a rotation default .
	*/
	UFUNCTION(BlueprintPure, meta = (Keywords = "OB", Category = "ReUpdate", WorldContext = "WorldContextObject"))
	static AActor* GetActorFromPak(UObject* WorldContextObject, const FString& Filename, const FVector& InLocation = FVector::ZeroVector, const FRotator& InRotator = FRotator::ZeroRotator);

	/*
	* Can be directly loaded into the Pak object and generated  ,If the Pak is already Mounted .
	* @WorldContextObject	If it is a blueprint, this parameter can be ignored. If it is C++,
							a world is needed because the current object needs to be generated in a world .
	* @Filename				The file format is similar to the following ："/Game/Mesh/YourAssetName" .
	* @InLocation			A character needs a place .
	* @InRotator			A character needs a rotation default .
	*/
	UFUNCTION(BlueprintPure, meta = (Keywords = "UMGWidget", Category = "ReUpdate", WorldContext = "WorldContextObject"))
	static UUserWidget* GetUMGFromPak(UObject* WorldContextObject,const FString& Filename);
};
