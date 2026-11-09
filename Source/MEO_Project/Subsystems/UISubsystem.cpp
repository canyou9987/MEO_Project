// Fill out your copyright notice in the Description page of Project Settings.


#include "MEO_Project/Subsystems/UISubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

UUserWidget* UUISubsystem::OpenWidgetOfClass(TSubclassOf<UUserWidget> WidgetClass, bool bSetInputModeUI)
{
    if (!WidgetClass) return nullptr;

    // 이미 생성되어 존재하는 경우 Viewport 재등록
    if (TObjectPtr<UUserWidget>* ExistingWidget = ActiveWidgets.Find(WidgetClass))
    {
        if (*ExistingWidget)
        {
            if (!(*ExistingWidget)->IsInViewport())
            {
                (*ExistingWidget)->AddToViewport();
            }
            return *ExistingWidget;
        }
    }

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC) return nullptr;

    UUserWidget* NewWidget = CreateWidget<UUserWidget>(PC, WidgetClass);
    if (NewWidget)
    {
        NewWidget->AddToViewport();
        ActiveWidgets.Add(WidgetClass, NewWidget);

        if (bSetInputModeUI)
        {
            FInputModeGameAndUI InputMode;
            InputMode.SetWidgetToFocus(NewWidget->TakeWidget());
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = true;
        }
    }

    return NewWidget;
}

bool UUISubsystem::CloseWidget(UUserWidget* WidgetToClose)
{
    if (!WidgetToClose || !WidgetToClose->IsInViewport()) return false;

    WidgetToClose->RemoveFromParent();

    // 남은 UI 가 없을 경우 Game Only 입력 모드로 전환
    bool bHasActiveViewportWidget = false;
    for (const auto& KVP : ActiveWidgets)
    {
        if (KVP.Value && KVP.Value->IsInViewport())
        {
            bHasActiveViewportWidget = true;
            break;
        }
    }

    if (!bHasActiveViewportWidget)
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (PC)
        {
            FInputModeGameOnly InputMode;
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = false;
        }
        CurrentUIState = EMEOUIState::HUDOnly;
    }

    return true;
}

void UUISubsystem::SwitchUIState(EMEOUIState NewState)
{
    CurrentUIState = NewState;
    UpdateInputModeForState(NewState);
}

void UUISubsystem::UpdateInputModeForState(EMEOUIState State)
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC) return;

    if (State == EMEOUIState::HUDOnly)
    {
        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = false;
    }
    else
    {
        FInputModeGameAndUI InputMode;
        InputMode.SetHideCursorDuringCapture(false);
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = true;
    }
}
