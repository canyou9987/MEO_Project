// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CraftingSubsystem.generated.h"

// 아이템 희귀도
UENUM(BlueprintType)
enum class EItemRarity : uint8
{
    Normal,
    Magic,
    Rare,
    Unique
};

// 어픽스 수식어 타입
UENUM(BlueprintType)
enum class EAffixType : uint8
{
    Prefix,
    Suffix
};

// 어픽스 수치 데이터 구조체
USTRUCT(BlueprintType)
struct FMEOItemAffixData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affix")
    FName AffixID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affix")
    EAffixType AffixType = EAffixType::Prefix;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affix")
    FName StatGroup = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affix")
    int32 Tier = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affix")
    float StatValue = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affix")
    int32 SpawnWeight = 100;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Affix")
    int32 MinItemLevel = 1;
};

// 런타임 아이템 인스턴스 데이터
USTRUCT(BlueprintType)
struct FMEOItemInstance
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FGuid ItemGUID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FName ItemID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    EItemRarity Rarity = EItemRarity::Normal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    int32 ItemLevel = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    int32 Quality = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    TArray<FMEOItemAffixData> PrefixAffixes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    TArray<FMEOItemAffixData> SuffixAffixes;
};
UCLASS()
class MEO_PROJECT_API UCraftingSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
    // 커런시 오브 크래프팅 실행 트랜잭션
    UFUNCTION(BlueprintCallable, Category = "Crafting Engine")
    bool ApplyCurrencyOrb(UPARAM(ref) FMEOItemInstance& TargetItem, FName OrbID);

    // 가중치 무작위 어픽스 스폰
    UFUNCTION(BlueprintCallable, Category = "Crafting Engine")
    bool RollRandomAffix(UPARAM(ref) FMEOItemInstance& TargetItem, EAffixType TargetAffixType, const TArray<FMEOItemAffixData>& AvailableAffixPool);

private:
    const FMEOItemAffixData* SelectWeightedAffix(const TArray<FMEOItemAffixData>& EligibleAffixes);

};
