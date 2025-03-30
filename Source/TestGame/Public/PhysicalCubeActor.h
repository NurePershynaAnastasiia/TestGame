#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PhysicalCubeActor.generated.h"

UCLASS()
class TESTGAME_API APhysicalCubeActor : public AActor
{
    GENERATED_BODY()

public:
    APhysicalCubeActor();

    void SetImpulseDirection(const FVector& InDirection);

    void ApplyImpulse();

    virtual void Tick(float DeltaTime) override;

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* Mesh;

    UPROPERTY(EditAnywhere, Category = "Optimization")
    int32 ReplicationQuality = 50; // 0 = precise, 100 = smoothly

    FVector ImpulseDirection = FVector::ForwardVector;

    FVector LastServerLocation;
    FVector TargetLocation;

    float InterpSpeed = 5.0f;
};
