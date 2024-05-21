// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Ball.h"
#include "TreasureChest.h"
#include "TagGameCharacter.h"
#include "EnemyAIController.generated.h"


struct FAIVState : public TSharedFromThis<FAIVState>
{
private:
	TFunction<void(AAIController*)> Enter;
	TFunction<void(AAIController*)> Exit;
	TFunction<TSharedPtr<FAIVState>(AAIController*,const float)> Tick;
public:
	FAIVState()
	{
		Enter = nullptr;
		Exit = nullptr;
		Tick = nullptr;
	}

	FAIVState(TFunction<void(AAIController*)> InEnter = nullptr,
			  TFunction<void(AAIController*)> InExit = nullptr,
			  TFunction<TSharedPtr<FAIVState>(AAIController*, const float)> InTick = nullptr)
	{
		Enter = InEnter;
		Exit = InExit;
		Tick = InTick;
	}

	FAIVState(const FAIVState& Other) = delete;
	FAIVState& operator=(const FAIVState& Other) = delete;
	FAIVState(FAIVState&& Other) = delete;
	FAIVState& operator=(FAIVState&& Other) = delete;

	void CallEnter(AAIController* AIController)
	{
		if (Enter)
		{
			Enter(AIController);
		}
	}
	void CallExit(AAIController* AIController)
	{
		if (Exit)
		{
			Exit(AIController);
		}
	}
	TSharedPtr<FAIVState> CallTick(AAIController* AIController, const float DeltaTime)
	{
		if (Tick)
		{
			TSharedPtr<FAIVState> NewState = Tick(AIController, DeltaTime);

			if (NewState != nullptr && NewState != AsShared())
			{
				CallExit(AIController);
				NewState->CallEnter(AIController);
				return NewState;
			}
		}

		return AsShared();
	}
};

/**
 * 
 */
UCLASS()
class TAGGAME_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
protected:
	TSharedPtr<FAIVState> CurrentState;
	TSharedPtr<FAIVState> Fight;
	TSharedPtr<FAIVState> GetStunned;
	TSharedPtr<FAIVState> SearchForKeys;
	TSharedPtr<FAIVState> GoToKeys;
	TSharedPtr<FAIVState> GrabKeys;
	TSharedPtr<FAIVState> GoToChest;
	TSharedPtr<FAIVState> StunCoolDown;
	TSharedPtr<FAIVState> Wait;


	void BeginPlay() override;
	void Tick(float DeltaTime) override;
	ABall* BestKey;
	ATreasureChest* Chest;
	UPROPERTY(BlueprintReadOnly)
	AActor* Adversary;
	float TimeToWait = 1.5f;
	float StunCooldown = 3.f;
	float CurrentTimer = 0.f;
};
