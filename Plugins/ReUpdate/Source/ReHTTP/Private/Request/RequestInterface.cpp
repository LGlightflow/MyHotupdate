// Copyright (C) RenZhai.2020.All Rights Reserved.

#include "Request/RequestInterface.h"
#include "HttpModule.h"
#include "ReHTTPLog.h"

ReHTTP::HTTP::IHTTPClientRequest::IHTTPClientRequest()
	:HttpReuest(FHttpModule::Get().CreateRequest())
{

}

bool ReHTTP::HTTP::IHTTPClientRequest::ProcessRequest()
{
	UE_LOG(LogReHTTP, Log, TEXT("Process Request."));

	return HttpReuest->ProcessRequest();
}

void ReHTTP::HTTP::IHTTPClientRequest::CancelRequest()
{
	UE_LOG(LogReHTTP, Log, TEXT("Cancel Request."));

	HttpReuest->CancelRequest();
}

