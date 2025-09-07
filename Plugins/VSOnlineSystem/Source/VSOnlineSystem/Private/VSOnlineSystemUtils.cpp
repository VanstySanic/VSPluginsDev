// Fill out your copyright notice in the Description page of Project Settings.


#include "VSOnlineSystemUtils.h"

UVSOnlineSystemUtils::UVSOnlineSystemUtils(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FName UVSOnlineSystemUtils::GetCurrentSessionName()
{
	return NAME_GameSession;
}
