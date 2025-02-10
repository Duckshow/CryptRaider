// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveComponent.h"
#include "Math/UnrealMathUtility.h"

// Sets default values for this component's properties
UMoveComponent::UMoveComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMoveComponent::BeginPlay()
{
	Super::BeginPlay();

	OriginalLocation = GetOwner()->GetActorLocation();
}


// Called every frame
void UMoveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector CurrentLocation = GetOwner()->GetActorLocation();
	FVector TargetLocation = ShouldMove ? OriginalLocation + MoveOffset : OriginalLocation;

	float Speed = MoveOffset.Length() / MoveTime;

	if (ShouldMove)
	{
		TargetLocation = OriginalLocation + MoveOffset;
		Speed = FVector::Distance(OriginalLocation, TargetLocation) / MoveTime;
	}
	else
	{
		TargetLocation = OriginalLocation;
		Speed = FVector::Distance(OriginalLocation + MoveOffset, TargetLocation) / MoveTime;
	}

	FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, DeltaTime, Speed);

	if (Debug)
	{
		UE_LOG(LogTemp, Display, TEXT("ShouldMove: %s, Current Location: %s, Original Location: %s"), ShouldMove ? TEXT("true") : TEXT("false"), *CurrentLocation.ToString(), *OriginalLocation.ToString());
	}
	
	GetOwner()->SetActorLocation(NewLocation);
}

void UMoveComponent::SetShouldMove(bool Value) 
{
	ShouldMove = Value;
}