
#include "HTTP/ReHttpActionMultpleRequest.h"
#include "Client/HTTPClient.h"
#include "Core/ReHttpMacro.h"
#include "HAL/FileManager.h"
#include "ReHTTPLog.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Math/UnrealMathUtility.h"

FReHttpActionMultpleRequest::FReHttpActionMultpleRequest()
	:Super()
	,RequestNumber(0)
{

}

bool FReHttpActionMultpleRequest::Suspend()
{
	return false;
}

bool FReHttpActionMultpleRequest::Cancel()
{
	for (auto &Tmp : Requests)
	{
		if (Tmp.IsValid())
		{
			FHTTPClient().Cancel(Tmp.ToSharedRef());
		}
	}

	return true;
}

void FReHttpActionMultpleRequest::GetObjects(const TArray<FString> &URL, const FString &SavePaths)
{
	SetPaths(SavePaths);

	for (const auto &Tmp : URL)
	{
		Requests.Add(MakeShareable(new FGetObjectRequest(Tmp)));
		TSharedPtr<IHTTPClientRequest> Request = Requests.Last();

		REQUEST_BIND_FUN(FReHttpActionMultpleRequest)

		if (FHTTPClient().Execute(Request.ToSharedRef()))
		{
			RequestNumber++;

			UE_LOG(LogReHTTP, Log, TEXT("Multple get objects request number %d by multple request."), RequestNumber);
		}
		else
		{
			UE_LOG(LogReHTTP, Error, TEXT("Multple get objects request execution failed."));
		}

		UE_LOG(LogReHTTP, Log, TEXT("GetObjects RequestNumber = %i"), RequestNumber);
	}
}

void FReHttpActionMultpleRequest::GetObjects(const TArray<FString> &URL)
{
	bSaveDisk = false;

	for (const auto &Tmp : URL)
	{
		Requests.Add(MakeShareable(new FGetObjectRequest(Tmp)));
		TSharedPtr<IHTTPClientRequest> Request = Requests.Last();

		REQUEST_BIND_FUN(FReHttpActionMultpleRequest)

		if (FHTTPClient().Execute(Request.ToSharedRef()))
		{
			RequestNumber++;
			UE_LOG(LogReHTTP, Log, TEXT("Multple get objects request number %d by multple request."), RequestNumber);
		}
		else
		{
			UE_LOG(LogReHTTP, Error, TEXT("Multple get objects request execution failed."));
		}

		UE_LOG(LogReHTTP, Log, TEXT("GetObjects RequestNumber = %i"), RequestNumber);
	}
}

void FReHttpActionMultpleRequest::DeleteObjects(const TArray<FString> &URL)
{
	for (const auto &Tmp : URL)
	{
		Requests.Add(MakeShareable(new FDeleteObjectsRequest(Tmp)));
		TSharedPtr<IHTTPClientRequest> Request = Requests.Last();

		REQUEST_BIND_FUN(FReHttpActionMultpleRequest)

		if (FHTTPClient().Execute(Request.ToSharedRef()))
		{
			RequestNumber++;

			UE_LOG(LogReHTTP, Log, TEXT("Multple delete request number %d by multple request."), RequestNumber);
		}
		else
		{
			UE_LOG(LogReHTTP, Error, TEXT("Fail to delete object."));
		}

		UE_LOG(LogReHTTP, Log, TEXT("DeleteObjects RequestNumber = %i"), RequestNumber);
	}
}

bool FReHttpActionMultpleRequest::PutObject(const FString &URL, const FString &LocalPaths)
{
	if (LocalPaths.Len())
	{
		UE_LOG(LogReHTTP, Log, TEXT("Set path %s."), *LocalPaths);
	}
	
	SetPaths(LocalPaths);

	//Start filtering path
	TArray<FString> AllPaths;
	IFileManager::Get().FindFilesRecursive(AllPaths, *LocalPaths, TEXT("*"), true, false);
	
	if (AllPaths.Num())
	{
		for (auto &Tmp : AllPaths)
		{
			UE_LOG(LogReHTTP, Log, TEXT("The uploaded resources are[%s]."), *Tmp);
		}
	}
	else
	{
		UE_LOG(LogReHTTP, Error, TEXT("The obtained content is empty. Please check whether there are resources under this path.[%s]"), *LocalPaths);
		return false;
	}

	for (const auto &Tmp: AllPaths)
	{
		TArray<uint8> ByteData;
		FFileHelper::LoadFileToArray(ByteData, *Tmp);

		FString ObjectName = FPaths::GetCleanFilename(Tmp);

		Requests.Add(MakeShareable(new FPutObjectRequest(URL / ObjectName, ByteData)));
		TSharedPtr<IHTTPClientRequest> Request = Requests.Last();

		REQUEST_BIND_FUN(FReHttpActionMultpleRequest)

		if (FHTTPClient().Execute(Request.ToSharedRef()))
		{
			RequestNumber++;

			UE_LOG(LogReHTTP, Log, TEXT("Multple put object request number %d by multple request."), RequestNumber);
		}
		else
		{
			UE_LOG(LogReHTTP, Error, TEXT("Fail to put object."));
		}

		UE_LOG(LogReHTTP, Log, TEXT("PutObject RequestNumber = %i"), RequestNumber);
	}

	return RequestNumber > 0;
}

void FReHttpActionMultpleRequest::ExecutionCompleteDelegate(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	Super::ExecutionCompleteDelegate(Request, Response, bConnectedSuccessfully);

	if (RequestNumber > 0)
	{
		RequestNumber--;
		if (RequestNumber <= 0)
		{
			AllRequestCompleteDelegate.ExecuteIfBound();
			AllTasksCompletedDelegate.ExecuteIfBound();

			bRequestComplete = true;

			UE_LOG(LogReHTTP, Log, TEXT("The task has been completed."));
		}

		UE_LOG(LogReHTTP, Log, TEXT("ExecutionCompleteDelegate RequestNumber = %i"), RequestNumber);
	}
	else
	{
		UE_LOG(LogReHTTP, Error, TEXT("Request quantity error."));
	}
}

void FReHttpActionMultpleRequest::HttpRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	Super::HttpRequestComplete(Request, Response, bConnectedSuccessfully);
}

void FReHttpActionMultpleRequest::HttpRequestProgress(FHttpRequestPtr Request, int32 BytesSent, int32 BytesReceived)
{
	Super::HttpRequestProgress(Request, BytesSent, BytesReceived);
}

void FReHttpActionMultpleRequest::HttpRequestProgress64(FHttpRequestPtr Request, uint64 BytesSent, uint64 BytesReceived)
{
	Super::HttpRequestProgress64(Request, BytesSent, BytesReceived);
}

void FReHttpActionMultpleRequest::HttpRequestHeaderReceived(FHttpRequestPtr Request, const FString& HeaderName, const FString& NewHeaderValue)
{
	Super::HttpRequestHeaderReceived(Request, HeaderName, NewHeaderValue);
}
