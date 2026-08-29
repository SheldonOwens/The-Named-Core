// NamedPlayerController.cpp

#include "NamedPlayerController.h"
#include "NamedHeroBase.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

ANamedPlayerController::ANamedPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void ANamedPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
			else
			{
				// Loud on purpose -- a controller with no mapping context
				// silently does nothing when clicked, which looks exactly
				// like a bug in RequestMoveToLocation rather than a
				// missing editor setup step. Save future-us the confusion.
				UE_LOG(LogTemp, Warning,
					TEXT("ANamedPlayerController: DefaultMappingContext is not assigned. Click-to-move will not function until an Input Mapping Context is set on this controller's defaults."));
			}
		}
	}
}

void ANamedPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (ClickMoveAction)
		{
			EnhancedInputComponent->BindAction(ClickMoveAction, ETriggerEvent::Started, this, &ANamedPlayerController::HandleClickMove);
		}
		else
		{
			UE_LOG(LogTemp, Warning,
				TEXT("ANamedPlayerController: ClickMoveAction is not assigned. Click-to-move will not function until an Input Action is set on this controller's defaults."));
		}
	}
}

void ANamedPlayerController::HandleClickMove(const FInputActionValue& Value)
{
	FHitResult HitResult;
	const bool bHit = GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

	if (!bHit)
	{
		return;
	}

	if (ANamedHeroBase* Hero = Cast<ANamedHeroBase>(GetPawn()))
	{
		Hero->RequestMoveToLocation(HitResult.Location);
	}
}
