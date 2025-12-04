#pragma once

#include "CoreMinimal.h"
#include "ReHTTPType.generated.h"

UENUM(BlueprintType)
enum class EHTTPRequestType :uint8
{
	SINGLE		UMETA(DisplayName = "Single"),
	MULTPLE		UMETA(DisplayName = "Multple"),
};

UENUM(BlueprintType)
enum class FReHttpStarte :uint8
{
	NotStarted,//ProcessRequest()
	Processing,
	Failed,
	Failed_ConnectionError,
	Succeeded,
};

UENUM(BlueprintType)
enum class EReHTTPVerbType :uint8
{
	Re_POST		UMETA(DisplayName = "Post"),
	Re_PUT		UMETA(DisplayName = "Put"),
	Re_GET		UMETA(DisplayName = "Get"),
	Re_DELETE	UMETA(DisplayName = "Delete")
};

UCLASS(BlueprintType)
class REHTTP_API UReHttpContent :public UObject
{
	GENERATED_BODY()

public:
	TArray<uint8> *Content;

	UFUNCTION(BlueprintPure, Category = "ReHTTP|HttpContent")
	TArray<uint8> &GetContent();

	UFUNCTION(BlueprintCallable, Category = "ReHTTP|HttpContent")
	bool Save(const FString &LocalPath);
};

USTRUCT(BlueprintType)
struct REHTTP_API FReHttpBase
{
	GENERATED_USTRUCT_BODY()

	FReHttpBase()
		:ContentLength(INDEX_NONE)
	{
		RowPtr = nullptr;
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite,Category = "ReHttpBase|ReHttpBase")
	FString URL;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ReHttpBase|ReHttpBase")
	TArray<FString> AllHeaders;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ReHttpBase|ReHttpBase")
	FString ContentType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ReHttpBase|ReHttpBase")
	int32 ContentLength;

	void* RowPtr;
};

USTRUCT(BlueprintType)
struct REHTTP_API FReHttpRequest :public FReHttpBase
{
	GENERATED_USTRUCT_BODY()

	FReHttpRequest()
		:Super()
		, ElapsedTime(INDEX_NONE)
		, Status(FReHttpStarte::NotStarted)
	{}

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ReHttpBase|ReHttpRequest")
	FString Verb;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ReHttpBase|ReHttpRequest")
	float ElapsedTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ReHttpBase|ReHttpRequest")
	FReHttpStarte Status;
};

USTRUCT(BlueprintType)
struct REHTTP_API FReHttpResponse :public FReHttpBase
{
	GENERATED_USTRUCT_BODY()

	FReHttpResponse()
		:Super()
		,ResponseCode(INDEX_NONE)
		,Content(NewObject<UReHttpContent>())
	{}

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ReHttpBase|ReHttpResponse")
	int32 ResponseCode;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ReHttpBase|ReHttpResponse")
	FString ResponseMessage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ReHttpBase|ReHttpResponse")
	TObjectPtr<UReHttpContent> Content;
};

//BP
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FReHttpSingleRequestCompleteDelegate,const FReHttpRequest ,Request,const FReHttpResponse , Response,bool ,bConnectedSuccessfully);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FReHttpSingleRequestProgressDelegate,const FReHttpRequest , Request, uint64, BytesSent, uint64, BytesReceived);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FReHttpSingleRequestHeaderReceivedDelegate, const FReHttpRequest , Request, const FString , HeaderName, const FString , NewHeaderValue);
DECLARE_DYNAMIC_DELEGATE(FAllRequestCompleteDelegate);

//C++
DECLARE_DELEGATE_ThreeParams(FReSingleCompleteDelegate, const FReHttpRequest &, const FReHttpResponse &, bool);
DECLARE_DELEGATE_ThreeParams(FReSingleRequestProgressDelegate, const FReHttpRequest &, uint64, uint64);
DECLARE_DELEGATE_ThreeParams(FReSingleRequestHeaderReceivedDelegate, const FReHttpRequest &, const FString &, const FString &);

USTRUCT(BlueprintType)
struct REHTTP_API FReHTTPBPResponseDelegate
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ReHTTP|HTTPResponseDelegate")
	FReHttpSingleRequestCompleteDelegate			ReHttpRequestCompleteDelegate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ReHTTP|HTTPResponseDelegate")
	FReHttpSingleRequestProgressDelegate			ReHttpRequestProgressDelegate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ReHTTP|HTTPResponseDelegate")
	FReHttpSingleRequestHeaderReceivedDelegate		ReHttpRequestHeaderReceivedDelegate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ReHTTP|HTTPResponseDelegate")
	FAllRequestCompleteDelegate							AllRequestCompleteDelegate;
};

struct REHTTP_API FReHTTPResponseDelegate
{
	FReSingleCompleteDelegate						ReCompleteDelegate;
	FReSingleRequestProgressDelegate				ReSingleRequestProgressDelegate;
	FReSingleRequestHeaderReceivedDelegate			ReSingleRequestHeaderReceivedDelegate;
	FSimpleDelegate										AllTasksCompletedDelegate;
};