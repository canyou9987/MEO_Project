// Fill out your copyright notice in the Description page of Project Settings.


#include "MEO_Project/Components/StatusComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UStatusComponent::UStatusComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	// ...
}

void UStatusComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UStatusComponent, CurrentHealth);
}

void UStatusComponent::RecalculateStats()
{
	TotalIncreasedHealthPercent = 0.0f;

	for (const FStatModifierData& Mod : ActiveModifiers)
	{
		if (Mod.StatType == EMEOStatType::MaxHealth)
		{
			if (Mod.ModifierType == EStatModifierType::Increased)
			{
				TotalIncreasedHealthPercent += Mod.Value;
			}
		}
	}

	CalculatedMaxHealth = BaseMaxHealth * (1.0f + (TotalIncreasedHealthPercent / 100.0f));
	CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, CalculatedMaxHealth);
}

void UStatusComponent::ApplyStatModifier(const FStatModifierData& Modifier)
{
	ActiveModifiers.Add(Modifier);
	RecalculateStats();
}

void UStatusComponent::RemoveStatModifier(const FStatModifierData& Modifier)
{
	ActiveModifiers.RemoveSingleSwap(Modifier);
	RecalculateStats();
}

void UStatusComponent::ModifyHealth(float Amount)
{
	if (GetOwner() && !GetOwner()->HasAuthority()) return;

	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.0f, CalculatedMaxHealth);
	OnHealthChanged.Broadcast(CurrentHealth, CalculatedMaxHealth);
}

void UStatusComponent::OnRep_CurrentHealth()
{
	OnHealthChanged.Broadcast(CurrentHealth, CalculatedMaxHealth);
}