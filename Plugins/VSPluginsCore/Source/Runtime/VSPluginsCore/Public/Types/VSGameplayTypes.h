// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "UObject/Object.h"
#include "VSGameplayTypes.generated.h"

namespace EVSGameplayTagControllerTags
{
	VSPLUGINSCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_TagsUpdated);
}

USTRUCT(BlueprintType)
struct FVSGameplayTagEventQuery
{
	GENERATED_BODY()

	FVSGameplayTagEventQuery()
		: bMatchExactTagEvent(true)
		, bEmptyEventAsPass(true)
		, bTagEventsEmptyAsPass(false)
		, bTagQueryEmptyAsPass(true)
	{
	}

	VSPLUGINSCORE_API bool Matches(const FGameplayTagContainer& GameplayTags, const FGameplayTag& TagEvent = FGameplayTag::EmptyTag) const;
	
	/** Work as an entry. Requires TagEvent in the array. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer TagEvents;

	/** Gameplay tag query to check. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagQuery TagQuery;

	/** If true, only exact tag event will match. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bMatchExactTagEvent : 1;
	
	/** If true, empty tag will be considered as a pass to the tag events. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bEmptyEventAsPass : 1;
	
	/** If true and TagEvents array is empty, any non-empty tag event will be considered as trigger. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bTagEventsEmptyAsPass : 1;

	/** If true and TagQuery is empty, any tag states will match the query. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bTagQueryEmptyAsPass : 1;
};

USTRUCT(BlueprintType)
struct FVSGameplayTagEventQueryContainer
{
	GENERATED_BODY()
	
	FVSGameplayTagEventQueryContainer()
		: bQueriesEmptyAsPass(true)
	{
	}

	/** Pass if matches one. */
	VSPLUGINSCORE_API bool Matches(const FGameplayTagContainer& GameplayTags, const FGameplayTag& TagEvent = FGameplayTag::EmptyTag) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVSGameplayTagEventQuery> Queries;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 bQueriesEmptyAsPass : 1;
};

/**
 * Useful in multi-layer TMap with key type of gameplay tags.
 */
USTRUCT(BlueprintType)
struct VSPLUGINSCORE_API FVSGameplayTagKey
{
	GENERATED_BODY()

	FVSGameplayTagKey(const TArray<FGameplayTag>& GameplayTagKeys = TArray<FGameplayTag>())
		: Keys(GameplayTagKeys)
	{
	}

	bool operator==(const FVSGameplayTagKey& Other) const
	{
		return Keys == Other.Keys;
	}

	friend uint32 GetTypeHash(const FVSGameplayTagKey& InKey)
	{
		return GetTypeHash(InKey.Keys);
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ShowOnlyInnerProperties))
	TArray<FGameplayTag> Keys;
};

/** Used to get user index or user id. */
struct FVSUserQueryParams
{
	FVSUserQueryParams() = delete;

	FVSUserQueryParams(uint32 InUserIndex) : Key(KeyType::UserIndex) { Data.UserIndex = InUserIndex; }
	FVSUserQueryParams(const FPlatformUserId& InUserId) : Key(KeyType::UserId) { Data.UserId = InUserId; }
	FVSUserQueryParams(ULocalPlayer* InLocalPlayer) : Key(KeyType::LocalPlayer) { Data.LocalPlayer = InLocalPlayer; }
	FVSUserQueryParams(APlayerController* InPlayerController) : Key(KeyType::PlayerController) { Data.PlayerController = InPlayerController; }

	VSPLUGINSCORE_API uint32 GetUserIndex() const;
	VSPLUGINSCORE_API FPlatformUserId GetUserId() const;

private:
	enum class KeyType
	{
		None,
		UserIndex,
		UserId,
		LocalPlayer,
		PlayerController
	};
	
	union DataUnion
	{
		uint32 UserIndex;
		FPlatformUserId UserId;
		ULocalPlayer* LocalPlayer;
		APlayerController* PlayerController;

		DataUnion() : UserIndex(INDEX_NONE) {}
	};

	KeyType Key = KeyType::None;
	DataUnion Data;
};


/** If the local role is Authority, what will the locally-called method do with network. */
UENUM(BlueprintType)
namespace EVSNetAuthorityMethodExecPolicy
{
	enum Type : int32
	{
		/** Nothing will happen. */
		None						= 0,
		/** Execute locally without sending RPC to clients. */
		Server						= 1 << 1,
		/** Send RPC to the owing client only without executing locally. */
		Client						= 1 << 2,
		/** Execute locally and then send RPC to the owing client only. */
		Simulated					= 1 << 3,
		ServerAndClient				= Server + Client,
		ServerAndSimulated			= Server + Simulated,
		ClientAndSimulated			= Client + Simulated,
		Multicast					= Server + Client + Simulated,
	};
}

