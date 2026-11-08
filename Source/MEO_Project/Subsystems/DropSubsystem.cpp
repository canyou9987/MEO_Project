// Fill out your copyright notice in the Description page of Project Settings.


#include "MEO_Project/Subsystems/DropSubsystem.h"
#include "Math/UnrealMathUtility.h"

float UDropSubsystem::CalculateEffectiveIIQ(float RawIIQ) const
{
	const float IIQCap = 100.0f;
	if (RawIIQ <= 0.0f) return 0.0f;
	return RawIIQ / (1.0f + (RawIIQ / IIQCap));
}

float UDropSubsystem::CalculateEffectiveIIR(float RawIIR) const
{
	const float IIRCap = 300.0f;
	if (RawIIR <= 0.0f) return 0.0f;
	return RawIIR / (1.0f + (RawIIR / IIRCap));
}

TArray<FMEOItemInstance> UDropSubsystem::GenerateLootFromTable(FName LootTableID, float RawIIQ, float RawIIR, int32 AreaLevel)
{
    TArray<FMEOItemInstance> DroppedItems;

    float EffectiveIIQ = CalculateEffectiveIIQ(RawIIQ);
    float EffectiveIIR = CalculateEffectiveIIR(RawIIR);

    // 기본 드랍 개수 계산 (IIQ 반영)
    int32 BaseDropCount = FMath::RandRange(1, 3);
    int32 FinalDropCount = FMath::RoundToInt(BaseDropCount * (1.0f + (EffectiveIIQ / 100.0f)));

    for (int32 i = 0; i < FinalDropCount; ++i)
    {
        FMEOItemInstance NewItem;
        NewItem.ItemGUID = FGuid::NewGuid();
        NewItem.ItemLevel = AreaLevel;

        // IIR 반영 희귀도 판정 롤링
        float RarityRoll = FMath::FRand() * (1.0f + (EffectiveIIR / 100.0f));
        if (RarityRoll > 1.8f)
        {
            NewItem.Rarity = EItemRarity::Rare;
        }
        else if (RarityRoll > 1.2f)
        {
            NewItem.Rarity = EItemRarity::Magic;
        }
        else
        {
            NewItem.Rarity = EItemRarity::Normal;
        }

        DroppedItems.Add(NewItem);
    }

    return DroppedItems;
}
