// Copyright (C) RenZhai.2020.All Rights Reserved.
#include "ReHTTPManage.h"
#include "HTTP/ReHttpActionMultpleRequest.h"
#include "HTTP/ReHttpActionSingleRequest.h"
#include "Core/ReHttpMacro.h"
#include "Misc/FileHelper.h"
#include "ReHTTPLog.h"
#include "HttpModule.h"
#include "HttpManager.h"

#if PLATFORM_WINDOWS
UE_DISABLE_OPTIMIZATION
#endif

FReHttpManage* FReHttpManage::Instance = NULL;

TSharedPtr<FReHttpActionRequest> GetHttpActionRequest(EHTTPRequestType RequestType)
{
	TSharedPtr<FReHttpActionRequest> HttpObject = nullptr;
	switch (RequestType)
	{
		case EHTTPRequestType::SINGLE:
		{
			HttpObject = MakeShareable(new FReHttpActionSingleRequest());
			UE_LOG(LogReHTTP, Log, TEXT("Action to create a single HTTP request"));
			break;
		}
		case EHTTPRequestType::MULTPLE:
		{
			HttpObject = MakeShareable(new FReHttpActionMultpleRequest());
			UE_LOG(LogReHTTP, Log, TEXT("Action to create a multple HTTP request"));
			break;
		}
	}

	return HttpObject;
}

void FReHttpManage::Tick(float DeltaTime)
{
	FScopeLock ScopeLock(&Instance->Mutex);

	if (!HTTP.bPause)
	{
		FHttpModule::Get().GetHttpManager().Tick(DeltaTime);
	}
	
	TArray<FName> RemoveRequest;
	for (auto &Tmp : HTTP.HTTPMap)
	{
		if (Tmp.Value->IsRequestComplete())
		{
			RemoveRequest.Add(Tmp.Key);
		}
	}

	for (auto &Tmp : RemoveRequest)
	{
		GetHTTP().HTTPMap.Remove(Tmp);

		UE_LOG(LogReHTTP, Log, TEXT("Remove request %s from tick"), *Tmp.ToString());
	}
}

bool FReHttpManage::IsTickableInEditor() const
{
	return true;
}

TStatId FReHttpManage::GetStatId() const
{
	return TStatId();
}

FReHttpManage * FReHttpManage::Get()
{
	if (Instance == nullptr)
	{
		Instance = new FReHttpManage();

		UE_LOG(LogReHTTP, Log, TEXT("Get HTTP management"));
	}

	return Instance;
}

void FReHttpManage::Destroy()
{
	if (Instance != nullptr)
	{
		FScopeLock ScopeLock(&Instance->Mutex);
		delete Instance;		

		UE_LOG(LogReHTTP, Log, TEXT("delete HTTP management"));
	}

	Instance = nullptr;
}

FReHTTPHandle FReHttpManage::FHTTP::RegisteredHttpRequest(
	EHTTPRequestType RequestType ,
	FReHttpSingleRequestCompleteDelegate ReHttpRequestCompleteDelegate /*= FReHttpRequestCompleteDelegate()*/,
	FReHttpSingleRequestProgressDelegate ReHttpRequestProgressDelegate /*= FReHttpRequestProgressDelegate()*/, 
	FReHttpSingleRequestHeaderReceivedDelegate ReHttpRequestHeaderReceivedDelegate /*= FReHttpRequestHeaderReceivedDelegate()*/,
	FAllRequestCompleteDelegate AllRequestCompleteDelegate /*= FAllRequestCompleteDelegate()*/)
{
	FScopeLock ScopeLock(&Instance->Mutex);

	UE_LOG(LogReHTTP, Log, TEXT("Start registering single BP agent."));

	TSharedPtr<FReHttpActionRequest> HttpObject = GetHttpActionRequest(RequestType);
	
	HttpObject->ReHttpRequestCompleteDelegate = ReHttpRequestCompleteDelegate;
	HttpObject->ReHttpRequestHeaderReceivedDelegate = ReHttpRequestHeaderReceivedDelegate;
	HttpObject->ReHttpRequestProgressDelegate = ReHttpRequestProgressDelegate;
	HttpObject->AllRequestCompleteDelegate = AllRequestCompleteDelegate;

	FReHTTPHandle Key = *FGuid::NewGuid().ToString();
	HTTPMap.Add(Key,HttpObject);

	return Key;
}

