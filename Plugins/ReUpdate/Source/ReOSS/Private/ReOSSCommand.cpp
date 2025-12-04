#include "ReOSSCommand.h"
#include "ReOSSManage.h"
#include "ReOSSLog.h"

#if PLATFORM_WINDOWS
#pragma optimize("",off) 
#endif

namespace ReOSSCommand
{
	bool GetValue(const TMap<FString,FString> &InMapParam,const TCHAR *InKey,FString &OutValue)
	{
		FString Key = FString::Printf(TEXT("-%s="), InKey);
		if (const FString* InValue = InMapParam.Find(Key))
		{
			OutValue = *InValue;

			return true;
		}

		return false;
	}

	bool Exec(const ReOSSCommand::FOSSCommand &InCommand)
	{
		switch (InCommand.CommandType)
		{
			case EReOSSCommand::OSS_INIT:
			{
				FString AccessKeyId;
				if (!GetValue(InCommand.Param, TEXT("AccessKeyId"),AccessKeyId))
				{
					return false;
				}

				FString AccessKeySecret;
				if (!GetValue(InCommand.Param, TEXT("AccessKeySecret"), AccessKeySecret))
				{
					return false;
				}

				FString Endpoint;
				if (!GetValue(InCommand.Param, TEXT("Endpoint"), Endpoint))
				{
					return false;
				}

				Re_OSS.InitAccounts(AccessKeyId, AccessKeySecret, Endpoint);

				return true;
			}
			case EReOSSCommand::OSS_BUCKET_EXIST:
			{
				FString BucketName;
				if (!GetValue(InCommand.Param, TEXT("BucketName"), BucketName))
				{
					return false;
				}

				return Re_OSS.DoesBucketExist(BucketName);
			}
			case EReOSSCommand::OSS_OBJECT_EXIST:
			{
				FString BucketName;
				if (!GetValue(InCommand.Param, TEXT("BucketName"), BucketName))
				{
					return false;
				}

				FString ObjectName;
				if (!GetValue(InCommand.Param, TEXT("ObjectName"), ObjectName))
				{
					return false;
				}

				return Re_OSS.DoesObjectExist(BucketName, ObjectName);
			}
			case EReOSSCommand::OSS_COPY_OBJECT:
			{
				FString SourceBucketName;
				if (!GetValue(InCommand.Param, TEXT("SourceBucketName"), SourceBucketName))
				{
					return false;
				}

				FString SourceObjectName;
				if (!GetValue(InCommand.Param, TEXT("SourceObjectName"), SourceObjectName))
				{
					return false;
				}

				FString CopyBucketName;
				if (!GetValue(InCommand.Param, TEXT("CopyBucketName"), CopyBucketName))
				{
					return false;
				}

				FString CopyObjectNam;
				if (!GetValue(InCommand.Param, TEXT("CopyObjectNam"), CopyObjectNam))
				{
					return false;
				}
					
				return Re_OSS.CopyObject(SourceBucketName, SourceObjectName, CopyBucketName, CopyObjectNam);
			}
			case EReOSSCommand::OSS_DELETE_OBJECT:
			{
				FString BucketName;
				if (!GetValue(InCommand.Param, TEXT("BucketName"), BucketName))
				{
					return false;
				}

				FString ObjectName;
				if (!GetValue(InCommand.Param, TEXT("ObjectName"), ObjectName))
				{
					return false;
				}

				return Re_OSS.DeleteObject(BucketName, ObjectName);
			}
			case EReOSSCommand::OSS_GET_OBJECT:
			{
				FString BucketName;
				if (!GetValue(InCommand.Param, TEXT("BucketName"), BucketName))
				{
					return false;
				}

				FString ObjectName;
				if (!GetValue(InCommand.Param, TEXT("ObjectName"), ObjectName))
				{
					return false;
				}

				FString LocalPaths;
				if (!GetValue(InCommand.Param, TEXT("LocalPaths"), LocalPaths))
				{
					return false;
				}

				FRange Range;
				return Re_OSS.GetObjectToLocal(BucketName, ObjectName, LocalPaths, Range);
			}
			case EReOSSCommand::OSS_RESUMABLE_DOWNLOAD_OBJECT:
			{
				FString BucketName;
				if (!GetValue(InCommand.Param, TEXT("BucketName"), BucketName))
				{
					return false;
				}

				FString ObjectName;
				if (!GetValue(InCommand.Param, TEXT("ObjectName"), ObjectName))
				{
					return false;
				}

				FString LocalPaths;
				if (!GetValue(InCommand.Param, TEXT("LocalPaths"), LocalPaths))
				{
					return false;
				}

				return Re_OSS.ResumableDownloadObject(BucketName, ObjectName, LocalPaths);
			}
			case EReOSSCommand::OSS_PUT_OBJECT:
			{
				FString BucketName;
				if (!GetValue(InCommand.Param, TEXT("BucketName"), BucketName))
				{
					return false;
				}

				FString ObjectName;
				if (!GetValue(InCommand.Param, TEXT("ObjectName"), ObjectName))
				{
					return false;
				}

				FString LocalPaths;
				if (!GetValue(InCommand.Param, TEXT("LocalPaths"), LocalPaths))
				{
					return false;
				}

				TMap<FString, FString> OSSMeta;
				return Re_OSS.PutObject(BucketName, LocalPaths, ObjectName, nullptr,OSSMeta);
			}
			case EReOSSCommand::OSS_RESUMABLE_UPLOAD_OBJECT:
			{
				FString BucketName;
				if (!GetValue(InCommand.Param, TEXT("BucketName"), BucketName))
				{
					return false;
				}

				FString ObjectName;
				if (!GetValue(InCommand.Param, TEXT("ObjectName"), ObjectName))
				{
					return false;
				}

				FString LocalPaths;
				if (!GetValue(InCommand.Param, TEXT("LocalPaths"), LocalPaths))
				{
					return false;
				}

				TMap<FString, FString> OSSMeta;
				return Re_OSS.ResumableUploadObject(BucketName, ObjectName, LocalPaths, 10485760,nullptr, OSSMeta);
			}
			case EReOSSCommand::OSS_UPLOAD_PART:
			{
				FString UploadId;
				if (!GetValue(InCommand.Param, TEXT("UploadId"), UploadId))
				{
					return false;
				}

				FString BucketName;
				if (!GetValue(InCommand.Param, TEXT("BucketName"), BucketName))
				{
					return false;
				}

				FString ObjectName;
				if (!GetValue(InCommand.Param, TEXT("ObjectName"), ObjectName))
				{
					return false;
				}

				FString LocalPaths;
				if (!GetValue(InCommand.Param, TEXT("LocalPaths"), LocalPaths))
				{
					return false;
				}
				int32 PartSize = 1024 * 1024 * 10;
				TMap<FString, FString> OSSMeta;
				return Re_OSS.UploadPart(UploadId,BucketName, ObjectName, LocalPaths, PartSize, OSSMeta);
			}
		}

		return false;
	}

