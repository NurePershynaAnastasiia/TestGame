#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemActor.generated.h"

UCLASS()
class TESTGAME_API AItemActor : public AActor
{
    GENERATED_BODY()

public:
    AItemActor();

    UPROPERTY(EditDefaultsOnly, Category = "Item")
    FString ItemName;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* Mesh;
};
