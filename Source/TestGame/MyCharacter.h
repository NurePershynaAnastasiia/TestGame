#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "MyCharacter.generated.h"

class AItemActor;

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

    void MoveForward(float Value);
    void MoveRight(float Value);
    void JumpPressed();
    void JumpReleased();
    void SpawnCube();

    UFUNCTION(Server, Reliable)
    void Server_SpawnCube();

    void Pickup();

    AItemActor* GetOverlappingItem();

    AItemActor* GetLookedAtItem();

    UFUNCTION(Server, Reliable)
    void Server_PickupItem(AItemActor* Item);

    UFUNCTION()
    void OnRep_InventoryChanged();

    UPROPERTY(ReplicatedUsing = OnRep_InventoryChanged)
    TArray<FString> Inventory;

public:
    AItemActor* OverlappedItem;

    UPROPERTY(EditDefaultsOnly, Category = "Spawn")
    TSubclassOf<AActor> MeshToSpawn;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    class USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
    class UCameraComponent* Camera;

    UPROPERTY(VisibleAnywhere)
    class UWidgetComponent* InventoryWidget;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UUserWidget> WBP_InventoryClass;

    UPROPERTY(VisibleAnywhere)
    class USphereComponent* InteractionSphere;

};
