// Copyright (C) 2018-2025 FiolaSoft Studio s.r.o.

#include "ViewModels/MatchoPerUserViewModel.h"
#include "Engine/LocalPlayer.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(MatchoPerUserViewModel)

void UMatchoPerUserViewModel::SetOwningPlayer(ULocalPlayer* InLocalPlayer)
{
	if (InLocalPlayer && InLocalPlayer != OwningPlayer)
	{
		OwningPlayer = InLocalPlayer;
		OnOwningPlayerSet();
	}
}

void UMatchoPerUserViewModel::OnOwningPlayerSet()
{
	// Intentionally empty; subclasses can react to player switch here.
}
