// Fill out your copyright notice in the Description page of Project Settings.


#include "DamagePipelineSubsystem.h"
#include "MEO_Project/Components/BuffComponent.h"
#include "Math/UnrealMathUtility.h"

//class UBuffComponent;

FDamagePacket UDamagePipelineSubsystem::CalculateFinalDamage(const FDamageCalculationContext& Context, EHitQuality& OutHitQuality)
{
    // [Step 1 & 2] 베이스 피해 및 속성 전환 / Extra 계산
    FDamagePacket CalculatedDamage = ApplyConversionAndExtra(Context);

    // [Step 3] 스탯 증폭 (Increased & More Multipliers)
    float IncCoeff = 1.0f + (Context.TotalIncreasedPercent / 100.0f);
    float MoreCoeff = 1.0f;
    for (float MoreVal : Context.MoreMultipliers)
    {
        MoreCoeff *= (1.0f + (MoreVal / 100.0f));
    }

    float TotalStatBonus = IncCoeff * MoreCoeff;
    CalculatedDamage.Physical *= TotalStatBonus;
    CalculatedDamage.Fire *= TotalStatBonus;
    CalculatedDamage.Cold *= TotalStatBonus;
    CalculatedDamage.Lightning *= TotalStatBonus;
    CalculatedDamage.Chaos *= TotalStatBonus;

    // [Step 4] 명중 등급 및 치명타/막기 판정
    OutHitQuality = DetermineHitQuality(Context);
    float QualityCoeff = 1.0f;

    switch (OutHitQuality)
    {
    case EHitQuality::Critical:
        QualityCoeff = Context.CritMultiplier;
        break;
    case EHitQuality::Glancing:
        QualityCoeff = 0.5f;
        break;
    case EHitQuality::Blocked:
        QualityCoeff = FMath::Clamp(1.0f - Context.DefenderBlockReduction, 0.0f, 1.0f);
        break;
    case EHitQuality::FullHit:
    default:
        QualityCoeff = 1.0f;
        break;
    }

    CalculatedDamage.Physical *= QualityCoeff;
    CalculatedDamage.Fire *= QualityCoeff;
    CalculatedDamage.Cold *= QualityCoeff;
    CalculatedDamage.Lightning *= QualityCoeff;
    CalculatedDamage.Chaos *= QualityCoeff;

    // [Step 5] 방어자 감쇄 적용 (방어도 & 원소 저항 캡 적용)
    return EvaluateMitigation(Context, CalculatedDamage, OutHitQuality);
}

void UDamagePipelineSubsystem::ProcessStatusAilments(AActor* Defender, const FDamagePacket& FinalDamage, float DefenderMaxHealth)
{
    if (!IsValid(Defender) || DefenderMaxHealth <= 0.0f) return;

    float TotalDamage = FinalDamage.GetTotalDamage();
    float DamageRatio = TotalDamage / DefenderMaxHealth;

    UBuffComponent* BuffComp = Defender->FindComponentByClass<UBuffComponent>();
    if (!BuffComp) return;

    // 단일 타격 체력 비율 기준 상태이상 트리거
    if (DamageRatio >= 0.05f && FinalDamage.Cold > 0.0f)
    {
        // 동결/냉기 상태이상 주입
        BuffComp->ApplyBuff(FName("Buff_Freeze"), 3.0f);
    }
    if (DamageRatio >= 0.002f && FinalDamage.Lightning > 0.0f)
    {
        // 감전 상태이상 주입
        BuffComp->ApplyBuff(FName("Buff_Shock"), 4.0f);
    }
    if (DamageRatio >= 0.01f && FinalDamage.Fire > 0.0f)
    {
        // 점화 상태이상 주입
        BuffComp->ApplyBuff(FName("Buff_Ignite"), 4.0f);
    }
}

FDamagePacket UDamagePipelineSubsystem::ApplyConversionAndExtra(const FDamageCalculationContext& Context)
{
    // 단방향 피해 구조 원형 보존
    return Context.BaseDamage;
}

EHitQuality UDamagePipelineSubsystem::DetermineHitQuality(const FDamageCalculationContext& Context)
{
    // 1. 방패 막기 판정
    if (Context.DefenderBlockChance > 0.0f && FMath::FRand() < (Context.DefenderBlockChance / 100.0f))
    {
        return EHitQuality::Blocked;
    }

    // 2. 정확도 vs 회피도 명중률 비율 연산
    float AccuracyRatio = Context.AttackerAccuracy / FMath::Max(1.0f, Context.AttackerAccuracy + Context.DefenderEvasion);
    float RandRoll = FMath::FRand();

    // 3. 치명타 및 명중 등급 판정
    if (RandRoll < (Context.CritChance * AccuracyRatio))
    {
        return EHitQuality::Critical;
    }
    if (RandRoll < AccuracyRatio)
    {
        return EHitQuality::FullHit;
    }

    return EHitQuality::Glancing;
}

FDamagePacket UDamagePipelineSubsystem::EvaluateMitigation(const FDamageCalculationContext& Context, const FDamagePacket& IncomingDamage, EHitQuality HitQuality)
{
    FDamagePacket Mitigated = IncomingDamage;

    // 물리 방어도 감쇄 수식 (Armour Damage Reduction)
    if (Mitigated.Physical > 0.0f && Context.DefenderArmour > 0.0f)
    {
        float ArmourReduction = Context.DefenderArmour / (Context.DefenderArmour + (10.0f * Mitigated.Physical));
        Mitigated.Physical *= (1.0f - FMath::Clamp(ArmourReduction, 0.0f, 0.90f)); // 최대 90% 방어 한계
    }

    // 원소 저항 감쇄 (최대 75% Resistance Cap 적용)
    const float MaxResistCap = 75.0f;

    auto ApplyResist = [](float Damage, float ResistVal, float MaxCap) -> float
        {
            float EffectiveResist = FMath::Min(ResistVal, MaxCap) / 100.0f;
            return Damage * (1.0f - EffectiveResist);
        };

    Mitigated.Fire = ApplyResist(Mitigated.Fire, Context.DefenderFireResist, MaxResistCap);
    Mitigated.Cold = ApplyResist(Mitigated.Cold, Context.DefenderColdResist, MaxResistCap);
    Mitigated.Lightning = ApplyResist(Mitigated.Lightning, Context.DefenderLightningResist, MaxResistCap);
    Mitigated.Chaos = ApplyResist(Mitigated.Chaos, Context.DefenderChaosResist, MaxResistCap);

    return Mitigated;
}
