#pragma once

#include "CoreMinimal.h"
#include "ReVersion.generated.h"

USTRUCT(BlueprintType)
struct REOBJECT_API FRemoteDataDescribe
{
	GENERATED_USTRUCT_BODY()

	FRemoteDataDescribe();

	FRemoteDataDescribe(int64 InLen, const FString& InLink, const FString& InCrc);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Remote Data")
	int64 Len;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Remote Data")
	FString Link;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Remote Data")
	FString Crc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Remote Data")
	bool bDiscard;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Remote Data")
	bool bMajorVersion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Remote Data")
	FString InstallationPath;
};

inline bool operator==(const FRemoteDataDescribe& L, const FRemoteDataDescribe& R)
{
	return L.Crc == R.Crc && L.Link == R.Link;
}

USTRUCT(BlueprintType)
struct REOBJECT_API FReVersionBase
{
	GENERATED_USTRUCT_BODY()

	FReVersionBase()
		:bDiscard(false)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Version")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Version")
	FString Crc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Version")
	bool bDiscard;

	FORCEINLINE friend bool operator== (const FReVersionBase& A, const FReVersionBase& B)
	{
		return A.Name == B.Name;
	}

	bool IsVaild() const;
};

USTRUCT(BlueprintType)
struct REOBJECT_API FVersionList :public FReVersionBase
{
	GENERATED_USTRUCT_BODY()

	FVersionList(){}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Version")
	TArray<FString> Content;
};

//补丁
USTRUCT(BlueprintType)
struct REOBJECT_API FVersionInfor :public FReVersionBase
{
	GENERATED_USTRUCT_BODY()

	FVersionInfor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Version")
	TArray<FRemoteDataDescribe> Content;

	void Empty();
};

typedef FVersionInfor FVersion;

namespace ReVersionControl
{
	REOBJECT_API void Save(const FVersionList& Infor, FString& Json);
	REOBJECT_API bool Read(const FString& Json, FVersionList& Infor);

	REOBJECT_API void Save(const FVersionInfor& Infor, FString& Json);
	REOBJECT_API bool Read(const FString& Json, FVersionInfor& Infor);

	REOBJECT_API void Save(const FRemoteDataDescribe& Infor, FString& Json);
	REOBJECT_API bool Read(const FString& Json, FRemoteDataDescribe& Infor);
}
