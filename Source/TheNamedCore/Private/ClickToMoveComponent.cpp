// ClickToMoveComponent.cpp

#include "ClickToMoveComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"
#include "NavigationData.h"
#include "NavigationPath.h"

UClickToMoveComponent::UClickToMoveComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UClickToMoveComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningCharacter = Cast<ACharacter>(GetOwner());
	if (OwningCharacter)
	{
		MovementComponent = OwningCharacter->GetCharacterMovement();
	}

	// This component is meaningless without a character + movement
	// component. Fail loudly in dev builds rather than silently no-op.
	ensureMsgf(OwningCharacter && MovementComponent,
		TEXT("UClickToMoveComponent requires its owner to be an ACharacter with a valid CharacterMovementComponent."));
}

void UClickToMoveComponent::HandleClickAtWorldLocation(const FVector& ClickWorldLocation)
{
	if (!OwningCharacter)
	{
		return;
	}

	// Reject clicks too close to current position -- avoids jitter from
	// accidental re-clicks on (or near) the hero's own location.
	const float DistanceToClick = FVector::Dist(OwningCharacter->GetActorLocation(), ClickWorldLocation);
	if (DistanceToClick < MinimumClickDistance)
	{
		return;
	}

	if (bHasActiveMove && !bNewClickInterruptsCurrentMove)
	{
		// Design choice: if we ever want queued movement, this is the
		// branch to build it in. Left as a no-op for now.
		return;
	}

	RequestMoveToLocation(ClickWorldLocation);
}

void UClickToMoveComponent::RequestMoveToLocation(const FVector& Destination)
{
	if (BuildPathTo(Destination))
	{
		CurrentPathIndex = 0;
		bHasActiveMove = true;
	}
}

void UClickToMoveComponent::CancelMovement()
{
	bHasActiveMove = false;
	CurrentPath.Reset();
	CurrentPathIndex = 0;

	if (MovementComponent)
	{
		MovementComponent->StopMovementImmediately();
	}
}

bool UClickToMoveComponent::IsMoving() const
{
	return bHasActiveMove;
}

bool UClickToMoveComponent::BuildPathTo(const FVector& Destination)
{
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSystem || !OwningCharacter)
	{
		return false;
	}

	// Project the raw destination onto the nav mesh in case the click
	// landed slightly off it (uneven ground, click on a small prop, etc).
	FNavLocation ProjectedDestination;
	const bool bProjected = NavSystem->ProjectPointToNavigation(
		Destination,
		ProjectedDestination,
		FVector(NavMeshProjectionExtent, NavMeshProjectionExtent, NavMeshProjectionExtent));

	if (!bProjected)
	{
		// Nothing reachable near the click. Don't silently fail forever --
		// this is a good place to hook a "can't go there" UI/audio cue later.
		return false;
	}

	UNavigationPath* NavPath = NavSystem->FindPathToLocationSynchronously(
		GetWorld(),
		OwningCharacter->GetActorLocation(),
		ProjectedDestination.Location,
		OwningCharacter);

	if (!NavPath || !NavPath->IsValid() || NavPath->PathPoints.Num() < 2)
	{
		return false;
	}

	CurrentPath = NavPath->PathPoints;
	return true;
}

void UClickToMoveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bHasActiveMove)
	{
		TickPathFollowing(DeltaTime);
	}
}

void UClickToMoveComponent::TickPathFollowing(float DeltaTime)
{
	if (!OwningCharacter || !MovementComponent || !CurrentPath.IsValidIndex(CurrentPathIndex))
	{
		CancelMovement();
		return;
	}

	const FVector CurrentLocation = OwningCharacter->GetActorLocation();
	const FVector TargetPoint = CurrentPath[CurrentPathIndex];

	const bool bIsFinalPoint = (CurrentPathIndex == CurrentPath.Num() - 1);
	const float AcceptanceRadius = bIsFinalPoint ? FinalDestinationAcceptanceRadius : PathPointAcceptanceRadius;

	const FVector ToTarget = TargetPoint - CurrentLocation;
	const float DistanceToTarget = ToTarget.Size2D(); // MOBA-style: ignore Z for arrival checks

	if (DistanceToTarget <= AcceptanceRadius)
	{
		if (bIsFinalPoint)
		{
			CancelMovement();
			return;
		}

		++CurrentPathIndex;
		return; // Re-evaluate against the new point next tick
	}

	const FVector MoveDirection = ToTarget.GetSafeNormal2D();

	// Feed the movement component the same way any input source would --
	// this keeps us compatible with the existing CharacterMovementComponent
	// acceleration/friction/speed settings rather than teleporting the
	// character along the path.
	OwningCharacter->AddMovementInput(MoveDirection, 1.0f);

	ApplyRotation(MoveDirection, DeltaTime);
}

void UClickToMoveComponent::ApplyRotation(const FVector& MoveDirection, float DeltaTime)
{
	if (MoveDirection.IsNearlyZero())
	{
		return;
	}

	const FRotator CurrentRotation = OwningCharacter->GetActorRotation();
	const FRotator TargetRotation = MoveDirection.Rotation();

	const FRotator NewRotation = FMath::RInterpConstantTo(
		CurrentRotation,
		TargetRotation,
		DeltaTime,
		RotationInterpSpeed);

	OwningCharacter->SetActorRotation(NewRotation);
}