// Copyright (C) 2018-2025 FiolaSoft Studio s.r.o.

#include "ViewModels/MatchoViewModelsManager.h"

#include "Blueprint/UserWidget.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "ViewModels/MatchoPerUserViewModel.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MatchoViewModelsManager)

/*-----------------------------------------------------------------------------
	Helpers for generating class keys.
-----------------------------------------------------------------------------*/

namespace MatchoViewModelsManager_Private
{
	/** Build the key for a (Class, LocalPlayer) VM lookup. LocalPlayer is only used for per-user VM subclasses. */
	FMatchoViewModelClassKey MakeClassKey(const UClass* VMClass, const ULocalPlayer* LocalPlayer)
	{
		return FMatchoViewModelClassKey(VMClass, VMClass->IsChildOf<UMatchoPerUserViewModel>() ? LocalPlayer : nullptr);
	}

	/** Build the key for an existing VM instance. */
	FMatchoViewModelClassKey MakeClassKey(const UObject* ViewModel)
	{
		CA_ASSUME(ViewModel); // Suppress static analysis warning - this method is never called with invalid view model class

		const ULocalPlayer* LocalPlayer = nullptr;

		if (ViewModel->GetClass()->IsChildOf<UMatchoPerUserViewModel>())
		{
			LocalPlayer = Cast<UMatchoPerUserViewModel>(ViewModel)->GetOwningPlayer();
		}

		return FMatchoViewModelClassKey(ViewModel->GetClass(), LocalPlayer);
	}
}


// -------- UMatchoViewModelsManager --------

UObject* UMatchoViewModelsManager::FindViewModel(const UClass* ViewModelClass, const ULocalPlayer* LocalPlayer)
{
	if (!LocalPlayer) return nullptr;

	const UWorld* World = LocalPlayer->GetWorld();
	if (!World) return nullptr;

	const UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance) return nullptr;

	UMatchoViewModelsManager* ViewModelManager = GameInstance->GetSubsystem<UMatchoViewModelsManager>();
	if (!ViewModelManager) return nullptr;

	const TWeakObjectPtr<UObject>* FoundViewModel = ViewModelManager->ViewModelContextCache.Find(MatchoViewModelsManager_Private::MakeClassKey(ViewModelClass, LocalPlayer));
	return FoundViewModel ? FoundViewModel->Get() : nullptr;
}

UObject* UMatchoViewModelsManager::GetOrCreateViewModel(const UClass* ExpectedType, const UUserWidget* UserWidget)
{
	if (!ensureMsgf(ExpectedType, TEXT("GetOrCreateViewModel: ExpectedType is null"))) return nullptr;
	if (!ensureMsgf(UserWidget, TEXT("GetOrCreateViewModel: UserWidget is null"))) return nullptr;

	ULocalPlayer* LocalPlayer = UserWidget->GetOwningLocalPlayer();
	const FMatchoViewModelClassKey Key = MatchoViewModelsManager_Private::MakeClassKey(ExpectedType, LocalPlayer);

	// Reuse if already exists
	if (const TWeakObjectPtr<UObject>* FoundPtr = ViewModelContextCache.Find(Key))
	{
		if (UObject* FoundViewModel = FoundPtr->Get())
		{
			ViewModelUsageCache.Emplace(FoundViewModel, UserWidget);
			return FoundViewModel;
		}
	}

	// Create a new instance.
	UObject* NewViewModel = NewObject<UObject>(this, ExpectedType);
	if (!ensureMsgf(NewViewModel, TEXT("Failed to create view-model of class %s"), *GetNameSafe(ExpectedType)))
	{
		return nullptr;
	}

	// If per-user VM, assign the owning player.
	if (UMatchoPerUserViewModel* PerUserViewModel = Cast<UMatchoPerUserViewModel>(NewViewModel))
	{
		PerUserViewModel->SetOwningPlayer(LocalPlayer);
	}

	// Track usage by this widget and register in the class-key map.
	ViewModelUsageCache.Emplace(NewViewModel, UserWidget);
	ViewModelContextCache.Emplace(Key, NewViewModel);

	constexpr EMatchoViewModelsDestructionType DestructionType = EMatchoViewModelsDestructionType::AfterLastUsage; // GetDestructionTypeForClass(ExpectedType);
	ViewModelCache.Emplace(NewViewModel, DestructionType);

	return NewViewModel;
}

void UMatchoViewModelsManager::RemoveViewModel(const UObject* ViewModel, const TObjectPtr<UUserWidget> UserWidget)
{
	if (!ViewModel || !UserWidget) return;

	ViewModelUsageCache.Remove(ViewModel, UserWidget);

	// If nothing else uses this VM, decide based on policy.
	if (ViewModelUsageCache.Num(ViewModel) == 0)
	{
		if (const EMatchoViewModelsDestructionType* DestructionType = ViewModelCache.Find(ViewModel))
		{
			if (*DestructionType == EMatchoViewModelsDestructionType::AfterLastUsage)
			{
				ViewModelCache.Remove(ViewModel);
				ViewModelContextCache.Remove(MatchoViewModelsManager_Private::MakeClassKey(ViewModel));
			}
		}
	}
}

void UMatchoViewModelsManager::RemoveViewModel(const UObject* ViewModel)
{
	if (!ViewModel) return;

	ViewModelCache.Remove(ViewModel);
	ViewModelUsageCache.Remove(ViewModel);
	ViewModelContextCache.Remove(MatchoViewModelsManager_Private::MakeClassKey(ViewModel));
}
