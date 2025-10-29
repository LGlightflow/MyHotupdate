#include "ReHttpDebugManage.h"
#include "Json/ReHttpDebugJson.h"
#include "Misc/Base64.h"

#if PLATFORM_WINDOWS
#pragma optimize("",off) 
#endif

FReHttpDebugManage::FReHttpDebugManage()
{
	URL = TEXT("http://127.0.0.1:8555");
}

void FReHttpDebugManage::Tick(float InDeltaTime)
{
	if (Queues.Num() > 0)
	{
		FInnerQueue QueueItem = Queues[0];
		Upload_Inner(QueueItem);

		Queues.RemoveAt(0);
	}
}

bool FReHttpDebugManage::Init(const FHotHttpDebugFileInfo& InFileInfo)
{
	auto RemovePathHead = [](const FString& InPath)->FString
	{
		if (InPath.Contains("."))
		{
			return FPaths::GetCleanFilename(InPath);
		}

		return TEXT("");
	};

	FString Param = FString::Printf(
		TEXT("Protocol=%i&Bucket=%s&PatchVersionLogName=%s&ServerVersionName=%s&VersionLock=%s&Platform=%s"),
		EHTTPObjectStorageProtocol::HSP_INIT,
		*InFileInfo.Bucket,
		*RemovePathHead(InFileInfo.PatchVersionLogName),
		*RemovePathHead(InFileInfo.ServerVersionName),
		*RemovePathHead(InFileInfo.VersionLock),
		*InFileInfo.Platform);

	FString FullURL = URL / TEXT("HotUpdate/Init") + TEXT("?") + Param;

	bool bOk = false;
	SynchronousRequest(
		FullURL,
		TEXT(""),
		TArray<uint8>(),
		TMap<FString, FString>(),
		ReHttpDebugHTTP::EHTTPVerbType::HOTUPDATE_POST,
		[&](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool bSuccess)
		{
			if (bSuccess)
			{
				FString ResponseString = InResponse->GetContentAsString();
				if (ResponseString.Equals(TEXT("OK")))
				{
					bOk = true;
				}
			}
		});

	return bOk;
}

bool FReHttpDebugManage::CheckHTTPServerLock(const FHotHttpDebugFileInfo& InFileInfo)
{
	return ProtocolLock(InFileInfo, EHTTPObjectStorageProtocol::HSP_CHECK_LOCK);
}

bool FReHttpDebugManage::HTTPServerLock(const FHotHttpDebugFileInfo& InFileInfo)
{
	return ProtocolLock(InFileInfo, EHTTPObjectStorageProtocol::HSP_LOCK);
}

bool FReHttpDebugManage::HTTPServerUnLock(const FHotHttpDebugFileInfo& InFileInfo)
{
	return ProtocolLock(InFileInfo, EHTTPObjectStorageProtocol::HSP_UNLOCK);
}

bool FReHttpDebugManage::ProtocolLock(const FHotHttpDebugFileInfo& InFileInfo, EHTTPObjectStorageProtocol InProtocol)
{
	const FString Bucket = InFileInfo.Bucket;
	const FString Platform = InFileInfo.Platform;
	const FString VersionLock = InFileInfo.VersionLock;

	FString Param = FString::Printf(
		TEXT("Protocol=%i&VersionLockPath=%s"),
		InProtocol,
		*(Bucket / Platform / VersionLock));

	FString FullURL = URL / TEXT("HotUpdate/ServerLock") + TEXT("?") + Param;

	bool bOk = false;
	SynchronousRequest(
		FullURL,
		TEXT(""),
		TArray<uint8>(),
		TMap<FString, FString>(),
		ReHttpDebugHTTP::EHTTPVerbType::HOTUPDATE_POST,
		[&](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool bSuccess)
		{
			if (bSuccess)
			{
				FString ResponseString = InResponse->GetContentAsString();
				bOk = ResponseString.ToBool();
			}
		});

	return bOk;
}

