// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MCPTest_C : ModuleRules
{
	public MCPTest_C(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"MCPTest_C",
			"MCPTest_C/Variant_Platforming",
			"MCPTest_C/Variant_Platforming/Animation",
			"MCPTest_C/Variant_Combat",
			"MCPTest_C/Variant_Combat/AI",
			"MCPTest_C/Variant_Combat/Animation",
			"MCPTest_C/Variant_Combat/Gameplay",
			"MCPTest_C/Variant_Combat/Interfaces",
			"MCPTest_C/Variant_Combat/UI",
			"MCPTest_C/Variant_SideScrolling",
			"MCPTest_C/Variant_SideScrolling/AI",
			"MCPTest_C/Variant_SideScrolling/Gameplay",
			"MCPTest_C/Variant_SideScrolling/Interfaces",
			"MCPTest_C/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
