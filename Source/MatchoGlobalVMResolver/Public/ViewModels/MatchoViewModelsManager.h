// Copyright (C) 2018-2025 FiolaSoft Studio s.r.o.

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "MatchoViewModelsManager.generated.h"

class ULocalPlayer;
class UUserWidget;

/** Destruction policy for cached view-models. */
UENUM()
enum class EMatchoViewModelsDestructionType : uint8
{
	/** View-model is destroyed automatically after its last tracked user goes away. */
	AfterLastUsage,

	/** View-model lifetime is managed manually by the caller. */
	Manual
};

/**
 * Key used to identify a view-model instance in the registry:
 *  - VMClass: the class of the view-model
 *  - LocalPlayer: only set for per-user VMs (i.e., subclasses of UMatchoPerUserViewModel), otherwise null
 */
using FMatchoViewModelClassKey = TTuple<TWeakObjectPtr<const UClass>, const ULocalPlayer*>;

/**
 * Subsystem responsible for providing a small service-locator/registry for ViewModels.
 * 
 * - For plain VMs: one instance per class.
 * - For per-user VMs (UMatchoPerUserViewModel): one instance per (class, local player).
 * - Tracks usage by user-widgets to optionally auto-destroy VMs after last usage.
 */
UCLASS(MinimalAPI)
class UMatchoViewModelsManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/** Find a view-model of given type for a PlayerController's local player (returns nullptr if not found). */
	template <typename TViewModelClass>
	static TViewModelClass* FindViewModel(APlayerController* InPlayerController);

private:
	MATCHOGLOBALVMRESOLVER_API static UObject* FindViewModel(const UClass* ViewModelClass, const ULocalPlayer* LocalPlayer);

public:
	/**
	 * Get or create a view-model instance of ExpectedType.
	 * - If ExpectedType derives from UMatchoPerUserViewModel, the instance is scoped to UserWidget->GetOwningLocalPlayer().
	 * - Otherwise the instance is shared per class.
	 * The instance is marked as "in use" by the given UserWidget.
	 */
	MATCHOGLOBALVMRESOLVER_API UObject* GetOrCreateViewModel(const UClass* ExpectedType, const UUserWidget* UserWidget);

	/** Stop tracking usage of ViewModel by a specific UserWidget; may auto-destroy based on policy. */
	MATCHOGLOBALVMRESOLVER_API void RemoveViewModel(const UObject* ViewModel, TObjectPtr<UUserWidget> UserWidget);

	/** Removes view-model instance from tracking - used to destroy view-models with manual lifetime management. */
	MATCHOGLOBALVMRESOLVER_API void RemoveViewModel(const UObject* ViewModel);

private:
	/** Cached view-models and their respective destruction policies. */
	UPROPERTY(Transient)
	TMap<TObjectPtr<UObject>, EMatchoViewModelsDestructionType> ViewModelCache;

	/** Tracks which widgets currently use a given view-model. */
	TMultiMap<TWeakObjectPtr<const UObject>, TWeakObjectPtr<const UUserWidget>> ViewModelUsageCache;

	/** Registry of view-model instances for given context based on view-model type and player. */
	TMap<FMatchoViewModelClassKey, TWeakObjectPtr<UObject>> ViewModelContextCache;
};

template <typename TViewModelClass>
TViewModelClass* UMatchoViewModelsManager::FindViewModel(APlayerController* InPlayerController)
{
	ULocalPlayer* LocalPlayer = InPlayerController ? InPlayerController->GetLocalPlayer() : nullptr;
	return Cast<TViewModelClass>(FindViewModel(TViewModelClass::StaticClass(), LocalPlayer));
}
