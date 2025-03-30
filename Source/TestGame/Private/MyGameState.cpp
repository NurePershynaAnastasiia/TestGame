#include "MyGameState.h"
#include "Net/UnrealNetwork.h"

AMyGameState::AMyGameState()
{
    GlobalReplicationQuality = 50;
    bReplicates = true;
}

void AMyGameState::Server_SetReplicationQuality_Implementation(int32 NewQuality)
{
    GlobalReplicationQuality = NewQuality;
}

void AMyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AMyGameState, GlobalReplicationQuality);
}