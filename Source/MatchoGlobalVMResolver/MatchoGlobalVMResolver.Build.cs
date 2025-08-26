// Copyright (C) 2018-2025 FiolaSoft Studio s.r.o.

using UnrealBuildTool;

public class MatchoGlobalVMResolver : ModuleRules
{
	public MatchoGlobalVMResolver(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange([
			"Core",
			"ModelViewViewModel"
		]);

		PrivateDependencyModuleNames.AddRange([
			"CoreUObject",
			"Engine",
			"UMG"
		]);
	}
}