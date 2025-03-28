// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "ReplicatedMeshActor.generated.h"

/**
 * 
 */
UCLASS()
class TESTGAME_API AReplicatedMeshActor : public AStaticMeshActor
{
    GENERATED_BODY()

public:
    AReplicatedMeshActor()
    {
        bReplicates = true;
        GetStaticMeshComponent()->SetIsReplicated(true);
    }
};
