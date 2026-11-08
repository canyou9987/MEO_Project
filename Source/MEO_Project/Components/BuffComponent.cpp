// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UBuffComponent::UBuffComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

// Called every frame
void UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    // 서버 권한(Authority)에서만 타이머 차감 및 타이머 만료 처리
    if (GetOwner() && GetOwner()->HasAuthority())
    {
        for (int32 i = ActiveBuffs.Num() - 1; i >= 0; --i)
        {
            FBuffInstanceData& Buff = ActiveBuffs[i];
            if (Buff.Duration > 0.0f) // 영구 버프가 아닌 경우
            {
                Buff.TimeRemaining -= DeltaTime;
                if (Buff.TimeRemaining <= 0.0f)
                {
                    FName ExpiredID = Buff.BuffID;
                    ActiveBuffs.RemoveAt(i);
                    OnBuffStateChanged.Broadcast(ExpiredID, false);
                }
            }
        }
    }
}

void UBuffComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UBuffComponent, ActiveBuffs);
}

bool UBuffComponent::ApplyBuff(FName BuffID, float Duration, int32 Stacks, bool bIsDebuff)
{
    if (BuffID.IsNone()) return false;
    if (GetOwner() && !GetOwner()->HasAuthority()) return false;

    // 이미 주입되어 있는 동일 버프가 있는지 검색
    FBuffInstanceData* ExistingBuff = ActiveBuffs.FindByPredicate([&BuffID](const FBuffInstanceData& Data)
        {
            return Data.BuffID == BuffID;
        });

    if (ExistingBuff)
    {
        // 지속시간 갱신 및 스택 증가
        ExistingBuff->TimeRemaining = FMath::Max(ExistingBuff->TimeRemaining, Duration);
        ExistingBuff->CurrentStacks += Stacks;
    }
    else
    {
        // 신규 버프 생성 및 활성화
        FBuffInstanceData NewBuff;
        NewBuff.BuffID = BuffID;
        NewBuff.CurrentStacks = Stacks;
        NewBuff.Duration = Duration;
        NewBuff.TimeRemaining = Duration;
        NewBuff.bIsDebuff = bIsDebuff;

        ActiveBuffs.Add(NewBuff);
        OnBuffStateChanged.Broadcast(BuffID, true);
    }

    return true;
}

bool UBuffComponent::RemoveBuff(FName BuffID)
{
    if (GetOwner() && !GetOwner()->HasAuthority()) return false;

    int32 RemovedIndex = ActiveBuffs.IndexOfByPredicate([&BuffID](const FBuffInstanceData& Data)
        {
            return Data.BuffID == BuffID;
        });

    if (RemovedIndex != INDEX_NONE)
    {
        ActiveBuffs.RemoveAt(RemovedIndex);
        OnBuffStateChanged.Broadcast(BuffID, false);
        return true;
    }

    return false;
}

bool UBuffComponent::HasBuff(FName BuffID) const
{
    return ActiveBuffs.ContainsByPredicate([&BuffID](const FBuffInstanceData& Data)
        {
            return Data.BuffID == BuffID;
        });
}

