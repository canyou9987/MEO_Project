// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Math/UnrealMathUtility.h"
#include "MEOCharacterTypes.generated.h"

//  언리얼 엔진 코어(Stats.h)의 EStatType 충돌 방지를 위해 EMEOStatType으로 변경
//  스탯 종류
UENUM(BlueprintType)
enum class EMEOStatType : uint8
{
    MaxHealth,
    MaxMana,
    Armour,
    Evasion,
    EnergyShield,
    FireResist,
    ColdResist,
    LightningResist,
    ChaosResist,
    AttackSpeed,
    CastSpeed,
    MovementSpeed,
    PhysicalDamage,
    FireDamage,
    ColdDamage,
    LightningDamage,
    ChaosDamage,
    CritChance,
    CritMultiplier
};

// 스탯 적용 연산 방식 (Flat: 고정값, Increased: % 합연산 증가, More: % 곱연산 증폭)
UENUM(BlueprintType)
enum class EStatModifierType : uint8
{
    Flat,
    Increased,
    More
};

// 스탯 모디파이어 데이터 구조체
USTRUCT(BlueprintType)
struct FStatModifierData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat Modifier")
    EMEOStatType StatType = EMEOStatType::MaxHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat Modifier")
    EStatModifierType ModifierType = EStatModifierType::Flat;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat Modifier")
    float Value = 0.0f;

    bool operator==(const FStatModifierData& Other) const
    {
        return StatType == Other.StatType && ModifierType == Other.ModifierType && FMath::IsNearlyEqual(Value, Other.Value);
    }
};

// 플레이어 캐릭터 세이브/런타임 데이터 컨테이너
USTRUCT(BlueprintType)
struct FMEOPlayerCharacterData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Data")
    FName CharacterClass = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Data")
    int32 Level = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Data")
    int64 CurrentExp = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Data")
    int32 AvailablePassivePoints = 0;
};