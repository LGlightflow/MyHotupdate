#include "Json/ReHttpDebugJson.h"
#include "Json.h"
#include "Misc/Base64.h"

namespace ReHttpDebugJson
{
	void HotHttpDebugFileInfoToJson(const FHotHttpDebugFileInfo& InFileInfo, FString& OutJsonString)
	{
		TSharedPtr<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter =
		TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutJsonString);

		JsonWriter->WriteObjectStart();
		{
			JsonWriter->WriteValue(TEXT("FileName"), InFileInfo.FileName);
			JsonWriter->WriteValue(TEXT("VersionName"), InFileInfo.VersionName);
			JsonWriter->WriteValue(TEXT("Bucket"), InFileInfo.Bucket);
			JsonWriter->WriteValue(TEXT("PatchVersionLogName"), InFileInfo.PatchVersionLogName);
			JsonWriter->WriteValue(TEXT("ServerVersionName"), InFileInfo.ServerVersionName);
			JsonWriter->WriteValue(TEXT("VersionLock"), InFileInfo.VersionLock);
			JsonWriter->WriteValue(TEXT("Platform"), InFileInfo.Platform);
			JsonWriter->WriteValue(TEXT("PakLocalPaths"), InFileInfo.PakLocalPaths);
			JsonWriter->WriteValue(TEXT("FilePath"), InFileInfo.FilePath);
			JsonWriter->WriteValue(TEXT("Endpoint"), InFileInfo.Endpoint);
			JsonWriter->WriteValue(TEXT("InstallationPath"), InFileInfo.InstallationPath);
		}
		JsonWriter->WriteObjectEnd();
		JsonWriter->Close();
	}

	bool JsonToHotHttpDebugFileInfo(const FString& InJsonString, FHotHttpDebugFileInfo& OutFileInfo)
	{
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(InJsonString);
		TSharedPtr<FJsonValue> ReadRoot;

		if (FJsonSerializer::Deserialize(JsonReader, ReadRoot))
		{
			if (const TSharedPtr<FJsonObject>& InJsonObject = ReadRoot->AsObject())
			{
				OutFileInfo.FileName = InJsonObject->GetStringField(TEXT("FileName"));
				OutFileInfo.VersionName = InJsonObject->GetStringField(TEXT("VersionName"));
				OutFileInfo.Bucket = InJsonObject->GetStringField(TEXT("Bucket"));
				OutFileInfo.PatchVersionLogName = InJsonObject->GetStringField(TEXT("PatchVersionLogName"));
				OutFileInfo.ServerVersionName = InJsonObject->GetStringField(TEXT("ServerVersionName"));
				OutFileInfo.VersionLock = InJsonObject->GetStringField(TEXT("VersionLock"));
				OutFileInfo.Platform = InJsonObject->GetStringField(TEXT("Platform"));
				OutFileInfo.PakLocalPaths = InJsonObject->GetStringField(TEXT("PakLocalPaths"));
				OutFileInfo.FilePath = InJsonObject->GetStringField(TEXT("FilePath"));
				OutFileInfo.Endpoint = InJsonObject->GetStringField(TEXT("Endpoint"));
				OutFileInfo.InstallationPath = InJsonObject->GetStringField(TEXT("InstallationPath"));

				return true;
			}
		}

		return false;
	}
}