FString FReHttpDebugManage::GetHTTPServerVersion(const FHotHttpDebugFileInfo& InFileInfo)
{
	const FString Bucket = InFileInfo.Bucket;
	const FString Platform = InFileInfo.Platform;
	FString ServerVersionName = InFileInfo.ServerVersionName;

	FString Param = FString::Printf(TEXT("ServerVersionPath=%s"), *(Bucket / Platform / ServerVersionName));
	FString FullURL = URL / TEXT("HotUpdate/GetServerVersion") + TEXT("?") + Param;
	FString ResponseString;

	bool bOK = false;
	SynchronousRequest(
		FullURL,
		TEXT(""),
		TArray<uint8>(),
		TMap<FString, FString>(),
		ReHttpDebugHTTP::EHTTPVerbType::HOTUPDATE_GET,
		[&](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool bSuccess)
		{
			if (bSuccess)
			{
				ResponseString = InResponse->GetContentAsString();
			}
		});

	return ResponseString;
}

bool FReHttpDebugManage::PutServerVersion(const FHotHttpDebugFileInfo& InFileInfo, const FString& ServerVersionJson)
{
	const FString Bucket = InFileInfo.Bucket;
	const FString Platform = InFileInfo.Platform;
	const FString ServerVersionName = InFileInfo.ServerVersionName;

	FString Param = FString::Printf(TEXT("ServerVersionPath=%s"), *(Bucket / Platform / ServerVersionName));

	FString FullURL = URL / TEXT("HotUpdate/PutServerVersion") + TEXT("?") + Param;

	bool bOk = false;
	SynchronousRequest(
		FullURL,
		ServerVersionJson,
		TArray<uint8>(),
		TMap<FString, FString>(),
		ReHttpDebugHTTP::EHTTPVerbType::HOTUPDATE_PUT,
		[&](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool bSuccess)
		{
			if (bSuccess)
			{
				FString ResponseString = InResponse->GetContentAsString();
				bOk = ResponseString.ToBool();
			}
		});

	return bOk;
}

bool FReHttpDebugManage::PutServerVersion(const FHotHttpDebugFileInfo& InFileInfo, const FVersion& ServerVersion)
{
	FString JsonString;
	SimpleVersionControl::Save(ServerVersion, JsonString);

	return PutServerVersion(InFileInfo, JsonString);
}

void FReHttpDebugManage::Upload(
	const TArray<FString>& InFiles,
	const FHotHttpDebugFileInfo& InFileInfo,
	int32 InShardingSize)
{
	for (const auto& File : InFiles)
	{
		TArray<uint8> FileData;
		if (FFileHelper::LoadFileToArray(FileData, *File))
		{
			int32 FileSize = FileData.Num();
			int32 NumChunks = (FileSize + InShardingSize - 1) / InShardingSize;

			for (int32 i = 0; i < NumChunks; ++i)
			{
				int32 StartIndex = i * InShardingSize;
				int32 EndIndex = FMath::Min(StartIndex + InShardingSize, FileSize);
				int32 CurrentChunkSize = EndIndex - StartIndex;

				TArray<uint8> Chunk;
				Chunk.Append(&FileData[StartIndex], CurrentChunkSize);

				FInnerQueue& InQueue = Queues.AddDefaulted_GetRef();
				InQueue.Files.Add(File);
				InQueue.FileData = Chunk;
				InQueue.ChunkIndex = i;
				InQueue.TotalChunks = NumChunks;
				InQueue.UncompressedSize = FileSize;

				FString JsonString;
				ReHttpDebugJson::HotHttpDebugFileInfoToJson(InFileInfo, JsonString);

				//序列化
				FMemoryWriter Writer(InQueue.FileJson);
				Writer << JsonString;
			}
		}
	}
}

