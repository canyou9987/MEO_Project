// Fill out your copyright notice in the Description page of Project Settings.


#include "MEO_Project/Subsystems/CraftingSubsystem.h"
#include "Math/UnrealMathUtility.h"

bool UCraftingSubsystem::ApplyCurrencyOrb(UPARAM(ref)FMEOItemInstance& TargetItem, FName OrbID)
{
    if (OrbID.IsNone()) return false;

    // 1. 카오스 오브 (Chaos Orb: 희귀 아이템 옵션 전체 재롤링)
    if (OrbID == "Orb_Chaos")
    {
        if (TargetItem.Rarity != EItemRarity::Rare) return false;

        TargetItem.PrefixAffixes.Empty();
        TargetItem.SuffixAffixes.Empty();

        int32 TargetPrefixCount = FMath::RandRange(2, 3);
        int32 TargetSuffixCount = FMath::RandRange(2, 3);

        TArray<FMEOItemAffixData> DummyPool; // 실제 구현 시 DataTable에서 로드

        for (int32 i = 0; i < TargetPrefixCount; ++i)
        {
            RollRandomAffix(TargetItem, EAffixType::Prefix, DummyPool);
        }
        for (int32 i = 0; i < TargetSuffixCount; ++i)
        {
            RollRandomAffix(TargetItem, EAffixType::Suffix, DummyPool);
        }
        return true;
    }

    // 2. 엑잘티드 오브 (Exalted Orb: 희귀 아이템 빈 슬롯 옵션 추가)
    if (OrbID == "Orb_Exalted")
    {
        if (TargetItem.Rarity != EItemRarity::Rare) return false;

        int32 TotalAffixes = TargetItem.PrefixAffixes.Num() + TargetItem.SuffixAffixes.Num();
        if (TotalAffixes >= 6) return false;

        bool bCanAddPrefix = TargetItem.PrefixAffixes.Num() < 3;
        bool bCanAddSuffix = TargetItem.SuffixAffixes.Num() < 3;

        EAffixType SelectedType = (bCanAddPrefix && bCanAddSuffix)
            ? (FMath::RandBool() ? EAffixType::Prefix : EAffixType::Suffix)
            : (bCanAddPrefix ? EAffixType::Prefix : EAffixType::Suffix);

        TArray<FMEOItemAffixData> DummyPool;
        return RollRandomAffix(TargetItem, SelectedType, DummyPool);
    }

    // 3. 소멸의 오브 (Orb of Annulment: 무작위 옵션 1개 제거)
    if (OrbID == "Orb_Annulment")
    {
        int32 TotalAffixes = TargetItem.PrefixAffixes.Num() + TargetItem.SuffixAffixes.Num();
        if (TotalAffixes == 0) return false;

        int32 RandomIndex = FMath::RandRange(0, TotalAffixes - 1);
        if (RandomIndex < TargetItem.PrefixAffixes.Num())
        {
            TargetItem.PrefixAffixes.RemoveAt(RandomIndex);
        }
        else
        {
            TargetItem.SuffixAffixes.RemoveAt(RandomIndex - TargetItem.PrefixAffixes.Num());
        }
        return true;
    }

    return false;
}

bool UCraftingSubsystem::RollRandomAffix(UPARAM(ref)FMEOItemInstance& TargetItem, EAffixType TargetAffixType, const TArray<FMEOItemAffixData>& AvailableAffixPool)
{
    TArray<FMEOItemAffixData> EligibleAffixes;

    for (const FMEOItemAffixData& Candidate : AvailableAffixPool)
    {
        if (Candidate.AffixType != TargetAffixType) continue;
        if (Candidate.MinItemLevel > TargetItem.ItemLevel) continue;

        // 동일 StatGroup 중복 부여 검증
        bool bAlreadyHasGroup = false;
        auto CheckGroup = [&Candidate](const FMEOItemAffixData& Existing) { return Existing.StatGroup == Candidate.StatGroup; };

        if (TargetItem.PrefixAffixes.ContainsByPredicate(CheckGroup) || TargetItem.SuffixAffixes.ContainsByPredicate(CheckGroup))
        {
            bAlreadyHasGroup = true;
        }

        if (!bAlreadyHasGroup)
        {
            EligibleAffixes.Add(Candidate);
        }
    }

    const FMEOItemAffixData* SelectedAffix = SelectWeightedAffix(EligibleAffixes);
    if (!SelectedAffix) return false;

    if (TargetAffixType == EAffixType::Prefix)
    {
        TargetItem.PrefixAffixes.Add(*SelectedAffix);
    }
    else
    {
        TargetItem.SuffixAffixes.Add(*SelectedAffix);
    }

    return true;
}

const FMEOItemAffixData* UCraftingSubsystem::SelectWeightedAffix(const TArray<FMEOItemAffixData>& EligibleAffixes)
{
    if (EligibleAffixes.Num() == 0) return nullptr;

    int32 TotalWeight = 0;
    for (const FMEOItemAffixData& Affix : EligibleAffixes)
    {
        TotalWeight += FMath::Max(0, Affix.SpawnWeight);
    }

    if (TotalWeight <= 0) return nullptr;

    int32 RandomRoll = FMath::RandRange(1, TotalWeight);
    int32 CurrentAccumulatedWeight = 0;

    for (const FMEOItemAffixData& Affix : EligibleAffixes)
    {
        CurrentAccumulatedWeight += Affix.SpawnWeight;
        if (RandomRoll <= CurrentAccumulatedWeight)
        {
            return &Affix;
        }
    }

    return &EligibleAffixes.Last();
}
