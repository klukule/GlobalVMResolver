// Copyright (C) 2018-2025 FiolaSoft Studio s.r.o.

#pragma once

#include "CoreMinimal.h"
#include "View/MVVMViewModelContextResolver.h"
#include "MatchoGlobalVMResolver.generated.h"

/**
 * Global View-Model resolver that delegates lifetime and reuse to UMatchoViewModelsManager.
 *
 * Behavior:
 *  - On CreateInstance: asks the subsystem for an instance (shared per class, or per (class, LocalPlayer)
 *    if the VM derives from UMatchoPerUserViewModel). Also records that the requesting widget uses it.
 *  - On DestroyInstance: tells the subsystem that the widget no longer uses the VM; the subsystem may
 *    auto-destroy it when usage drops to zero depending on its destruction policy.
 */
UCLASS(MinimalAPI)
class UMatchoGlobalVMResolver : public UMVVMViewModelContextResolver
{
	GENERATED_BODY()

public:
	MATCHOGLOBALVMRESOLVER_API virtual UObject* CreateInstance(const UClass* ExpectedType, const UUserWidget* UserWidget, const UMVVMView* View) const override;
	MATCHOGLOBALVMRESOLVER_API virtual void DestroyInstance(const UObject* ViewModel, const UMVVMView* View) const override;
};