FReHTTPHandle FReHttpManage::FHTTP::RegisteredHttpRequest(
	EHTTPRequestType RequestType /*= EHTTPRequestType::SINGLE*/, 
	FReSingleCompleteDelegate ReHttpRequestCompleteDelegate /*= nullptr*/, 
	FReSingleRequestProgressDelegate ReHttpRequestProgressDelegate /*= nullptr*/,
	FReSingleRequestHeaderReceivedDelegate ReHttpRequestHeaderReceivedDelegate /*= nullptr*/,
	FSimpleDelegate AllRequestCompleteDelegate /*= nullptr*/)
{
	FScopeLock ScopeLock(&Instance->Mutex);

	UE_LOG(LogReHTTP, Log, TEXT("Start registering single C++ agent."));

	TSharedPtr<FReHttpActionRequest> HttpObject = GetHttpActionRequest(RequestType);

	HttpObject->ReCompleteDelegate = ReHttpRequestCompleteDelegate;
	HttpObject->ReSingleRequestHeaderReceivedDelegate = ReHttpRequestHeaderReceivedDelegate;
	HttpObject->ReSingleRequestProgressDelegate = ReHttpRequestProgressDelegate;
	HttpObject->AllTasksCompletedDelegate = AllRequestCompleteDelegate;

	FReHTTPHandle Key = *FGuid::NewGuid().ToString();
	HTTPMap.Add(Key, HttpObject);

	return Key;
}

TWeakPtr<FReHttpActionRequest> FReHttpManage::FHTTP::Find(const FReHTTPHandle &Handle)
{
	FScopeLock ScopeLock(&Instance->Mutex);

	TWeakPtr<FReHttpActionRequest> Object = nullptr;
	for (auto &Tmp : Instance->GetHTTP().HTTPMap)
	{
		if (Tmp.Key == Handle)
		{
			Object = Tmp.Value;

			UE_LOG(LogReHTTP, Log, TEXT("Http Find at %s"),*(Tmp.Key.ToString()));
			break;
		}
	}

	return Object;
}

bool FReHttpManage::FHTTP::GetObjectToMemory(const FReHTTPHandle &Handle, const FString &URL, bool bSynchronous)
{
	TWeakPtr<FReHttpActionRequest> Object = Find(Handle);
	if (Object.IsValid())
	{
		Object.Pin()->SetAsynchronousState(!bSynchronous);
		return Object.Pin()->GetObject(URL);
	}
	else
	{
		UE_LOG(LogReHTTP, Warning, TEXT("The handle was not found [%s]"), *(Handle.ToString()));
	}

	return false;
}

bool FReHttpManage::FHTTP::GetObjectToMemory(const FReHTTPBPResponseDelegate &BPResponseDelegate, const FString &URL,bool bSynchronous)
{
	RE_HTTP_REGISTERED_REQUEST_BP(EHTTPRequestType::SINGLE);

	return GetObjectToMemory(Handle, URL, bSynchronous);
}

bool FReHttpManage::FHTTP::GetObjectToMemory(const FReHTTPResponseDelegate &BPResponseDelegate, const FString &URL, bool bSynchronous)
{
	RE_HTTP_REGISTERED_REQUEST(EHTTPRequestType::SINGLE);

	return GetObjectToMemory(Handle, URL, bSynchronous);
}

