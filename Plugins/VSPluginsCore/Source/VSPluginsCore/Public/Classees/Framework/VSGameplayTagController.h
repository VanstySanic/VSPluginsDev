// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "VSObjectFeature.h"
#include "Interfaces/VSGameplayTagControllerInterface.h"
#include "VSGameplayTagController.generated.h"

class UAbilitySystemComponent;

/**
 * Gives control for gameplay tags in ability system component.
 */
UCLASS(DisplayName = "Feature.GameplayTagController")
class VSPLUGINSCORE_API UVSGameplayTagController : public UVSObjectFeature, public IGameplayTagAssetInterface
{
	GENERATED_UCLASS_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameplayTagsUpdatedDelegate);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGameplayTagCountSignChangedDelegate, const FGameplayTag&, Tag, bool, bExists);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGameplayTagEventDelegate, const FGameplayTag&, TagEvent);
	
protected:
	virtual void Initialize_Implementation() override;
	virtual void Uninitialize_Implementation() override;
	virtual void Tick_Implementation(float DeltaTime) override;
	
public:
	//~ Begin IGameplayTagAssetInterface.
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	//~ End IGameplayTagAssetInterface.

	UFUNCTION(BlueprintCallable, Category = "GameplayTags")
	FGameplayTagContainer GetGameplayTags() const;
	
	/** Returns the number of instances of a given tag */
	UFUNCTION(BlueprintCallable, Category = "GameplayTags", meta = (AutoCreateRefTerm = "TagToCheck"))
	int32 GetTagCount(FGameplayTag TagToCheck) const;

	/** Checks whether the query matches the owned GameplayTags */
	UFUNCTION(BlueprintCallable, Category = "GameplayTags", meta = (AutoCreateRefTerm = "TagQuery"))
	bool MatchesGameplayTagQuery(const FGameplayTagQuery& TagQuery) const;
	
	UFUNCTION(BlueprintCallable, Category = "GameplayTags", meta = (AutoCreateRefTerm = "GameplayTag"))
	void AddTag(const FGameplayTag& GameplayTag, int32 Count = 1);

	UFUNCTION(BlueprintCallable, Category = "GameplayTags", meta = (AutoCreateRefTerm = "GameplayTags"))
	void AddTags(const FGameplayTagContainer& GameplayTags, int32 Count = 1);

	UFUNCTION(BlueprintCallable, Category = "GameplayTags", meta = (AutoCreateRefTerm = "GameplayTag"))
	void RemoveTag(const FGameplayTag& GameplayTag, int32 Count = 1);

	UFUNCTION(BlueprintCallable, Category = "GameplayTags", meta = (AutoCreateRefTerm = "GameplayTags"))
	void RemoveTags(const FGameplayTagContainer& GameplayTags, int32 Count = 1);

	UFUNCTION(BlueprintCallable, Category = "GameplayTags", meta = (AutoCreateRefTerm = "GameplayTag"))
	void SetTagCount(const FGameplayTag& GameplayTag, int32 NewCount);

	UFUNCTION(BlueprintCallable, Category = "GameplayTags", meta = (AutoCreateRefTerm = "GameplayTag"))
	void AddReplicatedTag(const FGameplayTag& GameplayTag);

	UFUNCTION(BlueprintCallable, Category = "GameplayTags", meta = (AutoCreateRefTerm = "GameplayTags"))
	void AddReplicatedTags(const FGameplayTagContainer& GameplayTags);

	UFUNCTION(BlueprintCallable, Category = "GameplayTags", meta = (AutoCreateRefTerm = "GameplayTag"))
	void RemoveReplicatedTag(const FGameplayTag& GameplayTag);

	UFUNCTION(BlueprintCallable, Category = "GameplayTags", meta = (AutoCreateRefTerm = "GameplayTags"))
	void RemoveReplicatedTags(const FGameplayTagContainer& GameplayTags);

	/** Notify that the tags have been updated. */
	UFUNCTION(BlueprintCallable, Category = "GameplayTags")
	void NotifyTagsUpdated(bool bAllowCleanNotify = false, bool bMulticast = false);

	/** Trigger a gameplay tag event notify. */
	UFUNCTION(BlueprintCallable, Category = "GameplayTags", meta = (AutoCreateRefTerm = "TagEvent"))
	void NotifyTagEvent(const FGameplayTag& TagEvent, bool bMulticast = false);
	
	UFUNCTION(BlueprintCallable, Category = "GameplayTags")
	bool IsDirty() const { return bTagsDirty; }

	UFUNCTION(BlueprintCallable, Category = "GameplayTags")
	FString ToDebugString();

private:
	UAbilitySystemComponent* GetAbilitySystemComponent() const;

	UFUNCTION()
	void OnAnyTagChange(const FGameplayTag Tag, int32 Count);
	
	UFUNCTION(Server, Reliable)
	void SetReplicatedTagExists_Server(const FGameplayTag& GameplayTag, bool bExists);

	UFUNCTION(Server, Reliable)
	void SetReplicatedTagsExist_Server(const FGameplayTagContainer& GameplayTags, bool bExists);

	UFUNCTION(Server, Reliable)
	void NotifyTagsUpdated_Server(bool bAllowCleanNotify);
	
	UFUNCTION(Server, Reliable)
	void NotifyTagsUpdated_Multicast(bool bAllowCleanNotify);
	
	UFUNCTION(Server, Reliable)
	void NotifyTagEvent_Server(const FGameplayTag& TagEvent);
	
	UFUNCTION(Server, Reliable)
	void NotifyTagEvent_Multicast(const FGameplayTag& TagEvent);

public:
	/** Broadcast when any tag count reaches 0 or move away from zero. */
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FOnGameplayTagsUpdatedDelegate OnTagsUpdated;
	
	/** Broadcast when a tag count reaches 0 or move away from zero. */
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FOnGameplayTagCountSignChangedDelegate OnTagNewOrClear;

	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FGameplayTagEventDelegate OnTagEventNotified;
	
public:
	/**
	 * If true, OnTagsUpdated delegate will broadcast everytime when a tag count sign changes. Otherwise, this will be processed in another way.
	 * This might be costly so it's recommended not to enable this.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayTags")
	bool bNotifyTagsUpdateInstantly = false;

	/** If true, OnTagsUpdated delegate will broadcast in ticks if dirty. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayTags")
	bool bNotifyTagsUpdateDuringTicks = true;

	/**
	 * If true, OnTagNewOrClear will broadcast for all tags. Otherwise, only tags in TagsToNotifyNewOrClear will broadcast.
	 * This might be costly so it's recommended not to enable this.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayTags")
	bool bNotifyTagNewOrClearForAll;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayTags", meta = (EditCondition = "bNotifyTagNewOrClearForAll"))
	FGameplayTagContainer TagsToNotifyNewOrClear;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bPrintDebugString = false;
#endif
	
private:
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponentPrivate;
	FDelegateHandle OnAnyTagChangeDelegateHandle;
	bool bTagsDirty = false;
};
