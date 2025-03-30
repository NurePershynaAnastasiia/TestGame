#include "PhysicalCubeActor.h"
#include "Components/StaticMeshComponent.h"

APhysicalCubeActor::APhysicalCubeActor()
{
    bReplicates = true;
    SetReplicateMovement(true);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;

    Mesh->SetIsReplicated(true);

    PrimaryActorTick.bCanEverTick = true;
}

void APhysicalCubeActor::SetImpulseDirection(const FVector& InDirection)
{
    ImpulseDirection = InDirection;
}

void APhysicalCubeActor::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        Mesh->SetSimulatePhysics(true);

        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &APhysicalCubeActor::ApplyImpulse, 0.01f, false);
    }
    else
    {
        Mesh->SetSimulatePhysics(false);
        LastServerLocation = GetActorLocation();
        TargetLocation = GetActorLocation();
    }
}

void APhysicalCubeActor::ApplyImpulse()
{
    Mesh->AddImpulse(ImpulseDirection * 1000.f, NAME_None, true);
    UE_LOG(LogTemp, Warning, TEXT("Impulse applied (delayed): %s"), *ImpulseDirection.ToString());
}

void APhysicalCubeActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!HasAuthority())
    {
        if (ReplicationQuality <= 0)
        {
            // Unreal replicates on itself
            return;
        }

        // Interpolation
        if (!TargetLocation.Equals(GetActorLocation(), 1.0f))
        {
            LastServerLocation = GetActorLocation();
            TargetLocation = GetActorLocation();
        }

        float SpeedMultiplier = FMath::Clamp(1.f + (ReplicationQuality / 20.f), 1.f, 10.f);
        FVector NewLocation = FMath::VInterpTo(LastServerLocation, TargetLocation, DeltaTime, InterpSpeed * SpeedMultiplier);
        SetActorLocation(NewLocation);

        LastServerLocation = NewLocation;
    }
}