bool FReHttpDebugManage::Upload_Inner(const FReHttpDebugManage::FInnerQueue& InQueue)
{
	FString FullURL = URL / TEXT("HotUpdate/Upload");

	TArray<uint8> DataToSend;
	DataToSend = InQueue.FileJson;
	DataToSend.Append(InQueue.FileData);

	int32 UncompressedSize = InQueue.UncompressedSize;
	int32 ChunkIndex = InQueue.ChunkIndex;
	int32 TotalChunks = InQueue.TotalChunks;

	TMap<FString, FString> InCustomMetadataHeader;
	InCustomMetadataHeader.Add(TEXT("JsonSize"), FString::FromInt(InQueue.FileJson.Num()));
	InCustomMetadataHeader.Add(TEXT("UncompressedSize"), FString::FromInt(UncompressedSize));
	InCustomMetadataHeader.Add(TEXT("ChunkIndex"), FString::FromInt(ChunkIndex));
	InCustomMetadataHeader.Add(TEXT("TotalChunks"), FString::FromInt(TotalChunks));
	InCustomMetadataHeader.Add(TEXT("Content-Type"), TEXT("application/octet-stream"));

	bool bOk = false;
	SynchronousRequest(
		FullURL,
		TEXT(""),
		DataToSend,
		InCustomMetadataHeader,
		ReHttpDebugHTTP::EHTTPVerbType::HOTUPDATE_POST,
		[&](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool bSuccess)
		{
			if (bSuccess)
			{
				FString ResponseString = InResponse->GetContentAsString();
				if (ResponseString.Equals(TEXT("OK")))
				{
					bOk = true;
				}
			}
		});

	return bOk;
}

int64 FReHttpDebugManage::GetFileSize(const FString& InURL)
{
	int64 FileSize = 0;

	SynchronousRequest(
		InURL,
		TEXT(""),
		TArray<uint8>(),
		TMap<FString, FString>(),
		ReHttpDebugHTTP::EHTTPVerbType::HOTUPDATE_GET,
		[&](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool bSuccess)
		{
			if (bSuccess)
			{
				FileSize = InResponse->GetContentLength();
			}
		});

	return FileSize;
}

TArray<uint8> FReHttpDebugManage::DownloadChunk(const FString& InURL, int64 Start, int64 End)
{
	FString RangeHeader = FString::Printf(TEXT("bytes=%lld-%lld"), Start, End);
	TMap<FString, FString> InCustomMetadataHeader;
	InCustomMetadataHeader.Add(TEXT("Range"), RangeHeader);
	TArray<uint8> ResponseBytes;

	SynchronousRequest(
		InURL,
		TEXT(""),
		TArray<uint8>(),
		InCustomMetadataHeader,
		ReHttpDebugHTTP::EHTTPVerbType::HOTUPDATE_GET,
		[&](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool bSuccess)
		{
			if (bSuccess)
			{
				ResponseBytes = InResponse->GetContent();
				return ResponseBytes;
			}
			else
			{
				TArray<uint8> EmptyArray;
				return EmptyArray;
			}
		});

	return ResponseBytes;
}

void FReHttpDebugManage::SynchronousRequest(
	const FString& InURL,
	const FString& InBody,
	const TArray<uint8>& InBytes,
	const TMap<FString, FString>& InCustomMetadataHeader,
	ReHttpDebugHTTP::EHTTPVerbType InType,
	TFunction<void(FHttpRequestPtr, FHttpResponsePtr, bool)> InFunc)
{
	ReHttpDebugHTTP::FHTTPDelegate InDelegate;
	InDelegate.SimpleCompleteDelegate.BindLambda(
	[&](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
	{
		InFunc(HttpRequest, HttpResponse, bSucceeded);
	});

	TSharedPtr<ReHttpDebugHTTP::FHTTP> TmpHTTP = ReHttpDebugHTTP::FHTTP::CreateHTTPObject(InDelegate);

	if (InBytes.Num() > 0)
	{
		TmpHTTP->Request(
			InURL,
			InBytes,
			InCustomMetadataHeader,
			true,
			InType);
	}
	else
	{
		TmpHTTP->Request(
			InURL,
			InBody,
			InCustomMetadataHeader,
			true,
			InType);
	}
}

#if PLATFORM_WINDOWS
#pragma optimize("",on) 
#endif