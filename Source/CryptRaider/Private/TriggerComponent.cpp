// Fill out your copyright notice in the Description page of Project Settings.


#include "TriggerComponent.h"

// Sets default values for this component's properties
UTriggerComponent::UTriggerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTriggerComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UTriggerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!Mover)
	{
		//UE_LOG(LogTemp, Display, TEXT("Mover was null!"));
		return;
	}

	AActor* Actor = GetAcceptableActor();

	if (Actor)
	{
		UE_LOG(LogTemp, Display, TEXT("Unlocking!"));

		UPrimitiveComponent* ActorPrimitive = Cast<UPrimitiveComponent>(Actor->GetRootComponent());

		if (ActorPrimitive)
		{
			ActorPrimitive->SetSimulatePhysics(false);
		}
		
		Actor->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);
		Mover->SetShouldMove(true);
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Locking!"));
		Mover->SetShouldMove(false);
	}
}

void UTriggerComponent::SetMover(UMoveComponent* NewMover) 
{
	UE_LOG(LogTemp, Display, TEXT("Set mover!"));
	Mover = NewMover;
}

AActor* UTriggerComponent::GetAcceptableActor() const 
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors) 
	{
		if (Actor->ActorHasTag(AcceptableTag) && !Actor->ActorHasTag("Grabbed"))
		{
			return Actor;
		}
	}

	return nullptr;
}