// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MEO_Project/Core/Types/MEOCharacterTypes.h"
#include "MEO_Project/Subsystems/CraftingSubsystem.h"
#include "MEOSaveGame.generated.h"


UCLASS()
class MEO_PROJECT_API UMEOSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
    UMEOSaveGame();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Data")
    FString SaveSlotName = TEXT("SaveSlot_01");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Data")
    int32 UserIndex = 0;

    // 플레이어 캐릭터 데이터
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Data")
    FMEOPlayerCharacterData CharacterData;

    // 인벤토리 아이템 데이터 (격자 위치 및 장착 정보 포함)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Data")
    TArray<FMEOItemInstance> SavedInventoryItems;

    // 패시브 트리 할당 노드 ID 목록
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Data")
    TArray<FName> SavedAllocatedPassiveNodeIDs;

    // 월드 내 캐릭터 최종 위치
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Data")
    FVector SavedCharacterLocation = FVector::ZeroVector;

};
