// Copyright Epic Games, Inc. All Rights Reserved.

#include "TagGameGameMode.h"
#include "TagGameCharacter.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ATagGameGameMode::ATagGameGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ATagGameGameMode::BeginPlay()
{
	Super::BeginPlay();
	TargetPoints.Empty();
	Keys.Empty();
	for (TActorIterator<ATagGameCharacter> It(GetWorld()); It; ++It)
	{
		Adversaries.Add(*It);

	}
	for (TActorIterator<ATreasureChest> It(GetWorld()); It; ++It)
	{
		Chest = (*It);

	}
	
	ResetMatch();
}

void ATagGameGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (int32 Index = 0; Index < Keys.Num(); Index++)
	{
		if (Keys[Index]->GetAttachParentActor() != GetWorld()->GetFirstPlayerController()->GetPawn())
		{
			return;
		}
	}

	ResetMatch();
}

void ATagGameGameMode::ResetMatch()
{
	for (TActorIterator<ATargetPoint> It(GetWorld()); It; ++It)
	{
		TargetPoints.Add(*It);

	}
	for (TActorIterator<ABall> It(GetWorld()); It; ++It)
	{

		if (It->GetAttachParentActor())
		{
			It->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		}
		Keys.Add(*It);
		It->SetActorHiddenInGame(false);

	}

	TArray<ATargetPoint*> RandomTargetPoints = TargetPoints;

	for (int32 Index = 0; Index < Keys.Num(); Index++)
	{
		const int32 RandomTargetIndex = FMath::RandRange(0, RandomTargetPoints.Num() - 1);
		Keys[Index]->SetActorLocation(RandomTargetPoints[RandomTargetIndex]->GetActorLocation());
		RandomTargetPoints.RemoveAt(RandomTargetIndex);
	}
	
}

const TArray<ABall*>& ATagGameGameMode::GetKeys() const
{
	return Keys;
}

ATreasureChest* ATagGameGameMode::GetChest() const
{
	return Chest;
}

const TArray<class ATagGameCharacter*>& ATagGameGameMode::GetAdversaries() const
{
	return Adversaries;
}

