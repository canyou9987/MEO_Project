// Fill out your copyright notice in the Description page of Project Settings.

#include "MEO_Project/Components/InventoryComponent.h"
#include "Net/UnrealNetwork.h"

void FInventoryItemEntry::PreReplication(const FInventoryFastArray& InArraySerializer)
{
    // =========================================================================================
    // [FastArray Lifecycle - Server Pre-Replication Callback]
    // - 호출 시점: 서버에서 해당 아이템 엔트리 데이터를 클라이언트로 송신(직렬화)하기 직전
    // - 현재 상태: ItemInstance 및 GridPosition 데이터가 서버 로직 연산 시점에 이미 최신화되어 있어
    //             동기화 직전 가공(Pre-processing) 로직이 불필요하므로 비워둠.
    // - 향후 확장: 네트워크 직렬화 전 아이템 패킹/압축, 런타임 스탯 최종 검증 로직 작성 위치
    // =========================================================================================
}

void FInventoryItemEntry::PostReplication(const FInventoryFastArray& InArraySerializer)
{
	if (InArraySerializer.OwnerComponent)
	{
		InArraySerializer.OwnerComponent->BroadcastInventoryUpdated();
	}
}


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	InventoryArray.OwnerComponent = this;

	// ...
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UInventoryComponent, InventoryArray, COND_OwnerOnly);
}

bool UInventoryComponent::TryAddItem(const FMEOItemInstance& NewItem, FIntPoint ItemSize)
{
    if (GetOwner() && !GetOwner()->HasAuthority()) return false;

    // 2D 격자 내 빈 공간 검색 (Scan)
    for (int32 y = 0; y <= GridSize.Y - ItemSize.Y; ++y)
    {
        for (int32 x = 0; x <= GridSize.X - ItemSize.X; ++x)
        {
            FIntPoint CandidatePos(x, y);
            if (CanPlaceItemAt(CandidatePos, ItemSize))
            {
                FInventoryItemEntry& NewEntry = InventoryArray.Items.AddDefaulted_GetRef();
                NewEntry.ItemInstance = NewItem;
                NewEntry.GridPosition = CandidatePos;
                NewEntry.bIsEquipped = false;

                InventoryArray.MarkItemDirty(NewEntry);
                BroadcastInventoryUpdated();
                return true;
            }
        }
    }

    return false; // 공간 부족
}

bool UInventoryComponent::MoveItem(const FGuid& ItemGUID, FIntPoint TargetPosition)
{
    if (GetOwner() && !GetOwner()->HasAuthority()) return false;

    FInventoryItemEntry* FoundEntry = InventoryArray.Items.FindByPredicate([&ItemGUID](const FInventoryItemEntry& Entry)
        {
            return Entry.ItemInstance.ItemGUID == ItemGUID;
        });

    if (!FoundEntry || FoundEntry->bIsEquipped) return false;

    FIntPoint ItemSize(1, 1);
    if (!CanPlaceItemAt(TargetPosition, ItemSize, ItemGUID)) return false;

    FoundEntry->GridPosition = TargetPosition;
    InventoryArray.MarkItemDirty(*FoundEntry);
    BroadcastInventoryUpdated();
    return true;
}

bool UInventoryComponent::EquipItem(const FGuid& ItemGUID, FName SlotTag)
{
    if (GetOwner() && !GetOwner()->HasAuthority()) return false;

    FInventoryItemEntry* FoundEntry = InventoryArray.Items.FindByPredicate([&ItemGUID](const FInventoryItemEntry& Entry)
        {
            return Entry.ItemInstance.ItemGUID == ItemGUID;
        });

    if (!FoundEntry) return false;

    FoundEntry->bIsEquipped = true;
    FoundEntry->EquippedSlotTag = SlotTag;
    FoundEntry->GridPosition = FIntPoint(-1, -1);

    InventoryArray.MarkItemDirty(*FoundEntry);
    BroadcastInventoryUpdated();
    return true;
}

bool UInventoryComponent::UnequipItem(const FGuid& ItemGUID, FIntPoint TargetGridPosition)
{
    if (GetOwner() && !GetOwner()->HasAuthority()) return false;

    FInventoryItemEntry* FoundEntry = InventoryArray.Items.FindByPredicate([&ItemGUID](const FInventoryItemEntry& Entry)
        {
            return Entry.ItemInstance.ItemGUID == ItemGUID;
        });

    if (!FoundEntry || !FoundEntry->bIsEquipped) return false;

    if (!CanPlaceItemAt(TargetGridPosition, FIntPoint(1, 1), ItemGUID)) return false;

    FoundEntry->bIsEquipped = false;
    FoundEntry->EquippedSlotTag = NAME_None;
    FoundEntry->GridPosition = TargetGridPosition;

    InventoryArray.MarkItemDirty(*FoundEntry);
    BroadcastInventoryUpdated();
    return true;
}

bool UInventoryComponent::CanPlaceItemAt(FIntPoint TargetPosition, FIntPoint ItemSize, const FGuid& IgnoredItemGUID) const
{
    // 경계 범위 검증
    if (TargetPosition.X < 0 || TargetPosition.Y < 0) return false;
    if (TargetPosition.X + ItemSize.X > GridSize.X || TargetPosition.Y + ItemSize.Y > GridSize.Y) return false;

    // 기존 격자 아이템과 충돌 검증
    for (const FInventoryItemEntry& Entry : InventoryArray.Items)
    {
        if (Entry.bIsEquipped) continue;
        if (IgnoredItemGUID.IsValid() && Entry.ItemInstance.ItemGUID == IgnoredItemGUID) continue;

        FIntPoint ExistingPos = Entry.GridPosition;
        FIntPoint ExistingSize = FIntPoint(1, 1); // 기본 크기 (확장 가능)

        bool bOverlapX = TargetPosition.X < (ExistingPos.X + ExistingSize.X) && (TargetPosition.X + ItemSize.X) > ExistingPos.X;
        bool bOverlapY = TargetPosition.Y < (ExistingPos.Y + ExistingSize.Y) && (TargetPosition.Y + ItemSize.Y) > ExistingPos.Y;

        if (bOverlapX && bOverlapY)
        {
            return false; // 위치 중복됨
        }
    }

    return true;
}

void UInventoryComponent::BroadcastInventoryUpdated()
{
    OnInventoryUpdated.Broadcast();
}
