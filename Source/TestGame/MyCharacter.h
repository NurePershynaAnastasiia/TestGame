#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PhysicalCubeActor.h"
#include "MyCharacter.generated.h"

// Forward declaration
class AItemActor;
class USphereComponent;
class UWidgetComponent;
class UUserWidget;

UCLASS()
class TESTGAME_API AMyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AMyCharacter();

    virtual void Tick(float DeltaTime) override;

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // === Movement ===
    void MoveForward(float Value);
    void MoveRight(float Value);
    void JumpPressed();
    void JumpReleased();

    // === Spawning ===
    void SpawnCube();

    UFUNCTION(Server, Reliable)
    void Server_SpawnCube();

    // === Interaction ===
    void Pickup();
    AItemActor* GetOverlappingItem();
    AItemActor* GetLookedAtItem();

    UFUNCTION(Server, Reliable)
    void Server_PickupItem(AItemActor* Item);

    // === Inventory Replication ===
    UFUNCTION()
    void OnRep_InventoryChanged();

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_UpdateInventoryWidget();

public:
    // === Components ===
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    UCameraComponent* Camera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    UWidgetComponent* InventoryWidget;

    UPROPERTY(VisibleAnywhere)
    USphereComponent* InteractionSphere;

    // === Inventory ===
    UPROPERTY(ReplicatedUsing = OnRep_InventoryChanged)
    TArray<FString> Inventory;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UUserWidget> WBP_InventoryClass;

    // === Spawning Settings ===
    UPROPERTY(EditDefaultsOnly, Category = "Spawn")
    TSubclassOf<APhysicalCubeActor> MeshToSpawn;

    // === State ===
    AItemActor* OverlappedItem;
};