	bool Exec(const TArray<FOSSCommand>& InCommands)
	{
		TMultiMap<EReOSSCommand, bool> OutResult;
		Exec(InCommands, OutResult);

		for (auto &Tmp : OutResult)
		{
			if (!Tmp.Value)
			{
				return false;
			}
		}
		
		return OutResult.Num() > 0;
	}

	void Exec(const TArray<FOSSCommand>& InCommands, TMultiMap<EReOSSCommand, bool>& OutResult)
	{
		for (auto& Tmp : InCommands)
		{
			OutResult.Add(Tmp.CommandType,Exec(Tmp));
		}
	}

	void PrintOSSCommand(const TArray<FOSSCommand>& InCommands)
	{
		for (auto& Tmp : InCommands)
		{
			switch (Tmp.CommandType)
			{
				case EReOSSCommand::OSS_UPLOAD_PART:
				{
					break;
				}
			}
		}
	}

	FString ToOSSCommandString(EReOSSCommand InOSSCommand)
	{
		FString NumFullName = UEnum::GetValueAsName(InOSSCommand).ToString();

		int32 ProtocolHeardLen = FCString::Strlen(TEXT("EReOSSCommand::OSS_"));
		FString HandleName = NumFullName.RightChop(ProtocolHeardLen);

		return HandleName.ToLower();
	}

	EReOSSCommand GetOSSCommandByName(const FString& InJsonName)
	{
		FString NumFullName = *FString::Printf(TEXT("EReOSSCommand::OSS_%s"), *InJsonName).ToUpper();

		return (EReOSSCommand)UEnum::LookupEnumName(FName(), *NumFullName);
	}

	bool JsonStringToOSSCommand(const FString& InString, TArray<ReOSSCommand::FOSSCommand>& OutConfig)
	{
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(InString);
		TArray<TSharedPtr<FJsonValue>> ReadRoot;

		if (FJsonSerializer::Deserialize(JsonReader, ReadRoot))
		{
			for (auto &Tmp : ReadRoot)
			{
				if (const TSharedPtr<FJsonObject> JsonObject = Tmp->AsObject())
				{
					JsonObjectToOSSCommand(JsonObject,OutConfig.AddDefaulted_GetRef());
				}
			}

			return true;
		}

		return false;
	}

