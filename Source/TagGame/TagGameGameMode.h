// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/TargetPoint.h"
#include "Ball.h"
#include "TreasureChest.h"
#include "TagGameGameMode.generated.h"

UCLASS(minimalapi)
class ATagGameGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATagGameGameMode();
	void BeginPlay() override;
	void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable,Category = "TagGameGameMode")
	void ResetMatch();
	const TArray<class ABall*>& GetKeys() const;
	const TArray<class ACharacter*>& GetCharacters() const;
	ATreasureChest* GetChest() const;
protected:
	TArray<ATargetPoint*> TargetPoints;
	TArray<ABall*> Keys;
	TArray<ACharacter*> Characters;
	ATreasureChest* Chest;


};



