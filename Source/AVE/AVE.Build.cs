// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AVE : ModuleRules
{
	public AVE(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "ProceduralMeshComponent", "Niagara", "AIModule", "NavigationSystem", "GameplayTasks", "UMG" ,"AnimGraphRuntime"});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		PublicDefinitions.Add("MYDebug_SM(x)=if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT(x));}");

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
