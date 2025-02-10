// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabber.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

const FName UGrabber::GrabbedTag = "Grabbed";

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();
	
	if (!PhysicsHandle || !PhysicsHandle->GetGrabbedComponent())
	{
		return;
	}

	FVector TargetLocation = GetComponentLocation() + GetForwardVector() * HoldDistance;
	PhysicsHandle->SetTargetLocationAndRotation(TargetLocation, GetComponentRotation());
}

void UGrabber::Grab()
{
	UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();

	if (!PhysicsHandle)
	{
		return;
	}

	FHitResult HitResult;
	if (!TryGetGrabbableInReach(HitResult))
	{
		return;
	}

	UPrimitiveComponent* Grabbable = HitResult.GetComponent();
	AActor* GrabbableOwner = Grabbable->GetOwner();

	if (!GrabbableOwner->Tags.Contains(GrabbedTag))
	{
		GrabbableOwner->Tags.Add(GrabbedTag);
	}

	UPrimitiveComponent* ActorPrimitive = Cast<UPrimitiveComponent>(GrabbableOwner->GetRootComponent());

	if (ActorPrimitive)
	{
		ActorPrimitive->SetSimulatePhysics(true);
	}

	GrabbableOwner->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	
	Grabbable->WakeAllRigidBodies();

	PhysicsHandle->GrabComponentAtLocationWithRotation(
		Grabbable,
		NAME_None,
		HitResult.ImpactPoint,
		GetComponentRotation()
	);

	UE_LOG(LogTemp, Display, TEXT("Grabber grabbed!"));
}

void UGrabber::Release() 
{
	UE_LOG(LogTemp, Display, TEXT("Grabber released!"));

	auto* PhysicsHandle = GetPhysicsHandle();

	if (!PhysicsHandle || !PhysicsHandle->GetGrabbedComponent())
	{
		return;
	}

	UPrimitiveComponent* GrabbedComponent = PhysicsHandle->GetGrabbedComponent();
	AActor* GrabbedOwner = GrabbedComponent->GetOwner();

	if (GrabbedOwner->Tags.Contains(GrabbedTag))
	{
		GrabbedOwner->Tags.Remove(GrabbedTag);
	}

	GrabbedComponent->WakeAllRigidBodies();
	PhysicsHandle->ReleaseComponent();
}

UPhysicsHandleComponent* UGrabber::GetPhysicsHandle() const 
{
	UPhysicsHandleComponent* PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();

	if (!PhysicsHandle)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find a PhysicsHandle component on %s!"), *GetName());
	}

	return PhysicsHandle;
}

bool UGrabber::TryGetGrabbableInReach(FHitResult& OutHitResult) const 
{
	FVector Start = GetComponentLocation();
	FVector End = Start + GetForwardVector() * MaxGrabDistance;
	DrawDebugLine(GetWorld(), Start, End, FColor::Red);

	constexpr ECollisionChannel GrabberChannel = ECC_GameTraceChannel2;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(GrabRadius);

	return GetWorld()->SweepSingleByChannel(
		OutHitResult,
		Start, End,
		FQuat::Identity,
		GrabberChannel,
		Sphere);
}