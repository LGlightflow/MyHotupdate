
#include "Request/HTTPClientRequest.h"
#include "Core/ReHttpMacro.h"
#include "ReHTTPLog.h"
#include "Core/ReHTTPMethod.h"

ReHTTP::HTTP::FPutObjectRequest::FPutObjectRequest(const FString &URL, const FString& ContentString)
{
	DEFINITION_HTTP_TYPE(PUT,"multipart/form-data;charset=utf-8")
	HttpReuest->SetContentAsString(ContentString);

	UE_LOG(LogReHTTP, Log, TEXT("PUT Action as string"));
}

ReHTTP::HTTP::FPutObjectRequest::FPutObjectRequest(const FString &URL, TSharedRef<FArchive, ESPMode::ThreadSafe> Stream)
{
	DEFINITION_HTTP_TYPE(PUT, "multipart/form-data;charset=utf-8")
	HttpReuest->SetContentFromStream(Stream);

	UE_LOG(LogReHTTP, Log, TEXT("PUT Action by stream."));
}

ReHTTP::HTTP::FPutObjectRequest::FPutObjectRequest(const FString &URL,const TArray<uint8>& ContentPayload)
{
	DEFINITION_HTTP_TYPE(PUT, "multipart/form-data;charset=utf-8")
	HttpReuest->SetContent(ContentPayload);

	UE_LOG(LogReHTTP, Log, TEXT("PUT Action by content."));
}

//Get: get resources from the server
//Post: create a new resource on the server.
//Put: update resources on the server and provide the changed complete resources on the client
//Patch: update resources on the server and provide changed properties on the client
//Delete: delete a resource from the server.
//Head: get the metadata of the resource.
//Options: get information about which attributes of the resource can be changed by the client.

ReHTTP::HTTP::FGetObjectRequest::FGetObjectRequest(const FString &URL)
{
	DEFINITION_HTTP_TYPE(GET, "application/x-www-form-urlencoded;charset=utf-8")

	UE_LOG(LogReHTTP, Log, TEXT("GET Action."));
}

ReHTTP::HTTP::FDeleteObjectsRequest::FDeleteObjectsRequest(const FString &URL)
{
	DEFINITION_HTTP_TYPE(DELETE, "application/x-www-form-urlencoded;charset=utf-8")

	UE_LOG(LogReHTTP, Log, TEXT("DELETE Action."));
}

ReHTTP::HTTP::FPostObjectsRequest::FPostObjectsRequest(const FString &URL)
{
	DEFINITION_HTTP_TYPE(POST, "application/x-www-form-urlencoded;charset=utf-8")

	UE_LOG(LogReHTTP, Log, TEXT("POST Action."));
}

ReHTTP::HTTP::FPostObjectsRequest::FPostObjectsRequest(const FString& URL, const FString& ContentString)
{
	DEFINITION_HTTP_TYPE(POST,"multipart/form-data;charset=utf-8")
	HttpReuest->SetContentAsString(ContentString);

	UE_LOG(LogReHTTP, Log, TEXT("Post Action as string"));
}

ReHTTP::HTTP::FPostObjectsRequest::FPostObjectsRequest(const FString& URL, const TArray<uint8>& ContentPayload)
{
	DEFINITION_HTTP_TYPE(POST,"multipart/form-data;charset=utf-8")
	HttpReuest->SetContent(ContentPayload);

	UE_LOG(LogReHTTP, Log, TEXT("Post Action as Byte"));
}

ReHTTP::HTTP::FObjectsRequest::FObjectsRequest(EReHTTPVerbType InType, const FString& URL, const TMap<FString, FString>& InMeta, const FString& InConentData)
{
	FString InNewURLEncoded = ReHTTP::ReURLEncode(*URL);

	HttpReuest->SetURL(URL);
	HttpReuest->SetVerb(HTTPVerbToString(InType));

	for (auto &Tmp : InMeta)
	{
		HttpReuest->SetHeader(Tmp.Key, Tmp.Value);
	}

	HttpReuest->SetContentAsString(InConentData);	
}

ReHTTP::HTTP::FObjectsRequest::FObjectsRequest(EReHTTPVerbType InType, const FString& URL, const TMap<FString, FString>& InMeta, const TArray<uint8>& InData)
{
	FString InNewURLEncoded = ReHTTP::ReURLEncode(*URL);

	HttpReuest->SetURL(InNewURLEncoded);
	HttpReuest->SetVerb(HTTPVerbToString(InType));

	for (auto& Tmp : InMeta)
	{
		HttpReuest->SetHeader(Tmp.Key, Tmp.Value);
	}

	HttpReuest->SetContent(InData);
}

FString ReHTTP::HTTP::HTTPVerbToString(EReHTTPVerbType InVerbType)
{
	switch (InVerbType)
	{
	case EReHTTPVerbType::Re_POST:
		return TEXT("POST");
	case EReHTTPVerbType::Re_PUT:
		return TEXT("PUT");
	case EReHTTPVerbType::Re_GET:
		return TEXT("GET");
	case EReHTTPVerbType::Re_DELETE:
		return TEXT("DELETE");
	}

	return TEXT("POST");
}
