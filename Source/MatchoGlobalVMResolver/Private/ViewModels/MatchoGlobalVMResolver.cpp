// Copyright (C) 2018-2025 FiolaSoft Studio s.r.o.

#include "ViewModels/MatchoGlobalVMResolver.h"

#include "ViewModels/MatchoViewModelsManager.h"
#include "Blueprint/UserWidget.h"
#include "Engine/GameInstance.h"
#include "View/MVVMView.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MatchoGlobalVMResolver)

UObject* UMatchoGlobalVMResolver::CreateInstance(const UClass* ExpectedType, const UUserWidget* UserWidget, const UMVVMView* View) const
{
	if (!ensureMsgf(ExpectedType, TEXT("MatchoGlobalVMResolver.CreateInstance: ExpectedType is null"))) return nullptr;
	if (!UserWidget) return nullptr;

	const UGameInstance* GameInstance = UserWidget->GetGameInstance();
	if (!GameInstance) return nullptr;

	if (UMatchoViewModelsManager* VMManager = GameInstance->GetSubsystem<UMatchoViewModelsManager>())
	{
		// Manager handles per-user vs global scoping and tracks this widget as a user.
		return VMManager->GetOrCreateViewModel(ExpectedType, UserWidget);
	}
	return nullptr;
}

void UMatchoGlobalVMResolver::DestroyInstance(const UObject* ViewModel, const UMVVMView* View) const
{
	if (!View) return;

	UUserWidget* Widget = View->GetOuterUUserWidget();
	if (!Widget) return;

	const UGameInstance* GameInstance = Widget->GetGameInstance();
	if (!GameInstance) return;

	if (UMatchoViewModelsManager* VMManager = GameInstance->GetSubsystem<UMatchoViewModelsManager>())
	{
		// Untrack usage for this specific widget; manager decides whether to keep or drop the VM.
		VMManager->RemoveViewModel(ViewModel, Widget);
	}
}