void FReHttpManage::FHTTP::GetObjectsToMemory(const FReHTTPHandle &Handle, const TArray<FString> &URL, bool bSynchronous)
{
	TWeakPtr<FReHttpActionRequest> Object = Find(Handle);
	if (Object.IsValid())
	{
		Object.Pin()->SetAsynchronousState(!bSynchronous);
		Object.Pin()->GetObjects(URL);
	}
	else
	{
		UE_LOG(LogReHTTP, Warning, TEXT("The handle was not found [%s]"), *(Handle.ToString()));
	}
}

void FReHttpManage::FHTTP::GetObjectsToMemory(const FReHTTPBPResponseDelegate &BPResponseDelegate, const TArray<FString> &URL, bool bSynchronous)
{
	RE_HTTP_REGISTERED_REQUEST_BP(EHTTPRequestType::MULTPLE);

	GetObjectsToMemory(Handle, URL, bSynchronous);
}

void FReHttpManage::FHTTP::GetObjectsToMemory(const FReHTTPResponseDelegate &BPResponseDelegate, const TArray<FString> &URL, bool bSynchronous)
{
	RE_HTTP_REGISTERED_REQUEST(EHTTPRequestType::MULTPLE);

	GetObjectsToMemory(Handle, URL, bSynchronous);
}

bool FReHttpManage::FHTTP::GetObjectToLocal(const FReHTTPHandle &Handle, const FString &URL, const FString &SavePaths)
{
	TWeakPtr<FReHttpActionRequest> Object = Find(Handle);
	if (Object.IsValid())
	{
		return Object.Pin()->GetObject(URL, SavePaths);
	}
	else
	{
		UE_LOG(LogReHTTP, Warning, TEXT("The handle was not found [%s]"), *(Handle.ToString()));
	}

	return false;
}

bool FReHttpManage::FHTTP::GetObjectToLocal(const FReHTTPResponseDelegate &BPResponseDelegate, const FString &URL, const FString &SavePaths)
{
	RE_HTTP_REGISTERED_REQUEST(EHTTPRequestType::SINGLE);

	return GetObjectToLocal(Handle, URL, SavePaths);
}

bool FReHttpManage::FHTTP::GetObjectToLocal(const FReHTTPBPResponseDelegate &BPResponseDelegate, const FString &URL, const FString &SavePaths)
{
	RE_HTTP_REGISTERED_REQUEST_BP(EHTTPRequestType::SINGLE);

	return GetObjectToLocal(Handle, URL, SavePaths);
}

void FReHttpManage::FHTTP::GetObjectsToLocal(const FReHTTPHandle &Handle, const TArray<FString> &URL, const FString &SavePaths)
{
	TWeakPtr<FReHttpActionRequest> Object = Find(Handle);
	if (Object.IsValid())
	{
		Object.Pin()->GetObjects(URL, SavePaths);
	}
	else
	{
		UE_LOG(LogReHTTP, Warning, TEXT("The handle was not found [%s]"), *(Handle.ToString()));
	}
}

void FReHttpManage::FHTTP::GetObjectsToLocal(const FReHTTPResponseDelegate &BPResponseDelegate, const TArray<FString> &URL, const FString &SavePaths)
{
	RE_HTTP_REGISTERED_REQUEST(EHTTPRequestType::MULTPLE);

	GetObjectsToLocal(Handle, URL, SavePaths);
}

void FReHttpManage::FHTTP::GetObjectsToLocal(const FReHTTPBPResponseDelegate &BPResponseDelegate, const TArray<FString> &URL, const FString &SavePaths)
{
	RE_HTTP_REGISTERED_REQUEST_BP(EHTTPRequestType::MULTPLE);

	GetObjectsToLocal(Handle, URL, SavePaths);
}

bool FReHttpManage::FHTTP::PutObjectFromBuffer(const FReHTTPHandle &Handle, const FString &URL, const TArray<uint8> &Data)
{
	TWeakPtr<FReHttpActionRequest> Object = Find(Handle);
	if (Object.IsValid())
	{
		return Object.Pin()->PutObject(URL, Data);
	}
	else
	{
		UE_LOG(LogReHTTP, Warning, TEXT("The handle was not found [%s]"), *(Handle.ToString()));
	}

	return false;
}

