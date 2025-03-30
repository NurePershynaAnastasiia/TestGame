#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MyGameState.generated.h"

UCLASS()
class TESTGAME_API AMyGameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    AMyGameState();

    UPROPERTY(Replicated, BlueprintReadWrite, Category = "Replication")
    int32 GlobalReplicationQuality;

    UFUNCTION(Server, Reliable)
    void Server_SetReplicationQuality(int32 NewQuality);

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};

