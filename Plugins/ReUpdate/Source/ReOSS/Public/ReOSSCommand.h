#pragma once

//该类为自动化考虑

#include "CoreMinimal.h"
#include "Json.h"
#include "ReOSSCommand.generated.h"

//命令
UENUM(BlueprintType)
enum class EReOSSCommand :uint8
{
	OSS_INIT,
	OSS_BUCKET_EXIST,
	OSS_OBJECT_EXIST,
	OSS_COPY_OBJECT,
	OSS_DELETE_OBJECT,
	OSS_GET_OBJECT,
	OSS_RESUMABLE_DOWNLOAD_OBJECT,
	OSS_PUT_OBJECT,
	OSS_RESUMABLE_UPLOAD_OBJECT,
	OSS_UPLOAD_PART,
};

namespace ReOSSCommand
{
	struct REOSS_API FOSSCommand
	{
		//指定命令类型
		EReOSSCommand CommandType;

		//对应参数
		TMap<FString, FString> Param;
	};

	//通过命令执行OSS操作
	bool REOSS_API Exec(const ReOSSCommand::FOSSCommand &InCommand);
	bool REOSS_API Exec(const TArray<FOSSCommand> &InCommands);
	void REOSS_API Exec(const TArray<FOSSCommand>& InCommands,TMultiMap<EReOSSCommand,bool> &OutResult);
	void REOSS_API PrintOSSCommand(const TArray<FOSSCommand>& InCommands);

	//OSS协议
	FString REOSS_API ToOSSCommandString(EReOSSCommand InOSSCommand);
	EReOSSCommand REOSS_API GetOSSCommandByName(const FString& InJsonName);

	//Json解析
	bool REOSS_API JsonStringToOSSCommand(const FString& InString, TArray<ReOSSCommand::FOSSCommand>& OutConfig);
	bool REOSS_API JsonObjectToOSSCommand(TSharedPtr<FJsonObject> InJsonObject, ReOSSCommand::FOSSCommand& OutConfig);
	bool REOSS_API JsonObjectToOSSCommand(TSharedPtr<FJsonObject> InJsonObject, ReOSSCommand::FOSSCommand& OutConfig);

	//用于反序列化
	TSharedPtr<FJsonObject> REOSS_API OSSCommandConfigToJsonObject(const TArray<ReOSSCommand::FOSSCommand>& InConfig);
	bool REOSS_API OSSCommandConfigToJsonObject(const TArray<ReOSSCommand::FOSSCommand>& InConfig,TArray<TSharedPtr<FJsonValue>> &OutArray);

	//构建配置
	bool REOSS_API BuildConfig(TArray<ReOSSCommand::FOSSCommand>& OutConfig);
}