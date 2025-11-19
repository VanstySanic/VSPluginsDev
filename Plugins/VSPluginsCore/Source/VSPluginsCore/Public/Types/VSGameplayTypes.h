// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "VSGameQueryTypes.h"
#include "UObject/Object.h"
#include "VSGameplayTypes.generated.h"

class URichTextBlock;
class URichTextBlockDecorator;

namespace EVSGameplayTagControllerTags
{
	VSPLUGINSCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_TagsUpdated);
}

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
		/** Execute locally without sending RPC to server. */
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
 * @remarks This is set to local execution by default.
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

	VSPLUGINSCORE_API bool IsNull() const
	{
		return Row.IsNull();
	}

	VSPLUGINSCORE_API bool operator==(const FVSDataTableRowHandleWrap& Other) const
	{
		return Row == Other.Row;
	}

	VSPLUGINSCORE_API bool operator!=(const FVSDataTableRowHandleWrap& Other) const
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
struct FVSLocationUnderCursorQueryParams
{
	GENERATED_BODY()

	VSPLUGINSCORE_API bool IsValid() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<APlayerController> PlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bTraceByCollision = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ETraceTypeQuery> TraceType = TraceTypeQuery1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIntersectByPlane = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bIntersectByPlane"))
	FVector PlaneNormal = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bIntersectByPlane"))
	FVector PlanePoint = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bOverrideCursorPosition = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bOverrideCursorPosition"))
	FVector2D CursorPositionOverride = FVector2D::ZeroVector;
};
