#pragma once

#include "CoreMinimal.h"
#include "HTTP/ReHttpDebugHttp.h"
#include "Version/SimpleVersion.h"
#include "ReHttpDebugType.h"

class ReHttpDEBUG_API FReHttpDebugManage
{
public:
	FReHttpDebugManage();

public:
	//上层调用
	virtual void Tick(float InDeltaTime);

	void SetURL(const FString& InURL) { URL = InURL; }
	FString GetURL() const { return URL; }

	bool Init(const FHotHttpDebugFileInfo& InFileInfo);

	bool CheckHTTPServerLock(const FHotHttpDebugFileInfo& InFileInfo);
	bool HTTPServerLock(const FHotHttpDebugFileInfo& InFileInfo);
	bool HTTPServerUnLock(const FHotHttpDebugFileInfo& InFileInfo);

	bool ProtocolLock(const FHotHttpDebugFileInfo& InFileInfo, EHTTPObjectStorageProtocol InProtocol);

	FString GetHTTPServerVersion(const FHotHttpDebugFileInfo& InFileInfo);
	bool PutServerVersion(const FHotHttpDebugFileInfo& InFileInfo, const FString& ServerVersionJson);
	bool PutServerVersion(const FHotHttpDebugFileInfo& InFileInfo, const FVersion& ServerVersion);

	void Upload(const TArray<FString>& InFiles, const FHotHttpDebugFileInfo& InFileInfo, int32 InShardingSize = 33 * 1024 * 1024);

	int64 GetFileSize(const FString& InURL);
	TArray<uint8> DownloadChunk(const FString& InURL, int64 Start, int64 End);

	//是否在进程中
	bool IsInProcess() const { return !Queues.IsEmpty(); }

	//获取进程百分比
	float GetProcess() const { return TotalNumber == 0 ? 0.f : ((float)Queues.Num() / (float)TotalNumber); }

	//检查是否所有上传已经完成
	bool IsUploadComplete() const { return Queues.IsEmpty(); }

	struct FInnerQueue
	{
		TArray<FString> Files;
		TArray<uint8> FileData;
		int32 ChunkIndex;
		int32 TotalChunks;
		int32 UncompressedSize;
		TArray<uint8> FileJson;
	};

	//将内容上传到服务器使用
	bool Upload_Inner(const FReHttpDebugManage::FInnerQueue& InQueue);

protected:
	void SynchronousRequest(
		const FString& InURL,
		const FString& InBody,
		const TArray<uint8>& InBytes,
		const TMap<FString, FString>& InCustomMetadataHeader,
		ReHttpDebugHTTP::EHTTPVerbType InType,
		TFunction<void(FHttpRequestPtr, FHttpResponsePtr, bool)> InFunc);

protected:
	FString URL;
	TArray<FReHttpDebugManage::FInnerQueue> Queues;
	int32 TotalNumber;
};