bool FReHttpManage::FHTTP::PutObjectFromString(const FReHTTPHandle& Handle, const FString& URL, const FString& Buffer, bool bSynchronouse)
{
	TWeakPtr<FReHttpActionRequest> Object = Find(Handle);
	if (Object.IsValid())
	{
		Object.Pin()->SetAsynchronousState(!bSynchronouse);
		return Object.Pin()->PutObjectByString(URL, Buffer);
	}
	else
	{
		UE_LOG(LogReHTTP, Warning, TEXT("The handle was not found [%s]"), *(Handle.ToString()));
	}

	return false;
}

bool FReHttpManage::FHTTP::PutObjectFromBuffer(const FReHTTPBPResponseDelegate &BPResponseDelegate, const FString &URL, const TArray<uint8> &Buffer)
{
	RE_HTTP_REGISTERED_REQUEST_BP(EHTTPRequestType::SINGLE);

	return PutObjectFromBuffer(Handle, URL, Buffer);
}

bool FReHttpManage::FHTTP::PutObjectFromString(const FReHTTPBPResponseDelegate& BPResponseDelegate, const FString& URL, const FString& InBuffer, bool bSynchronouse)
{
	RE_HTTP_REGISTERED_REQUEST_BP(EHTTPRequestType::SINGLE);

	return PutObjectFromString(Handle, URL, InBuffer, bSynchronouse);
}

bool FReHttpManage::FHTTP::PutObjectFromBuffer(const FReHTTPResponseDelegate &BPResponseDelegate, const FString &URL, const TArray<uint8> &Buffer)
{
	RE_HTTP_REGISTERED_REQUEST(EHTTPRequestType::SINGLE);

	return PutObjectFromBuffer(Handle, URL, Buffer);
}

bool FReHttpManage::FHTTP::PutObjectFromString(const FReHTTPResponseDelegate& BPResponseDelegate, const FString& URL, const FString& InBuffer,bool bSynchronouse)
{
	RE_HTTP_REGISTERED_REQUEST(EHTTPRequestType::SINGLE);

	return PutObjectFromString(Handle, URL, InBuffer, bSynchronouse);
}

bool FReHttpManage::FHTTP::PutObjectFromStream(const FReHTTPHandle &Handle, const FString &URL, TSharedRef<FArchive, ESPMode::ThreadSafe> Stream)
{
	TWeakPtr<FReHttpActionRequest> Object = Find(Handle);
	if (Object.IsValid())
	{
		return Object.Pin()->PutObject(URL, Stream);
	}
	else
	{
		UE_LOG(LogReHTTP, Warning, TEXT("The handle was not found [%s]"), *(Handle.ToString()));
	}

	return false;
}

bool FReHttpManage::FHTTP::PutObjectFromStream(const FReHTTPBPResponseDelegate &BPResponseDelegate, const FString &URL, TSharedRef<FArchive, ESPMode::ThreadSafe> Stream)
{
	RE_HTTP_REGISTERED_REQUEST_BP(EHTTPRequestType::SINGLE);

	return PutObjectFromStream(Handle, URL, Stream);
}

bool FReHttpManage::FHTTP::PutObjectFromStream(const FReHTTPResponseDelegate &BPResponseDelegate, const FString &URL, TSharedRef<FArchive, ESPMode::ThreadSafe> Stream)
{
	RE_HTTP_REGISTERED_REQUEST(EHTTPRequestType::SINGLE);

	return PutObjectFromStream(Handle, URL, Stream);
}

bool FReHttpManage::FHTTP::PutObjectFromLocal(const FReHTTPHandle &Handle, const FString &URL, const FString &LocalPaths, bool bSynchronouse)
{
	TWeakPtr<FReHttpActionRequest> Object = Find(Handle);
	if (Object.IsValid())
	{
		Object.Pin()->SetAsynchronousState(!bSynchronouse);
		return Object.Pin()->PutObject(URL, LocalPaths);
	}
	else
	{
		UE_LOG(LogReHTTP, Warning, TEXT("The handle was not found [%s]"), *(Handle.ToString()));
	}

	return false;
}

