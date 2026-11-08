// Fill out your copyright notice in the Description page of Project Settings.


#include "MEO_Project/Components/AnimControlComponent.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"

// Sets default values for this component's properties
UAnimControlComponent::UAnimControlComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

bool UAnimControlComponent::PlaySkillMontage(UAnimMontage* MontageToPlay, float AttackOrCastSpeedStat, FName StartSectionName)
{
    if (!MontageToPlay) return false;

    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (!OwnerCharacter) return false;

    UAnimInstance* AnimInstance = OwnerCharacter->GetMesh() ? OwnerCharacter->GetMesh()->GetAnimInstance() : nullptr;
    if (!AnimInstance) return false;

    // 현재 Recovery 상태가 아닌 공격 동작 중이면 신규 시전 거부
    if (CurrentActionWindow == EActionStateWindow::WindUp || CurrentActionWindow == EActionStateWindow::Active)
    {
        return false;
    }

    // 공격 속도 / 시전 속도 기반 PlayRate 연산수식 (PlayRate = 1.0 + (SpeedStat / 100.0))
    float CalculatedPlayRate = FMath::Max(0.1f, 1.0f + (AttackOrCastSpeedStat / 100.0f));

    float PlayedLength = AnimInstance->Montage_Play(MontageToPlay, CalculatedPlayRate, EMontagePlayReturnType::MontageLength, 0.0f, true);
    if (PlayedLength > 0.0f)
    {
        if (StartSectionName != NAME_None)
        {
            AnimInstance->Montage_JumpToSection(StartSectionName, MontageToPlay);
        }

        CurrentActiveMontage = MontageToPlay;
        CurrentActionWindow = EActionStateWindow::WindUp;

        FOnMontageEnded EndedDelegate;
        EndedDelegate.BindUObject(this, &UAnimControlComponent::OnMontageEndedHandler);
        AnimInstance->Montage_SetEndDelegate(EndedDelegate, MontageToPlay);

        OnActionWindowChanged.Broadcast(CurrentActionWindow);
        return true;
    }

    return false;
}

void UAnimControlComponent::StopCurrentMontage(float BlendOutTime)
{
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (!OwnerCharacter) return;

    UAnimInstance* AnimInstance = OwnerCharacter->GetMesh() ? OwnerCharacter->GetMesh()->GetAnimInstance() : nullptr;
    if (AnimInstance && CurrentActiveMontage)
    {
        AnimInstance->Montage_Stop(BlendOutTime, CurrentActiveMontage);
    }

    CurrentActionWindow = EActionStateWindow::Idle;
    CurrentActiveMontage = nullptr;
    OnActionWindowChanged.Broadcast(CurrentActionWindow);
}

bool UAnimControlComponent::CanCancelCurrentAction(EActionStateWindow RequestedActionType) const
{
    if (CurrentActionWindow == EActionStateWindow::Idle)
    {
        return true;
    }

    // Recovery (후딜레이) 구간에서는 구르기(Dodge) 및 타 스킬 캔슬 허용
    if (CurrentActionWindow == EActionStateWindow::Recovery)
    {
        return true;
    }

    // WindUp / Active 구간에서는 캔슬 불허
    return false;
}

void UAnimControlComponent::DispatchHitFrame(FName SocketOrSkillTag)
{
    OnHitFrameTriggered.Broadcast(SocketOrSkillTag);
}

void UAnimControlComponent::DispatchActionWindowChange(EActionStateWindow NewWindow)
{
    CurrentActionWindow = NewWindow;
    OnActionWindowChanged.Broadcast(CurrentActionWindow);
}

void UAnimControlComponent::OnMontageEndedHandler(UAnimMontage* Montage, bool bInterrupted)
{
    if (CurrentActiveMontage == Montage)
    {
        CurrentActiveMontage = nullptr;
        CurrentActionWindow = EActionStateWindow::Idle;
        OnActionWindowChanged.Broadcast(CurrentActionWindow);
    }
}


