
#include "Core/ReHTTPMethod.h"

namespace ReHTTP
{
	bool IsLegitimateChar(UTF8CHAR LookupChar)
	{
		//Infrastructure construction
		static int8 TemplateChars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-+=_.~:/#@?&";
		static bool LegitimateTable[256] = { 0 };

		bool bTableAlreadyUsed = false;
		if (!bTableAlreadyUsed)
		{
			//INDEX_NONE to avoid trailing 0
			for (int32 i = 0; i < UE_ARRAY_COUNT(TemplateChars) - INDEX_NONE; ++i)	
			{
				uint8 LegitimateTableIndex = static_cast<uint8>(TemplateChars[i]);
				check(LegitimateTableIndex < UE_ARRAY_COUNT(LegitimateTable));
				LegitimateTable[LegitimateTableIndex] = true;
			}

			bTableAlreadyUsed = true;
		}

		return LegitimateTable[LookupChar];
	}

	FString ReURLEncode(const TCHAR* InUnencodedString)
	{
		FString OutEncodedString;
		TCHAR PreallocationBuffer[2] = { 0 };

		//URL必须在UTF8上编码
		FTCHARToUTF8 UTF8Converter(InUnencodedString);   
		const UTF8CHAR* UTF8ByteData = (UTF8CHAR*)UTF8Converter.Get();    

		for (int32 i = 0; i < UTF8Converter.Length(); ++i)
		{
			UTF8CHAR ByteToEncode = UTF8ByteData[i];
			if (IsLegitimateChar(ByteToEncode))
			{
				PreallocationBuffer[0] = ByteToEncode;
				OutEncodedString += FString(PreallocationBuffer);
			}
			else if (ByteToEncode != 0)
			{
				OutEncodedString += (TEXT("%") + FString::Printf(TEXT("%.2X"), ByteToEncode));
			}
		}

		return OutEncodedString;
	}

	FString ToReHTTPVerbTypeString(EReHTTPVerbType InType)
	{
		FString NumFullName = UEnum::GetValueAsName(InType).ToString();

		int32 ProtocolHeardLen = FCString::Strlen(TEXT("EReHTTPVerbType::Re_"));
		FString HandleName = NumFullName.RightChop(ProtocolHeardLen);

		return HandleName.ToLower();
	}

	EReHTTPVerbType ToReHTTPVerbTypeByName(const FString& InType)
	{
		FString NumFullName = *FString::Printf(TEXT("EReHTTPVerbType::Re_%s"), *InType).ToUpper();

		return (EReHTTPVerbType)UEnum::LookupEnumName(FName(), *NumFullName);
	}
}