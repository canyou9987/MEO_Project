// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "MEO_Project/Subsystems/CraftingSubsystem.h"
#include "InventoryComponent.generated.h"

class UInventoryComponent;

// 패스트 어레이 개별 아이템 엔트리
USTRUCT(BlueprintType)
struct FInventoryItemEntry : public FFastArraySerializerItem
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    FMEOItemInstance ItemInstance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    FIntPoint GridPosition = FIntPoint(-1, -1); // 2D 격자 위치 (X, Y)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    bool bIsEquipped = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    FName EquippedSlotTag = NAME_None;

    // FFastArraySerializer 필수 콜백
    void PreReplication(const struct FInventoryFastArray& InArraySerializer);
    void PostReplication(const struct FInventoryFastArray& InArraySerializer);
};

// Delta 동기화 패스트 어레이 컨테이너
USTRUCT(BlueprintType)
struct FInventoryFastArray : public FFastArraySerializer
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<FInventoryItemEntry> Items;

    UPROPERTY()
    TObjectPtr<UInventoryComponent> OwnerComponent = nullptr;

    bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
    {
        return FFastArraySerializer::FastArrayDeltaSerialize<FInventoryItemEntry, FInventoryFastArray>(Items, DeltaParms, *this);
    }
};

template<>
struct TStructOpsTypeTraits<FInventoryFastArray> : public TStructOpsTypeTraitsBase2<FInventoryFastArray>
{
    enum
    {
        WithNetDeltaSerializer = true,
    };
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MEO_PROJECT_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
    UInventoryComponent();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // --------------------------------------------------
    // [2D 격자 인벤토리 조작 API]
    // --------------------------------------------------
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool TryAddItem(const FMEOItemInstance& NewItem, FIntPoint ItemSize);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool MoveItem(const FGuid& ItemGUID, FIntPoint TargetPosition);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool EquipItem(const FGuid& ItemGUID, FName SlotTag);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool UnequipItem(const FGuid& ItemGUID, FIntPoint TargetGridPosition);

    UFUNCTION(BlueprintPure, Category = "Inventory")
    bool CanPlaceItemAt(FIntPoint TargetPosition, FIntPoint ItemSize, const FGuid& IgnoredItemGUID = FGuid()) const;

    UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
    FOnInventoryUpdated OnInventoryUpdated;

    void BroadcastInventoryUpdated();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Config")
    FIntPoint GridSize = FIntPoint(12, 5); // 12x5 인벤토리 격자

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Inventory State")
    FInventoryFastArray InventoryArray;

		
};