bool FReHttpManage::FHTTP::PostObjectFromLocal(const FReHTTPHandle& Handle, const FString& URL, const FString& LocalPaths, bool bSynchronouse)
{
	TWeakPtr<FReHttpActionRequest> Object = Find(Handle);
	if (Object.IsValid())
	{
		Object.Pin()->SetAsynchronousState(!bSynchronouse);
		return Object.Pin()->PostObject(URL, LocalPaths);
	}
	else
	{
		UE_LOG(LogReHTTP, Warning, TEXT("The handle was not found [%s]"), *(Handle.ToString()));
	}

	return false;
}

bool FReHttpManage::FHTTP::PutObjectFromLocal(const FReHTTPBPResponseDelegate &BPResponseDelegate, const FString &URL, const FString &LocalPaths,bool bSynchronouse)
{
	RE_HTTP_REGISTERED_REQUEST_BP(EHTTPRequestType::SINGLE);

	return PutObjectFromLocal(Handle, URL, LocalPaths, bSynchronouse);
}

bool FReHttpManage::FHTTP::PutObjectFromLocal(const FReHTTPResponseDelegate &BPResponseDelegate, const FString &URL, const FString &LocalPaths, bool bSynchronouse)
{
	RE_HTTP_REGISTERED_REQUEST(EHTTPRequestType::SINGLE);

	return PutObjectFromLocal(Handle, URL, LocalPaths, bSynchronouse);
}

bool FReHttpManage::FHTTP::PostObjectFromLocal(const FReHTTPResponseDelegate& BPResponseDelegate, const FString& URL, const FString& LocalPaths, bool bSynchronouse)
{
	RE_HTTP_REGISTERED_REQUEST(EHTTPRequestType::SINGLE);

	return PostObjectFromLocal(Handle, URL, LocalPaths, bSynchronouse);
}

bool FReHttpManage::FHTTP::PutObjectsFromLocal(const FReHTTPHandle &Handle, const FString &URL, const FString &LocalPaths, bool bSynchronouse)
{
	return PutObjectFromLocal(Handle, URL, LocalPaths, bSynchronouse);
}

bool FReHttpManage::FHTTP::PutObjectsFromLocal(const FReHTTPBPResponseDelegate &BPResponseDelegate, const FString &URL, const FString &LocalPaths, bool bSynchronouse)
{
	RE_HTTP_REGISTERED_REQUEST_BP(EHTTPRequestType::MULTPLE);

	return PutObjectsFromLocal(Handle, URL, LocalPaths, bSynchronouse);
}

bool FReHttpManage::FHTTP::PutObjectsFromLocal(const FReHTTPResponseDelegate &BPResponseDelegate, const FString &URL, const FString &LocalPaths, bool bSynchronouse)
{
	RE_HTTP_REGISTERED_REQUEST(EHTTPRequestType::MULTPLE);

	return PutObjectsFromLocal(Handle, URL, LocalPaths, bSynchronouse);
}

bool FReHttpManage::FHTTP::PostObjectsFromLocal(const FReHTTPResponseDelegate& BPResponseDelegate, const FString& URL, const FString& LocalPaths, bool bSynchronouse)
{
	RE_HTTP_REGISTERED_REQUEST(EHTTPRequestType::MULTPLE);

	return PostObjectFromLocal(Handle, URL, LocalPaths, bSynchronouse);
}

bool FReHttpManage::FHTTP::DeleteObject(const FReHTTPHandle &Handle, const FString &URL)
{
	TWeakPtr<FReHttpActionRequest> Object = Find(Handle);
	if (Object.IsValid())
	{
		return Object.Pin()->DeleteObject(URL);
	}
	else
	{
		UE_LOG(LogReHTTP, Warning, TEXT("The handle was not found [%s]"), *(Handle.ToString()));
	}

	return false;
}

