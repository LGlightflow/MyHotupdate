// Copyright (C) RenZhai.2020.All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

enum class EHTTPObjectStorageProtocol :uint8
{
	HSP_INIT,//初始化服务器的数据
	HSP_CHECK_LOCK,//检测锁
	HSP_LOCK,//上锁
	HSP_UNLOCK,//解锁
};