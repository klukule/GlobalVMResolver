// Copyright (C) 2018-2025 FiolaSoft Studio s.r.o.

#pragma once

#include "MVVMViewModelBase.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "MatchoPerUserViewModel.generated.h"

class ULocalPlayer;

/**
 * Base class for view-models that are scoped to a specific LocalPlayer.
 * 
 * If a view-model class derives from UMatchoPerUserViewModel, the manager will
 * keep a distinct instance per (Class, LocalPlayer) pair. Otherwise, the manager
 * keeps a single shared instance per Class (LocalPlayer is treated as null).
 */
UCLASS(MinimalAPI)
class UMatchoPerUserViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	MATCHOGLOBALVMRESOLVER_API void SetOwningPlayer(ULocalPlayer* InLocalPlayer);
	MATCHOGLOBALVMRESOLVER_API ULocalPlayer* GetOwningPlayer() const { return OwningPlayer.Get(); }

protected:
	MATCHOGLOBALVMRESOLVER_API virtual void OnOwningPlayerSet();

private:
	TWeakObjectPtr<ULocalPlayer> OwningPlayer;
};
