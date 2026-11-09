// Fill out your copyright notice in the Description page of Project Settings.


#include "MEO_Project/Subsystems/SaveGameSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "MEO_Project/Components/InventoryComponent.h"
#include "MEO_Project/Components/PassiveTreeComponent.h"
#include "MEO_Project/Components/StatusComponent.h"

bool USaveGameSubsystem::SavePlayerData(APawn* TargetPawn, const FString& SlotName)
{
    if (!TargetPawn) return false;

    UMEOSaveGame* SaveInstance = Cast<UMEOSaveGame>(UGameplayStatics::CreateSaveGameObject(UMEOSaveGame::StaticClass()));
    if (!SaveInstance) return false;

    SaveInstance->SaveSlotName = SlotName;
    SaveInstance->SavedCharacterLocation = TargetPawn->GetActorLocation();

    // 1. 인벤토리 컴포넌트 데이터 수집
    UInventoryComponent* InvComp = TargetPawn->FindComponentByClass<UInventoryComponent>();
    if (InvComp)
    {
        // 인벤토리 항목들을 SaveInstance에 보관
        SaveInstance->SavedInventoryItems.Empty();
        // 실제 데이터 수집 로직 연동
    }

    // 2. 패시브 트리 컴포넌트 할당 노드 수집
    UPassiveTreeComponent* PassiveComp = TargetPawn->FindComponentByClass<UPassiveTreeComponent>();
    if (PassiveComp)
    {
        SaveInstance->SavedAllocatedPassiveNodeIDs.Empty();
        // 활성화된 노드 ID 목록 보관
    }

    // 파일로 디스크 저장
    bool bSaved = UGameplayStatics::SaveGameToSlot(SaveInstance, SlotName, 0);
    CurrentSaveObject = SaveInstance;

    OnSaveGameCompleted.Broadcast(bSaved);
    return bSaved;
}

bool USaveGameSubsystem::LoadPlayerData(APawn* TargetPawn, const FString& SlotName)
{
    if (!TargetPawn || !DoesSaveSlotExist(SlotName)) return false;

    UMEOSaveGame* LoadedSave = Cast<UMEOSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
    if (!LoadedSave) return false;

    CurrentSaveObject = LoadedSave;

    // 1. 위치 복원
    TargetPawn->SetActorLocation(LoadedSave->SavedCharacterLocation);

    // 2. 인벤토리 및 패시브 트리 컴포넌트 복원 로직 연동
    // ...

    OnLoadGameCompleted.Broadcast(true);
    return true;
}

bool USaveGameSubsystem::DoesSaveSlotExist(const FString& SlotName) const
{
	return UGameplayStatics::DoesSaveGameExist(SlotName, 0);
}