	bool JsonObjectToOSSCommand(TSharedPtr<FJsonObject> InJsonObject, ReOSSCommand::FOSSCommand& OutConfig)
	{
		if (InJsonObject)
		{
			OutConfig.CommandType = GetOSSCommandByName(InJsonObject->GetStringField(TEXT("CommandType")));

			const TArray<TSharedPtr<FJsonValue>>& ObjectArray = InJsonObject->GetArrayField(TEXT("Param"));
			for (auto& Tmp : ObjectArray)
			{
				if (TSharedPtr<FJsonObject> TmpObject = Tmp->AsObject())
				{
					FString Key = TmpObject->GetStringField(TEXT("Key"));
					FString Value = TmpObject->GetStringField(TEXT("Value"));

					FPaths::NormalizeFilename(Key);
					FPaths::NormalizeFilename(Value);

					OutConfig.Param.Add(Key, Value);
				}
			}

			return true;
		}

		return false;
	}

	TSharedPtr<FJsonObject> OSSCommandConfigToJsonObject(const TArray<ReOSSCommand::FOSSCommand>& InConfig)
	{
		TSharedPtr<FJsonObject> RootObject = MakeShareable<FJsonObject>(new FJsonObject);
		{
			TArray<TSharedPtr<FJsonValue>> ObjectArray;
			ReOSSCommand::OSSCommandConfigToJsonObject(InConfig,ObjectArray);

			RootObject->SetArrayField(TEXT("OSSCommands"), ObjectArray);
		}

		return RootObject;
	}

	bool OSSCommandConfigToJsonObject(const TArray<ReOSSCommand::FOSSCommand>& InConfig, TArray<TSharedPtr<FJsonValue>>& OutArray)
	{
		for (auto& InOSSCommand : InConfig)
		{
			TSharedPtr<FJsonObject> InOSSCommandObject = MakeShareable<FJsonObject>(new FJsonObject);
			InOSSCommandObject->SetStringField(TEXT("CommandType"), ReOSSCommand::ToOSSCommandString(InOSSCommand.CommandType));

			TArray<TSharedPtr<FJsonValue>> ParamObjectArray;
			for (auto& Tmp : InOSSCommand.Param)
			{
				TSharedPtr<FJsonObject> ParamObject = MakeShareable<FJsonObject>(new FJsonObject);
				ParamObject->SetStringField(TEXT("Key"), Tmp.Key);
				ParamObject->SetStringField(TEXT("Value"), Tmp.Value);

				ParamObjectArray.Add(MakeShareable<FJsonValueObject>(new FJsonValueObject(ParamObject)));
			}

			InOSSCommandObject->SetArrayField(TEXT("Param"), ParamObjectArray);

			OutArray.Add(MakeShareable<FJsonValueObject>(new FJsonValueObject(InOSSCommandObject)));
		}

		return OutArray.Num() > 0;
	}

	bool BuildConfig(TArray<ReOSSCommand::FOSSCommand>& OutConfig)
	{
		FString OSSCommands;
		if (!FParse::Value(FCommandLine::Get(), TEXT("-OSSCommands="),OSSCommands))
		{
			UE_LOG(LogReOSS, Error, TEXT("-OSSCommands was not found value"));

			return false;
		}

		TArray<FString> Elements;
		OSSCommands.ParseIntoArray(Elements,TEXT("&&&"));

		for (auto &InCommandTmp: Elements)
		{
			ReOSSCommand::FOSSCommand &InCommand = OutConfig.AddDefaulted_GetRef();

			TArray<FString> CommandElements;
			InCommandTmp.ParseIntoArray(CommandElements,TEXT("|||"));

			if (CommandElements.Num() >= 2)
			{
				InCommand.CommandType = ReOSSCommand::GetOSSCommandByName(CommandElements[0]);

				TArray<FString> ParamArrays;
				CommandElements[1].ParseIntoArray(ParamArrays,TEXT("&&"));

				for (auto &ParamTmp : ParamArrays)
				{
					FString R, L;
					ParamTmp.Split(TEXT("||"),&L,&R);

					InCommand.Param.Add(L, R);
				}
			}
		}

		return OutConfig.Num() > 0;
	}
}

#if PLATFORM_WINDOWS
#pragma optimize("",on) 
#endif
