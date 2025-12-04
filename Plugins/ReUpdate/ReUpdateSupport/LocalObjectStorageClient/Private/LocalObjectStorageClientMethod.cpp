// Copyright (C) RenZhai.2020.All Rights Reserved.
#include "LocalObjectStorageClientMethod.h"
#include "LocalObjectStorageClientLog.h"

namespace LocalObjectStorageClientMethod
{
	FString GetParseValue(const FString& InKey)
	{
		FString Value;
		if (!FParse::Value(FCommandLine::Get(), *InKey, Value))
		{
			UE_LOG(LogLocalObjectStorageClient, Error, TEXT("%s was not found value"), *InKey);
		}

		return Value;
	}

	struct FLocalPutProgress :public FCopyProgress
	{
		virtual bool Poll(float Fraction)
		{
			//UE_LOG(LogTemp, Display, TEXT("%f"), Fraction);

			return true;
		}
	};

	bool LocalPut(const FString& InSource, const FString& InTarget)
	{
		FLocalPutProgress PutProgress;
		return IFileManager::Get().Copy(*InTarget, *InSource, true, false, false, &PutProgress) == ECopyResult::COPY_OK;
	}
}

