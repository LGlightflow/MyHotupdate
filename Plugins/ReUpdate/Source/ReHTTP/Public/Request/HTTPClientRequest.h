
#pragma once

#include "CoreMinimal.h"
#include "Request/RequestInterface.h"
#include "ReHTTPType.h"

namespace ReHTTP
{
	namespace HTTP
	{
		FString HTTPVerbToString(EReHTTPVerbType InVerbType);

		struct FPutObjectRequest :IHTTPClientRequest
		{
			FPutObjectRequest(const FString &URL, const FString& ContentString);
			FPutObjectRequest(const FString &URL, const TArray<uint8>& ContentPayload);
			FPutObjectRequest(const FString &URL, TSharedRef<FArchive, ESPMode::ThreadSafe> Stream);
		};

		struct FGetObjectRequest :IHTTPClientRequest
		{
			FGetObjectRequest(const FString &URL);
		};

		struct FDeleteObjectsRequest :IHTTPClientRequest
		{
			FDeleteObjectsRequest(const FString &URL);
		};

		struct FPostObjectsRequest :IHTTPClientRequest
		{
			FPostObjectsRequest(const FString &URL);
			FPostObjectsRequest(const FString& URL, const FString& ContentString);
			FPostObjectsRequest(const FString& URL, const TArray<uint8>& ContentPayload);
		};

		struct FObjectsRequest :IHTTPClientRequest
		{
			FObjectsRequest(EReHTTPVerbType InType,const FString& URL, const TMap<FString, FString>& InMeta, const FString& InConentData);
			FObjectsRequest(EReHTTPVerbType InType,const FString& URL, const TMap<FString, FString>& InMeta, const TArray<uint8>& InData);
		};
	}
}