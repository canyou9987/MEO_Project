// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MEO_Project/Core/Types/MEOCharacterTypes.h"
#include "StatusComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, NewHealth, float, MaxHealth);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MEO_PROJECT_API UStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatusComponent();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // --------------------------------------------------
    // [스탯 연산 및 수정 API]
    // --------------------------------------------------
    UFUNCTION(BlueprintCallable, Category = "Status")
    void RecalculateStats();

    UFUNCTION(BlueprintCallable, Category = "Status")
    void ApplyStatModifier(const FStatModifierData& Modifier);

    UFUNCTION(BlueprintCallable, Category = "Status")
    void RemoveStatModifier(const FStatModifierData& Modifier);

    // --------------------------------------------------
    // [체력 / 자원 상태]
    // --------------------------------------------------
    UFUNCTION(BlueprintCallable, Category = "Status")
    void ModifyHealth(float Amount);

    UFUNCTION(BlueprintPure, Category = "Status")
    float GetCurrentHealth() const { return CurrentHealth; }

    UFUNCTION(BlueprintPure, Category = "Status")
    float GetMaxHealth() const { return CalculatedMaxHealth; }

    UPROPERTY(BlueprintAssignable, Category = "Status|Events")
    FOnHealthChanged OnHealthChanged;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Stats")
    float BaseMaxHealth = 100.0f;

    UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth, BlueprintReadOnly, Category = "Runtime Stats")
    float CurrentHealth = 100.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Runtime Stats")
    float CalculatedMaxHealth = 100.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Runtime Stats")
    float TotalIncreasedHealthPercent = 0.0f;

    UFUNCTION()
    void OnRep_CurrentHealth();

private:
    TArray<FStatModifierData> ActiveModifiers;
};
