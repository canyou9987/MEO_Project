// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AnimControlComponent.generated.h"

// 3단계 액션 캔슬 윈도우 상태
UENUM(BlueprintType)
enum class EActionStateWindow : uint8
{
	Idle,       // 기본 대기 상태
	WindUp,     // 선딜레이 (캔슬 불허)
	Active,     // 타격/투사체 스폰 (AnimNotify 유발)
	Recovery    // 후딜레이 (구르기 및 타 스킬 캔슬 가능)
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHitFrameTriggered, FName, SocketOrSkillTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionWindowChanged, EActionStateWindow, NewWindow);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MEO_PROJECT_API UAnimControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAnimControlComponent();

    // 공속/시속 스탯 연동 PlayRate 산출 후 몽타주 재생
    UFUNCTION(BlueprintCallable, Category = "Anim Control")
    bool PlaySkillMontage(UAnimMontage* MontageToPlay, float AttackOrCastSpeedStat, FName StartSectionName = NAME_None);

    // 몽타주 즉시 중단 (Recovery 캔슬 또는 피격 경직 발생 시)
    UFUNCTION(BlueprintCallable, Category = "Anim Control")
    void StopCurrentMontage(float BlendOutTime = 0.1f);

    // 현재 액션 윈도우 기반 캔슬 가능 여부 검증
    UFUNCTION(BlueprintPure, Category = "Anim Control")
    bool CanCancelCurrentAction(EActionStateWindow RequestedActionType) const;

    // AnimNotify 호출 전파 인터페이스
    void DispatchHitFrame(FName SocketOrSkillTag);
    void DispatchActionWindowChange(EActionStateWindow NewWindow);

    UPROPERTY(BlueprintAssignable, Category = "Anim Control|Events")
    FOnHitFrameTriggered OnHitFrameTriggered;

    UPROPERTY(BlueprintAssignable, Category = "Anim Control|Events")
    FOnActionWindowChanged OnActionWindowChanged;

    UFUNCTION(BlueprintPure, Category = "Anim Control|State")
    EActionStateWindow GetCurrentActionWindow() const { return CurrentActionWindow; }

protected:
    UPROPERTY(BlueprintReadOnly, Category = "Anim Control|State")
    EActionStateWindow CurrentActionWindow = EActionStateWindow::Idle;

    UPROPERTY()
    TObjectPtr<UAnimMontage> CurrentActiveMontage = nullptr;

private:
    UFUNCTION()
    void OnMontageEndedHandler(UAnimMontage* Montage, bool bInterrupted);
		
};
