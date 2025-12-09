

#include "Client/HTTPClient.h"

ReHTTP::HTTP::FHTTPClient::FHTTPClient()
{

}

ReHTTP::HTTP::FHTTPClient::FHTTPClient(TSharedPtr<IHTTPClientRequest> InHTTPRequest)
{
	Execute(InHTTPRequest.ToSharedRef());
}

bool ReHTTP::HTTP::FHTTPClient::Execute(TSharedRef<IHTTPClientRequest> InHTTPRequest) const
{
	return InHTTPRequest->ProcessRequest();
}

void ReHTTP::HTTP::FHTTPClient::Cancel(TSharedRef<IHTTPClientRequest> InHTTPRequest)
{
	InHTTPRequest->CancelRequest();
}
