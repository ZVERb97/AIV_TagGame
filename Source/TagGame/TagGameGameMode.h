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
	void ResetMatch();
	const TArray<class ABall*>& GetKeys() const;
	ATreasureChest* GetChest() const;
	const TArray<class ATagGameCharacter*>& GetAdversaries() const;
protected:
	TArray<ATargetPoint*> TargetPoints;
	TArray<ABall*> Keys;
	ATreasureChest* Chest;
	TArray<ATagGameCharacter*> Adversaries;


};



