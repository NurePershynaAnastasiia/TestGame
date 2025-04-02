#include "PhysicalCubeActor.h"
#include "Components/StaticMeshComponent.h"
#include "MyGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

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

    if (HasAuthority())
    {
        ServerPosition = GetActorLocation();
        return;
    }

    int32 Quality = 50;
    AMyGameState* MyGS = Cast<AMyGameState>(UGameplayStatics::GetGameState(GetWorld()));
    if (MyGS) { Quality = MyGS->GlobalReplicationQuality; }

    //if (Quality <= 0) return;

    float SpeedMultiplier = FMath::Clamp(1.f + (Quality / 20.f), 1.f, 10.f);
    float ActualSpeed = InterpSpeed * SpeedMultiplier;

    FVector NewLocation = FMath::VInterpTo(GetActorLocation(), TargetLocation, DeltaTime, ActualSpeed);
    SetActorLocation(NewLocation);

    float DistanceToTarget = FVector::Dist(NewLocation, TargetLocation);

    UE_LOG(LogTemp, Warning, TEXT("[Interp] ReplicationQuality = %d"), Quality);
    UE_LOG(LogTemp, Warning, TEXT("[Interp] SpeedMultiplier = %.2f"), SpeedMultiplier);
    UE_LOG(LogTemp, Warning, TEXT("[Interp] FinalInterpSpeed = %.2f"), ActualSpeed);
    UE_LOG(LogTemp, Warning, TEXT("[Interp] CurrentLocation = %s"), *NewLocation.ToString());
    UE_LOG(LogTemp, Warning, TEXT("[Interp] TargetLocation = %s"), *TargetLocation.ToString());
    UE_LOG(LogTemp, Warning, TEXT("[Interp] DistanceToTarget = %.2f"), DistanceToTarget);
}

void APhysicalCubeActor::OnRep_ServerPosition()
{
    TargetLocation = ServerPosition;
}

void APhysicalCubeActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(APhysicalCubeActor, ServerPosition);
}
