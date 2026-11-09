// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "UISubsystem.generated.h"

// UI 위젯 레이어 분류
UENUM(BlueprintType)
enum class EMEOUIState : uint8
{
    HUDOnly,          // 게임플레이 인게임 HUD만 표시
    InventoryMenu,    // 인벤토리 / 장비 창 오픈
    PassiveTreeMenu,  // 패시브 트리 캔버스 오픈
    PauseMenu         // 일시정지 메뉴 오픈
};

UCLASS()
class MEO_PROJECT_API UUISubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
    // --------------------------------------------------
    // [UI 생성 및 전환 API]
    // --------------------------------------------------
    UFUNCTION(BlueprintCallable, Category = "UI Engine")
    UUserWidget* OpenWidgetOfClass(TSubclassOf<UUserWidget> WidgetClass, bool bSetInputModeUI = true);

    UFUNCTION(BlueprintCallable, Category = "UI Engine")
    bool CloseWidget(UUserWidget* WidgetToClose);

    UFUNCTION(BlueprintCallable, Category = "UI Engine")
    void SwitchUIState(EMEOUIState NewState);

    UFUNCTION(BlueprintPure, Category = "UI Engine")
    EMEOUIState GetCurrentUIState() const { return CurrentUIState; }

protected:
    UPROPERTY(BlueprintReadOnly, Category = "UI Engine")
    EMEOUIState CurrentUIState = EMEOUIState::HUDOnly;

    UPROPERTY()
    TMap<TSubclassOf<UUserWidget>, TObjectPtr<UUserWidget>> ActiveWidgets;

private:
    void UpdateInputModeForState(EMEOUIState State);

};