/** If the local role is Autonomous, what will the locally-called method do with network. */
UENUM(BlueprintType)
namespace EVSNetAutonomousMethodExecPolicy
{
	enum Type : uint8
	{
		/** Nothing will happen. */
		None						= 0,
		/** Execute locally without sending RPC to serever. */
		Client						= 1 << 0,
		/** Send RPC to server without executing locally. */
		Server						= 1 << 1,
		/** Execute locally and then send RPC to server. */
		ClientAndServer				= Client + Server,
	};
}

/**
 * Defines how a locally-called method works with network under different local role.
 * Notice that this only gives policies but doesn't handle them. Users need to manually process conditions.
 * Sometimes your logic might only handle part of the policies.
 */
USTRUCT(BlueprintType)
struct FVSNetMethodExecutionPolicies
{
	GENERATED_BODY()

	FVSNetMethodExecutionPolicies(
		EVSNetAutonomousMethodExecPolicy::Type AutonomousLocalPolicy = EVSNetAutonomousMethodExecPolicy::Client,
		EVSNetAuthorityMethodExecPolicy::Type AuthorityLocalPolicy = EVSNetAuthorityMethodExecPolicy::Server,
		EVSNetAuthorityMethodExecPolicy::Type ServerRPCPolicy = EVSNetAuthorityMethodExecPolicy::None,
		bool bSimulatedLocalExecution = true)
		: AutonomousLocalPolicy(AutonomousLocalPolicy)
		, AuthorityLocalPolicy(AuthorityLocalPolicy)
		, ServerRPCPolicy(ServerRPCPolicy)
		, bSimulatedLocalExecution(bSimulatedLocalExecution)
	{
		
	}
	
	/** If the local role is Autonomous, what will the locally-called method do with network. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSNetAutonomousMethodExecPolicy::Type> AutonomousLocalPolicy;
	
	/** If the local role is Authority, what will the locally-called method do with network. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSNetAuthorityMethodExecPolicy::Type> AuthorityLocalPolicy;
	
	/** If the call is from an autonomous RPC, what will the server method do with network.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSNetAuthorityMethodExecPolicy::Type> ServerRPCPolicy;
	
	/** If the local role is Simulated, whether to execute locally. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSimulatedLocalExecution;

	VSPLUGINSCORE_API static FVSNetMethodExecutionPolicies LocalExecution;
	VSPLUGINSCORE_API static FVSNetMethodExecutionPolicies AutonomousPredictedMulticast;
	VSPLUGINSCORE_API static FVSNetMethodExecutionPolicies AuthorityMulticast;
};

USTRUCT(BlueprintType)
struct FVSDataTableRowHandleWrap
{
	GENERATED_BODY()

	FVSDataTableRowHandleWrap(const FDataTableRowHandle& Row = FDataTableRowHandle())
		: Row(Row)
	{
	}

	friend uint8 GetTypeHash(const FVSDataTableRowHandleWrap& Wrap)
	{
		return HashCombine(GetTypeHash(Wrap.Row.DataTable), GetTypeHash(Wrap.Row.RowName.ToString()));
	}
	
	template <typename T>
	T* GetRow(const TCHAR* ContextString = nullptr) const;

	bool IsNull() const
	{
		return Row.IsNull();
	}

	bool operator!=(const FVSDataTableRowHandleWrap& Other) const
	{
		return Row != Other.Row;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ShowOnlyInnerProperties))
	FDataTableRowHandle Row;
};

template <typename T>
T* FVSDataTableRowHandleWrap::GetRow(const TCHAR* ContextString) const
{
	return Row.GetRow<T>(ContextString);
}

USTRUCT(BlueprintType)
struct FVSSceneComponentQuery
{
	GENERATED_BODY()

	VSPLUGINSCORE_API bool Matches(const USceneComponent* Component) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<USceneComponent>> ComponentClasses;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> ComponentTags;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> ActorTags;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInverseClassAllowance = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInverseComponentTagAllowance = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInverseActorTagAllowance = false;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bComponentClassesEmptyAsPass = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bObjectTypesEmptyAsPass = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bComponentTagsEmptyAsPass = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bActorTagsEmptyAsPass = true;
};
