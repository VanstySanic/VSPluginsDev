// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "UObject/Object.h"
#include "VSGameplayTypes.generated.h"

class URichTextBlock;
class URichTextBlockDecorator;

// /** Useful in multi-layer TMap with key type of ordered gameplay tags. */
// USTRUCT(BlueprintType)
// struct VSPLUGINSCORE_API FVSGameplayTagKey
// {
// 	GENERATED_BODY()
//
// 	FVSGameplayTagKey(const TArray<FGameplayTag>& GameplayTagKeys = TArray<FGameplayTag>())
// 		: Keys(GameplayTagKeys)
// 	{
// 	}
//
// 	bool operator==(const FVSGameplayTagKey& Other) const
// 	{
// 		return Keys == Other.Keys;
// 	}
//
// 	friend uint32 GetTypeHash(const FVSGameplayTagKey& InKey)
// 	{
// 		return GetTypeHash(InKey.Keys);
// 	}
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ShowOnlyInnerProperties))
// 	TArray<FGameplayTag> Keys;
// };
//
// USTRUCT(BlueprintType)
// struct FVSDataTableRowHandleWrapper
// {
// 	GENERATED_BODY()
//
// 	FVSDataTableRowHandleWrapper(const FDataTableRowHandle& Row = FDataTableRowHandle())
// 		: Row(Row)
// 	{
// 	}
//
// 	friend uint8 GetTypeHash(const FVSDataTableRowHandleWrapper& Wrap)
// 	{
// 		return HashCombine(GetTypeHash(Wrap.Row.DataTable), GetTypeHash(Wrap.Row.RowName.ToString()));
// 	}
// 	
// 	template <typename T>
// 	T* GetRow(const TCHAR* ContextString = nullptr) const
// 	{
// 		return Row.GetRow<T>(ContextString);
// 	}
//
// 	VSPLUGINSCORE_API bool IsNull() const
// 	{
// 		return Row.IsNull();
// 	}
//
// 	VSPLUGINSCORE_API bool operator==(const FVSDataTableRowHandleWrapper& Other) const
// 	{
// 		return Row == Other.Row;
// 	}
//
// 	VSPLUGINSCORE_API bool operator!=(const FVSDataTableRowHandleWrapper& Other) const
// 	{
// 		return Row != Other.Row;
// 	}
// 	
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ShowOnlyInnerProperties))
// 	FDataTableRowHandle Row;
// };

/**
 * Helper key that resolves either user index or platform user id.
 */
struct FVSUserQueryParams
{
	FVSUserQueryParams() = delete;

	FVSUserQueryParams(uint32 InUserIndex) : Key(KeyType::UserIndex) { Data.UserIndex = InUserIndex; }
	FVSUserQueryParams(const FPlatformUserId& InUserId) : Key(KeyType::UserId) { Data.UserId = InUserId; }
	FVSUserQueryParams(ULocalPlayer* InLocalPlayer) : Key(KeyType::LocalPlayer) { Data.LocalPlayer = InLocalPlayer; }
	FVSUserQueryParams(APlayerController* InPlayerController) : Key(KeyType::PlayerController) { Data.PlayerController = InPlayerController; }

	VSPLUGINSCORE_API int32 GetUserIndex() const;
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


/**
 * Network execution policy applied when local role is Authority.
 */
UENUM(BlueprintType)
namespace EVSNetAuthorityMethodExecPolicy
{
	enum Type : int32
	{
		/** Nothing will happen. */
		None						= 0,
		/** Execute locally without sending RPC to clients. */
		Server						= 1 << 1,
		/** Send RPC to the owning client only without executing locally. */
		Client						= 1 << 2,
		/** Execute locally and then send RPC to the owning client only. */
		Simulated					= 1 << 3,
		
		ServerAndClient				= Server + Client,
		ServerAndSimulated			= Server + Simulated,
		ClientAndSimulated			= Client + Simulated,
		Multicast					= Server + Client + Simulated,
	};
}

/**
 * Network execution policy applied when local role is Autonomous.
 */
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
 * Per-role network execution policies used by caller-side dispatch code.
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
	
	/** Network behavior when the local role is Autonomous. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSNetAutonomousMethodExecPolicy::Type> AutonomousLocalPolicy;
	
	/** Network behavior when the local role is Authority. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSNetAuthorityMethodExecPolicy::Type> AuthorityLocalPolicy;
	
	/** Server-side network behavior when entered from autonomous RPC calls. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EVSNetAuthorityMethodExecPolicy::Type> ServerRPCPolicy;
	
	/** Whether simulated proxies also execute the method locally. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSimulatedLocalExecution;

	VSPLUGINSCORE_API static FVSNetMethodExecutionPolicies LocalExecution;
	VSPLUGINSCORE_API static FVSNetMethodExecutionPolicies AutonomousPredictedMulticast;
	VSPLUGINSCORE_API static FVSNetMethodExecutionPolicies AuthorityMulticast;
};

/**
 * Parameters for resolving a world-space location under cursor input.
 */
USTRUCT(BlueprintType)
struct FVSLocationUnderCursorQueryParams
{
	GENERATED_BODY()

	VSPLUGINSCORE_API bool IsValid() const;

	/** Player controller used to fetch cursor position and perform tracing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<APlayerController> PlayerController;

	/** If true, return location from collision trace under cursor. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bTraceByCollision = true;

	/** Collision channel used when bTraceByCollision is true. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bTraceByCollision"))
	TEnumAsByte<ETraceTypeQuery> TraceType = TraceTypeQuery1;

	/** If true, also allow resolving location by intersecting a plane. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIntersectByPlane = false;

	/** Plane normal used for cursor ray/plane intersection. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bIntersectByPlane"))
	FVector PlaneNormal = FVector::ZeroVector;

	/** A point on the plane used for cursor ray/plane intersection. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bIntersectByPlane"))
	FVector PlanePoint = FVector::ZeroVector;

	/** If true, use CursorPositionOverride instead of current hardware cursor position. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bOverrideCursorPosition = false;

	/** Screen-space cursor position override. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bOverrideCursorPosition"))
	FVector2D CursorPositionOverride = FVector2D::ZeroVector;
};
