// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DamagePipelineSubsystem.generated.h"

// 판정 명중 등급
UENUM(BlueprintType)
enum class EHitQuality : uint8
{
    Glancing, // 빗맞춤 (50% 피해)
    FullHit,  // 정타 (100% 피해)
    Critical, // 치명타 (CritMultiplier 배율 적용)
    Blocked   // 방패 막기 (BlockReduction 피해 경감)
};

// 속성별 피해 가중치 패킷
USTRUCT(BlueprintType)
struct FDamagePacket
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float Physical = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float Fire = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float Cold = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float Lightning = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float Chaos = 0.0f;

    float GetTotalDamage() const
    {
        return Physical + Fire + Cold + Lightning + Chaos;
    }
};

// 데미지 계산 파이프라인 컨텍스트 데이터
USTRUCT(BlueprintType)
struct FDamageCalculationContext
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Context")
    TObjectPtr<AActor> Attacker = nullptr;

    UPROPERTY(BlueprintReadWrite, Category = "Context")
    TObjectPtr<AActor> Defender = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context")
    FDamagePacket BaseDamage;

    // 공격 스탯 파라미터
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attacker Stats")
    float AttackerAccuracy = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attacker Stats")
    float CritChance = 0.05f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attacker Stats")
    float CritMultiplier = 1.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attacker Stats")
    float TotalIncreasedPercent = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attacker Stats")
    TArray<float> MoreMultipliers;

    // 방어자 스탯 및 저항 파라미터
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defender Stats")
    float DefenderArmour = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defender Stats")
    float DefenderEvasion = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defender Stats")
    float DefenderBlockChance = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defender Stats")
    float DefenderBlockReduction = 0.6f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defender Stats")
    float DefenderFireResist = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defender Stats")
    float DefenderColdResist = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defender Stats")
    float DefenderLightningResist = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defender Stats")
    float DefenderChaosResist = 0.0f;
};

UCLASS()
class MEO_PROJECT_API UDamagePipelineSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
    // 6단계 피해 계산 메인 진입점
    UFUNCTION(BlueprintCallable, Category = "Combat Pipeline")
    FDamagePacket CalculateFinalDamage(const FDamageCalculationContext& Context, EHitQuality& OutHitQuality);

    // 피격 대상 체력 비율 기반 상태이상 유발 판정
    UFUNCTION(BlueprintCallable, Category = "Combat Pipeline")
    void ProcessStatusAilments(AActor* Defender, const FDamagePacket& FinalDamage, float DefenderMaxHealth);

private:
    FDamagePacket ApplyConversionAndExtra(const FDamageCalculationContext& Context);
    EHitQuality DetermineHitQuality(const FDamageCalculationContext& Context);
    FDamagePacket EvaluateMitigation(const FDamageCalculationContext& Context, const FDamagePacket& IncomingDamage, EHitQuality HitQuality);
	
};
