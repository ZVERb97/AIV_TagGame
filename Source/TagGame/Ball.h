// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ball.generated.h"

UCLASS()
class TAGGAME_API ABall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* StaticMeshComponent;
	ABall();
	bool GetIsKeyTaken();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	bool bIsKeyTaken;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
