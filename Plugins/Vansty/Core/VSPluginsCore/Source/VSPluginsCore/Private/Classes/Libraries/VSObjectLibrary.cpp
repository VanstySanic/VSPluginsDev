// Copyright VanstySanic. All Rights Reserved.

#include "Classes/Libraries/VSObjectLibrary.h"

UVSObjectLibrary::UVSObjectLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UVSObjectLibrary::IsObjectTemplate(UObject* Object)
{
	return Object && Object->IsTemplate();
}
