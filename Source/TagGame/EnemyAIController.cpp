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
			UE_LOG(LogTemp, Error, TEXT("FOUND KEY"));
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

			AIController->MoveToActor(BestKey, 10.f);
			UE_LOG(LogTemp, Error, TEXT("GOING TO THE KEY"));
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

			if (BestKey->GetAttachParentActor())
			{
				return Fight;
			}
				BestKey->AttachToActor(AIController->GetPawn(), FAttachmentTransformRules::KeepRelativeTransform);
				BestKey->SetActorRelativeLocation(FVector(0,0,150));
				UE_LOG(LogTemp, Error, TEXT("GRABBED KEY"));
				return GoToChest;

		});

	GoToChest = MakeShared<FAIVState>(
		[this](AAIController* AIController) {

			AGameModeBase* GameModeToSearch = AIController->GetWorld()->GetAuthGameMode();
			ATagGameGameMode* AIGameMode = Cast<ATagGameGameMode>(GameModeToSearch);
			Chest = AIGameMode->GetChest();
			AIController->MoveToActor(Chest, 100.f);
			UE_LOG(LogTemp, Error, TEXT("CHEST"));
			
		},
		[this](AAIController* AIController) {

			BestKey->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
			BestKey->AttachToActor(Chest, FAttachmentTransformRules::KeepRelativeTransform);
			BestKey->SetActorHiddenInGame(true);
			

		},
		[this](AAIController* AIController, const float DeltaTime) -> TSharedPtr<FAIVState> {

			EPathFollowingStatus::Type State = AIController->GetMoveStatus();

			if (BestKey->GetAttachParentActor() != AIController->GetPawn())
			{
				return Fight;

			}else if (State == EPathFollowingStatus::Moving)
			{
				return nullptr;
			}
			
			return Wait;
		});

	Fight = MakeShared<FAIVState>(
		[this](AAIController* AIController) {

			if (BestKey->GetAttachParentActor())
			{
				Adversary = BestKey->GetAttachParentActor();
				AIController->MoveToActor(Adversary, 10.f);
			}

		},
		nullptr,
		[this](AAIController* AIController, const float DeltaTime) -> TSharedPtr<FAIVState> {

			if(!BestKey->GetAttachParentActor())
			{
				return SearchForKeys;
			}
			
			if (AIController->IsOverlappingActor(Adversary) && BestKey->GetAttachParentActor() == Adversary)
			{
				
				BestKey->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
				BestKey->SetActorRelativeLocation(FVector(200, 0, 20));
				return SearchForKeys;
			}

			return nullptr;
		});

	Wait = MakeShared<FAIVState>(
		[this](AAIController* AIController) {

			CurrentTimer = TimeToWait;
			UE_LOG(LogTemp, Error, TEXT("WAITING"));

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
