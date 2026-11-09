// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MEO_Project/Subsystems/MEOSaveGame.h"
#include "SaveGameSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveGameCompleted, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadGameCompleted, bool, bSuccess);

UCLASS()
class MEO_PROJECT_API USaveGameSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
    // --------------------------------------------------
    // [세이브 / 로드 트랜잭션 API]
    // --------------------------------------------------
    UFUNCTION(BlueprintCallable, Category = "Save System")
    bool SavePlayerData(APawn* TargetPawn, const FString& SlotName = TEXT("SaveSlot_01"));

    UFUNCTION(BlueprintCallable, Category = "Save System")
    bool LoadPlayerData(APawn* TargetPawn, const FString& SlotName = TEXT("SaveSlot_01"));

    UFUNCTION(BlueprintPure, Category = "Save System")
    bool DoesSaveSlotExist(const FString& SlotName = TEXT("SaveSlot_01")) const;

    UPROPERTY(BlueprintAssignable, Category = "Save System|Events")
    FOnSaveGameCompleted OnSaveGameCompleted;

    UPROPERTY(BlueprintAssignable, Category = "Save System|Events")
    FOnLoadGameCompleted OnLoadGameCompleted;

private:
    UPROPERTY()
    TObjectPtr<UMEOSaveGame> CurrentSaveObject = nullptr;

};
