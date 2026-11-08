// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MEO_Project/Core/Types/MEOCharacterTypes.h"
#include "PassiveTreeComponent.generated.h"

// 노드 희귀도 및 기능 분류
UENUM(BlueprintType)
enum class EPassiveNodeType : uint8
{
    Minor,
    Notable,
    Keystone,
    JewelSocket
};

// Dual Spec 무기 세트 바인딩 구획
UENUM(BlueprintType)
enum class EWeaponSetBinding : uint8
{
    Common,       // 공통 적용
    WeaponSet1,   // 무기 세트 1 장착 시만 활성화
    WeaponSet2    // 무기 세트 2 장착 시만 활성화
};

// 그래프 노드 구조체
USTRUCT(BlueprintType)
struct FPassiveGraphNode
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive")
    FName NodeID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive")
    EPassiveNodeType NodeType = EPassiveNodeType::Minor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive")
    EWeaponSetBinding WeaponSetBinding = EWeaponSetBinding::Common;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive")
    TArray<FName> NeighborNodeIDs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive")
    TArray<FStatModifierData> Modifiers;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive")
    bool bIsAllocated = false;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MEO_PROJECT_API UPassiveTreeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    UPassiveTreeComponent();

    // --------------------------------------------------
    // [그래프 할당/반환 & Dual Spec 조작 API]
    // --------------------------------------------------
    UFUNCTION(BlueprintCallable, Category = "Passive Tree")
    bool TryAllocateNode(FName NodeID);

    UFUNCTION(BlueprintCallable, Category = "Passive Tree")
    bool TryRefundNode(FName NodeID);

    UFUNCTION(BlueprintPure, Category = "Passive Tree")
    bool CanRefundNode(FName NodeIDToRefund) const;

    UFUNCTION(BlueprintCallable, Category = "Passive Tree")
    void OnWeaponSetSwapped(int32 NewWeaponSetIndex);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive Data")
    FName ClassStartNodeID = FName("Node_Start_Warrior");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive Data")
    TMap<FName, FPassiveGraphNode> NodeGraph;

    UPROPERTY(BlueprintReadOnly, Category = "Passive Data")
    TSet<FName> AllocatedNodeIDs;

    UPROPERTY(BlueprintReadOnly, Category = "Passive Data")
    int32 CurrentWeaponSetIndex = 0;

private:
    void ReapplyTreeModifiers();

		
};
