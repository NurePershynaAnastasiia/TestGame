#include "MyCharacter.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"

AMyCharacter::AMyCharacter()
{
    bReplicates = true;

    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 300.f;
    SpringArm->bUsePawnControlRotation = true;
    SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 75.f));

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    Camera->bUsePawnControlRotation = false;
}

void AMyCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        PC->PlayerCameraManager->ViewPitchMin = -60.f;
        PC->PlayerCameraManager->ViewPitchMax = 45.f;
    }
}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMyCharacter::JumpPressed);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMyCharacter::JumpReleased);

    PlayerInputComponent->BindAction("Spawn", IE_Pressed, this, &AMyCharacter::SpawnCube);

    PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
    PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}

void AMyCharacter::MoveForward(float Value)
{
    if (Controller != nullptr && Value != 0.0f)
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}

void AMyCharacter::MoveRight(float Value)
{
    if (Controller != nullptr && Value != 0.0f)
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(Direction, Value);
    }
}

void AMyCharacter::JumpPressed()
{
    Jump();
}

void AMyCharacter::JumpReleased()
{
    StopJumping();
}

void AMyCharacter::SpawnCube()
{
    if (HasAuthority())
    {
        FVector Forward = GetActorForwardVector();
        FVector Up = GetActorUpVector();
        FVector SpawnLocation = GetActorLocation() + Forward * 200.f + Up * 50.f;

        FRotator SpawnRotation = GetActorRotation();

        DrawDebugBox(GetWorld(), SpawnLocation, FVector(20, 20, 20), FColor::Red, false, 5.f);

        FActorSpawnParameters Params;
        Params.Owner = this;

        if (MeshToSpawn)
        {
            GetWorld()->SpawnActor<AActor>(MeshToSpawn, SpawnLocation, SpawnRotation, Params);
        }
    }
    else
    {
        Server_SpawnCube();
    }

    UE_LOG(LogTemp, Warning, TEXT("SpawnCube called. Role: %s, HasAuthority: %s"),
        *UEnum::GetValueAsString(GetLocalRole()),
        HasAuthority() ? TEXT("true") : TEXT("false"));

}

void AMyCharacter::Server_SpawnCube_Implementation()
{
    SpawnCube();

    UE_LOG(LogTemp, Warning, TEXT("Server_SpawnCube_Implementation called"));
}
