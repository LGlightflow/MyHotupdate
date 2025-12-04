// Copyright (C) RenZhai.2020.All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ReHTTPType.h"

namespace ReHTTP
{
	//如果里面包含中文字符，会进行特殊处理，防止字符因为特殊导致HTTP错误
	FString REHTTP_API ReURLEncode(const TCHAR* InUnencodedString);

	FString REHTTP_API ToReHTTPVerbTypeString(EReHTTPVerbType InType);
	EReHTTPVerbType REHTTP_API ToReHTTPVerbTypeByName(const FString &InType);
}