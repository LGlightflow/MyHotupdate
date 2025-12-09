


#include "ReHTTPFunctionLibrary.h"
#include "ReHttpManage.h"
#include "HTTP/Core/ReHttpActionRequest.h"
#include "HTTP/Core/ReHTTPHandle.h"

void UReHTTPFunctionLibrary::Pause()
{
	RE_HTTP.Suspend();
}

void UReHTTPFunctionLibrary::Awaken()
{
	RE_HTTP.Awaken();
}

bool UReHTTPFunctionLibrary::Cancel()
{
	return RE_HTTP.Cancel();;
}

bool UReHTTPFunctionLibrary::CancelByHandle(const FName& Handle)
{
	return RE_HTTP.Cancel((FReHTTPHandle)Handle);
}

FName UReHTTPFunctionLibrary::GetHandleByLastExecutionRequest()
{
	return RE_HTTP.GetHandleByLastExecutionRequest();
}

bool UReHTTPFunctionLibrary::PostRequest(const FString &InURL, const FString &InParam, const FReHTTPBPResponseDelegate &BPResponseDelegate)
{
	return RE_HTTP.PostRequest(*InURL,*InParam, BPResponseDelegate);
}

void UReHTTPFunctionLibrary::Tick(float DeltaTime)
{
	FReHttpManage::Get()->Tick(DeltaTime);
}

bool UReHTTPFunctionLibrary::GetObjectToMemory(const FReHTTPBPResponseDelegate &BPResponseDelegate, const FString &URL)
{
	return RE_HTTP.GetObjectToMemory(BPResponseDelegate, URL);
}

bool UReHTTPFunctionLibrary::GetObjectToLocal(const FReHTTPBPResponseDelegate &BPResponseDelegate, const FString &URL, const FString &SavePaths)
{
	return RE_HTTP.GetObjectToLocal(BPResponseDelegate, URL, SavePaths);
}

bool UReHTTPFunctionLibrary::PutObjectFromLocal(const FReHTTPBPResponseDelegate &BPResponseDelegate, const FString &URL, const FString &LocalPaths)
{
	return RE_HTTP.PutObjectFromLocal(BPResponseDelegate, URL, LocalPaths);
}

bool UReHTTPFunctionLibrary::PutObjectFromBuffer(const FReHTTPBPResponseDelegate &BPResponseDelegate, const FString &URL, const TArray<uint8> &Buffer)
{
	return RE_HTTP.PutObjectFromBuffer(BPResponseDelegate, URL, Buffer);
}

bool UReHTTPFunctionLibrary::PutObjectFromString(const FReHTTPBPResponseDelegate& BPResponseDelegate, const FString& URL, const FString& InBuffer)
{
	return RE_HTTP.PutObjectFromString(BPResponseDelegate, URL, InBuffer);
}

bool UReHTTPFunctionLibrary::DeleteObject(const FReHTTPBPResponseDelegate &BPResponseDelegate, const FString &URL)
{
	return RE_HTTP.DeleteObject(BPResponseDelegate, URL);
}

bool UReHTTPFunctionLibrary::PutObjectsFromLocal(const FReHTTPBPResponseDelegate &BPResponseDelegate, const FString &URL, const FString &LocalPaths)
{
	return RE_HTTP.PutObjectsFromLocal(BPResponseDelegate, URL,LocalPaths);
}

void UReHTTPFunctionLibrary::GetObjectsToLocal(const FReHTTPBPResponseDelegate &BPResponseDelegate, const TArray<FString> &URL, const FString &SavePaths)
{
	RE_HTTP.GetObjectsToLocal(BPResponseDelegate, URL, SavePaths);
}

void UReHTTPFunctionLibrary::GetObjectsToMemory(const FReHTTPBPResponseDelegate &BPResponseDelegate, const TArray<FString> &URL)
{
	RE_HTTP.GetObjectsToMemory(BPResponseDelegate, URL);
}

void UReHTTPFunctionLibrary::DeleteObjects(const FReHTTPBPResponseDelegate &BPResponseDelegate, const TArray<FString> &URL)
{
	RE_HTTP.DeleteObjects(BPResponseDelegate, URL);
}

bool UReHTTPFunctionLibrary::RequestByConentString(
	EReHTTPVerbType InType,
	const FString& InURL,
	const TMap<FString, FString>& InHeadMeta,
	const FString& InContent, 
	const FReHTTPBPResponseDelegate& BPResponseDelegate)
{
	return RE_HTTP.RequestByConentString(InType, InURL, InHeadMeta, InContent, BPResponseDelegate);
}

bool UReHTTPFunctionLibrary::RequestByConentByte(
	EReHTTPVerbType InType,
	const FString& InURL, 
	const TMap<FString, FString>& InHeadMeta, 
	const TArray<uint8>& InBytes,
	const FReHTTPBPResponseDelegate& BPResponseDelegate)
{
	return RE_HTTP.RequestByConentByte(InType, InURL, InHeadMeta, InBytes, BPResponseDelegate);
}