bool FReHttpManage::FHTTP::DeleteObject(const FReHTTPBPResponseDelegate &BPResponseDelegate, const FString &URL)
{
	RE_HTTP_REGISTERED_REQUEST_BP(EHTTPRequestType::SINGLE);

	return DeleteObject(Handle, URL);
}

bool FReHttpManage::FHTTP::DeleteObject(const FReHTTPResponseDelegate &BPResponseDelegate, const FString &URL)
{
	RE_HTTP_REGISTERED_REQUEST(EHTTPRequestType::SINGLE);

	return DeleteObject(Handle, URL);
}

void FReHttpManage::FHTTP::DeleteObjects(const FReHTTPHandle &Handle, const TArray<FString> &URL)
{
	TWeakPtr<FReHttpActionRequest> Object = Find(Handle);
	if (Object.IsValid())
	{
		Object.Pin()->DeleteObjects(URL);
	}
	else
	{
		UE_LOG(LogReHTTP, Warning, TEXT("The handle was not found [%s]"), *(Handle.ToString()));
	}
}

void FReHttpManage::FHTTP::DeleteObjects(const FReHTTPBPResponseDelegate &BPResponseDelegate, const TArray<FString> &URL)
{
	RE_HTTP_REGISTERED_REQUEST_BP(EHTTPRequestType::MULTPLE);

	DeleteObjects(Handle, URL);
}

void FReHttpManage::FHTTP::DeleteObjects(const FReHTTPResponseDelegate &BPResponseDelegate, const TArray<FString> &URL)
{
	RE_HTTP_REGISTERED_REQUEST(EHTTPRequestType::MULTPLE);

	DeleteObjects(Handle, URL);
}

bool FReHttpManage::FHTTP::PostRequest(const TCHAR *InURL, const TCHAR *InParam, const FReHTTPBPResponseDelegate &BPResponseDelegate, bool bSynchronouse)
{
	RE_HTTP_REGISTERED_REQUEST_BP(EHTTPRequestType::SINGLE);

	return PostRequest(Handle,InURL + FString(TEXT("?")) + InParam);
}

bool FReHttpManage::FHTTP::PostRequest(const FReHTTPHandle &Handle, const FString &URL, bool bSynchronouse)
{
	TWeakPtr<FReHttpActionRequest> Object = Find(Handle);

	if (Object.IsValid())
	{
		Object.Pin()->SetAsynchronousState(!bSynchronouse);
		Object.Pin()->PostObject(URL);

		return true;
	}
	else
	{
		UE_LOG(LogReHTTP, Warning, TEXT("The handle was not found [%s]"), *(Handle.ToString()));
		return false;
	}
}

bool FReHttpManage::FHTTP::PostRequest(const TCHAR *InURL, const TCHAR *InParam,const FReHTTPResponseDelegate &BPResponseDelegate, bool bSynchronouse)
{
	RE_HTTP_REGISTERED_REQUEST(EHTTPRequestType::SINGLE);

	return PostRequest(Handle, InURL + FString(TEXT("?")) + InParam, bSynchronouse);
}

TArray<uint8> NULLArrayUint8;
TArray<uint8> & UReHttpContent::GetContent()
{
	if (Content)
	{
		return *Content;
	}

	return NULLArrayUint8;
}

bool UReHttpContent::Save(const FString &LocalPath)
{
	return FFileHelper::SaveArrayToFile(*Content, *LocalPath);
}

FReHttpManage::FHTTP::FHTTP()
	:bPause(false)
{
}

void FReHttpManage::FHTTP::Suspend()
{
	bPause = true;
}

void FReHttpManage::FHTTP::Awaken()
{
	bPause = false;
}

bool FReHttpManage::FHTTP::Suspend(const FReHTTPHandle& Handle)
{
	TWeakPtr<FReHttpActionRequest> Object = Find(Handle);
	if (Object.IsValid())
	{
		return Object.Pin()->Suspend();
	}

	return false;
}

