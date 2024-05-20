// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"
#include "TagGameGameMode.h"



void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	SearchForKeys = MakeShared<FAIVState>(
		[this](AAIController* AIController) {

			AGameModeBase* GameModeToSearch = AIController->GetWorld()->GetAuthGameMode();
			ATagGameGameMode* AIGameMode = Cast<ATagGameGameMode>(GameModeToSearch);
			const TArray<ABall*>& KeysList = AIGameMode->GetKeys();

			ABall* NearestKey = nullptr;

			for (int32 Index = 0; Index < KeysList.Num(); Index++)
			{
				if (!KeysList[Index]->GetAttachParentActor() &&
					(!NearestKey || 
					FVector::Distance(AIController->GetPawn()->GetActorLocation(), KeysList[Index]->GetActorLocation()) <
					FVector::Distance(AIController->GetPawn()->GetActorLocation(), NearestKey->GetActorLocation())))
				{
					NearestKey = KeysList[Index];
				}
			}

			BestKey = NearestKey;
			UE_LOG(LogTemp, Warning, TEXT("Searching for Ball"));
		},
		nullptr,
		[this](AAIController* AIController, const float DeltaTime) -> TSharedPtr<FAIVState> {

			if (BestKey)
			{
				return GoToKeys;
			}
			else
			{
				return SearchForKeys;
			}
		}
		);

	GoToKeys = MakeShared<FAIVState>(
		[this](AAIController* AIController) {

			AIController->MoveToActor(BestKey, 100.f);
			UE_LOG(LogTemp, Warning, TEXT("Going to the Key"));
		},
		nullptr,
		[this](AAIController* AIController, const float DeltaTime) -> TSharedPtr<FAIVState> {

			EPathFollowingStatus::Type State = AIController->GetMoveStatus();
			if (State == EPathFollowingStatus::Moving)
			{
				return nullptr;
			}
			return GrabKeys;
		});

	GrabKeys = MakeShared<FAIVState>(
		nullptr,
		nullptr,
		[this](AAIController* AIController, const float DeltaTime) -> TSharedPtr<FAIVState> {

			EPathFollowingStatus::Type State = AIController->GetMoveStatus();

			if (!BestKey)
			{
				return Fight;
			}
				BestKey->AttachToActor(AIController->GetPawn(), FAttachmentTransformRules::KeepRelativeTransform);
				BestKey->SetActorRelativeLocation(FVector(0,0,150));
				return GoToChest;

		});

	GoToChest = MakeShared<FAIVState>(
		[this](AAIController* AIController) {

			AGameModeBase* GameModeToSearch = AIController->GetWorld()->GetAuthGameMode();
			ATagGameGameMode* AIGameMode = Cast<ATagGameGameMode>(GameModeToSearch);
			Chest = AIGameMode->GetChest();
			AIController->MoveToActor(Chest, 100.f);
			UE_LOG(LogTemp, Warning, TEXT("Going to the chest"));
			
		},
		[this](AAIController* AIController) {

			BestKey->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
			BestKey->SetActorHiddenInGame(true);

		},
		[this](AAIController* AIController, const float DeltaTime) -> TSharedPtr<FAIVState> {

			EPathFollowingStatus::Type State = AIController->GetMoveStatus();
			if (State == EPathFollowingStatus::Moving)
			{
				return nullptr;
			}
			if (!BestKey)
			{
				return Fight;
			}
			
			return Wait;
		});

	Fight = MakeShared<FAIVState>(
		[this](AAIController* AIController) {

			//AIController->MoveToActor(AIController->GetWorld()->GetFirstPlayerController()->GetPawn(), 100.f);
			AGameModeBase* GameModeToSearch = AIController->GetWorld()->GetAuthGameMode();
			ATagGameGameMode* AIGameMode = Cast<ATagGameGameMode>(GameModeToSearch);
			const TArray<ABall*>& KeysList = AIGameMode->GetKeys();
			for (int32 Index = 0; Index < KeysList.Num(); Index++)
			{
				if (KeysList[Index]->GetAttachParentActor())
				{
					Adversary = KeysList[Index]->GetAttachParentActor();
					AIController->MoveToActor(Adversary,100.f);
				}
			}

		},
		nullptr,
		[this](AAIController* AIController, const float DeltaTime) -> TSharedPtr<FAIVState> {

			EPathFollowingStatus::Type State = AIController->GetMoveStatus();
			if (State == EPathFollowingStatus::Moving)
			{
				return nullptr;
			}
			
			if(Adversary)
			{
				UE_LOG(LogTemp, Error, TEXT("FIGHTING"));
			}

			return SearchForKeys;
		});

	Wait = MakeShared<FAIVState>(
		[this](AAIController* AIController) {

			CurrentTimer = TimeToWait;

		},
		nullptr,
		[this](AAIController* AIController, const float DeltaTime) -> TSharedPtr<FAIVState> {

			CurrentTimer -= DeltaTime;
			if (CurrentTimer <= 0)
			{
				return SearchForKeys;
			}

			return nullptr;
		});
	

	CurrentState = SearchForKeys;
	CurrentState->CallEnter(this);
}

void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (CurrentState)
	{
		CurrentState = CurrentState->CallTick(this, DeltaTime);
	}
}