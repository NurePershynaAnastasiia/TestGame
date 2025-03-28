#include "ItemActor.h"

AItemActor::AItemActor()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;

    ItemName = "DefaultItem";
}