bool FReHttpManage::FHTTP::Cancel()
{
	bool bCancel = true;
	for (auto& Tmp : Instance->GetHTTP().HTTPMap)
	{
		if (!Tmp.Value->Cancel())
		{
			bCancel = false;
		}
	}

	return bCancel;
}

bool FReHttpManage::FHTTP::Cancel(const FReHTTPHandle& Handle)
{
	TWeakPtr<FReHttpActionRequest> Object = Find(Handle);
	if (Object.IsValid())
	{
		return Object.Pin()->Cancel();
	}

	return false;
}

FReHTTPHandle FReHttpManage::FHTTP::GetHandleByLastExecutionRequest()
{
	return TemporaryStorageHandle;
}

bool FReHttpManage::FHTTP::RequestByConentString(
	EReHTTPVerbType InType,
	const FString& InURL, 
	const TMap<FString, FString>& InHeadMeta, 
	const FString& InContent,
	const FReHTTPBPResponseDelegate& BPResponseDelegate)
{
	RE_HTTP_REGISTERED_REQUEST_BP(EHTTPRequestType::SINGLE);

	return RequestByConentString(Handle, InType, InURL, InHeadMeta, InContent);
}

bool FReHttpManage::FHTTP::RequestByConentByte(EReHTTPVerbType InType, const FString& InURL, const TMap<FString, FString>& InHeadMeta, const TArray<uint8>& InBytes, const FReHTTPBPResponseDelegate& BPResponseDelegate)
{
	RE_HTTP_REGISTERED_REQUEST_BP(EHTTPRequestType::SINGLE);

	return RequestByConentByte(Handle,InType,InURL, InHeadMeta, InBytes);
}

bool FReHttpManage::FHTTP::RequestByConentString(EReHTTPVerbType InType, const FString& InURL, const TMap<FString, FString>& InHeadMeta, const FString& InContent, const FReHTTPResponseDelegate& BPResponseDelegate)
{
	RE_HTTP_REGISTERED_REQUEST(EHTTPRequestType::SINGLE);

	return RequestByConentString(Handle, InType, InURL, InHeadMeta, InContent);
}

bool FReHttpManage::FHTTP::RequestByConentByte(EReHTTPVerbType InType, const FString& InURL, const TMap<FString, FString>& InHeadMeta, const TArray<uint8>& InBytes, const FReHTTPResponseDelegate& BPResponseDelegate)
{
	RE_HTTP_REGISTERED_REQUEST(EHTTPRequestType::SINGLE);

	return RequestByConentByte(Handle, InType, InURL, InHeadMeta, InBytes);
}

bool FReHttpManage::FHTTP::RequestByConentString(
	FReHTTPHandle Handle, 
	EReHTTPVerbType InType, 
	const FString& InURL,
	const TMap<FString, FString>& InHeadMeta,
	const FString& InContent)
{
	TWeakPtr<FReHttpActionRequest> Object = Find(Handle);

	if (Object.IsValid())
	{
		Object.Pin()->SendRequest(InType, InURL, InHeadMeta, InContent);

		return true;
	}
	else
	{
		UE_LOG(LogReHTTP, Warning, TEXT("The handle was not found [%s]"), *(Handle.ToString()));
		return false;
	}
}

bool FReHttpManage::FHTTP::RequestByConentByte(FReHTTPHandle Handle, EReHTTPVerbType InType, const FString& InURL, const TMap<FString, FString>& InHeadMeta, const TArray<uint8>& InBytes)
{
	TWeakPtr<FReHttpActionRequest> Object = Find(Handle);

	if (Object.IsValid())
	{
		Object.Pin()->SendRequest(InType, InURL, InHeadMeta, InBytes);

		return true;
	}
	else
	{
		UE_LOG(LogReHTTP, Warning, TEXT("The handle was not found [%s]"), *(Handle.ToString()));
		return false;
	}
}

#if PLATFORM_WINDOWS
UE_ENABLE_OPTIMIZATION
#endif