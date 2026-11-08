// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"

// 개별 버프/디버프 런타임 데이터 구조체
USTRUCT(BlueprintType)
struct FBuffInstanceData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
    FName BuffID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
    int32 CurrentStacks = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
    float Duration = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
    float TimeRemaining = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
    bool bIsDebuff = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBuffStateChanged, FName, BuffID, bool, bAdded);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MEO_PROJECT_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    UBuffComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // --------------------------------------------------
    // [버프 / 디버프 주입 및 제거 API]
    // --------------------------------------------------
    // UDamagePipelineSubsystem 및 스킬 시스템에서 상태이상 주입 시 호출
    UFUNCTION(BlueprintCallable, Category = "Buff System")
    bool ApplyBuff(FName BuffID, float Duration, int32 Stacks = 1, bool bIsDebuff = true);

    UFUNCTION(BlueprintCallable, Category = "Buff System")
    bool RemoveBuff(FName BuffID);

    UFUNCTION(BlueprintPure, Category = "Buff System")
    bool HasBuff(FName BuffID) const;

    UPROPERTY(BlueprintAssignable, Category = "Buff System|Events")
    FOnBuffStateChanged OnBuffStateChanged;

protected:
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Buff System|State")
    TArray<FBuffInstanceData> ActiveBuffs;

		
};
