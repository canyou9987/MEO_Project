// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MEO_Project/Subsystems/CraftingSubsystem.h"
#include "DropSubsystem.generated.h"


UCLASS()
class MEO_PROJECT_API UDropSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
    // IIQ / IIR 점감 수획 연산 (Diminishing Returns)
    UFUNCTION(BlueprintPure, Category = "Loot Engine")
    float CalculateEffectiveIIQ(float RawIIQ) const;

    UFUNCTION(BlueprintPure, Category = "Loot Engine")
    float CalculateEffectiveIIR(float RawIIR) const;

    // 몬스터 처치 시 루트 드랍 생성 메인 파이프라인
    UFUNCTION(BlueprintCallable, Category = "Loot Engine")
    TArray<FMEOItemInstance> GenerateLootFromTable(FName LootTableID, float RawIIQ, float RawIIR, int32 AreaLevel);

};
