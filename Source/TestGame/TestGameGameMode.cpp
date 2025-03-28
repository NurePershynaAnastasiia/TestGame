// Copyright Epic Games, Inc. All Rights Reserved.

#include "TestGameGameMode.h"
#include "TestGameCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATestGameGameMode::ATestGameGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
