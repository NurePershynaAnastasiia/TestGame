#include "MyCharacter.h"
#include "ItemActor.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"

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

    InventoryWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InventoryWidget"));
    InventoryWidget->SetupAttachment(RootComponent);
    InventoryWidget->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
    InventoryWidget->SetWidgetSpace(EWidgetSpace::Screen);
    InventoryWidget->SetDrawSize(FVector2D(250.f, 100.f));

    InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
    InteractionSphere->SetupAttachment(RootComponent);
    InteractionSphere->SetSphereRadius(150.f);
    InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    InteractionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    InteractionSphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);

}

void AMyCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        PC->PlayerCameraManager->ViewPitchMin = -60.f;
        PC->PlayerCameraManager->ViewPitchMax = 45.f;
    }

    if (IsLocallyControlled())
    {
        InventoryWidget->SetWidgetClass(WBP_InventoryClass);
        InventoryWidget->SetVisibility(true);
    }
    else
    {
        InventoryWidget->SetVisibility(false);
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

    PlayerInputComponent->BindAction("Pickup", IE_Pressed, this, &AMyCharacter::Pickup);
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

void AMyCharacter::Pickup()
{
    UE_LOG(LogTemp, Warning, TEXT("F pressed"));

    if (IsLocallyControlled())
    {
        if (OverlappedItem)
        {
            Server_PickupItem(OverlappedItem);
        }
    }
}

void AMyCharacter::Server_PickupItem_Implementation(AItemActor* Item)
{
    UE_LOG(LogTemp, Warning, TEXT("Server-side pickup"));

    if (Item)
    {
        Inventory.Add(Item->ItemName);

        OnRep_InventoryChanged();

        Item->Destroy();
    }
}

AItemActor* AMyCharacter::GetLookedAtItem()
{
    FVector Start = Camera->GetComponentLocation();
    FVector End = Start + Camera->GetForwardVector() * 500.f;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.f, 0, 1.f);

    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
    {
        AItemActor* HitItem = Cast<AItemActor>(Hit.GetActor());
        if (HitItem)
        {
            UE_LOG(LogTemp, Warning, TEXT("HIT ITEM: %s"), *HitItem->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("HIT Something else: %s"), *Hit.GetActor()->GetName());
        }

        return HitItem;
    }

    return nullptr;
}


void AMyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AMyCharacter, Inventory);
}

void AMyCharacter::OnRep_InventoryChanged()
{
    if (InventoryWidget && InventoryWidget->GetUserWidgetObject())
    {
        UUserWidget* Widget = InventoryWidget->GetUserWidgetObject();

        UTextBlock* InventoryText = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("InventoryText")));
        if (InventoryText)
        {
            FString CombinedText;
            for (const FString& ItemName : Inventory)
            {
                CombinedText += ItemName + TEXT("\n");
            }

            InventoryText->SetText(FText::FromString(CombinedText));
        }
    }
}

AItemActor* AMyCharacter::GetOverlappingItem()
{
    TArray<AActor*> OverlappingActors;
    InteractionSphere->GetOverlappingActors(OverlappingActors, AItemActor::StaticClass());

    if (OverlappingActors.Num() > 0)
    {
        return Cast<AItemActor>(OverlappingActors[0]);
    }

    return nullptr;
}

void AMyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (InventoryWidget)
    {
        FVector CameraLocation;
        FRotator CameraRotation;
        GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(CameraLocation, CameraRotation);

        FRotator LookAtRotation = (CameraLocation - InventoryWidget->GetComponentLocation()).Rotation();
        InventoryWidget->SetWorldRotation(LookAtRotation);
    }

    if (IsLocallyControlled())
    {
        OverlappedItem = nullptr;

        TArray<AActor*> OverlappingActors;
        InteractionSphere->GetOverlappingActors(OverlappingActors, AItemActor::StaticClass());

        for (AActor* Actor : OverlappingActors)
        {
            AItemActor* Item = Cast<AItemActor>(Actor);
            if (Item)
            {
                OverlappedItem = Item;
                break;
            }
        }
    }
}

