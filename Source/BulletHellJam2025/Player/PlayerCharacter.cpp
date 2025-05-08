#include "BulletHellJam2025/Player/PlayerCharacter.h"
#include "BulletHellJam2025/Core/TapHandler.h"
#include "BulletHellJam2025/Grid/GridManager.h"
#include "BulletHellJam2025/Grid/Tile.h"
#include "BulletHellJam2025/Core/Vector2Int.h"
#include "BulletHellJam2025/Enemies/ShooterComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!ShooterComp) ShooterComp = FindComponentByClass<UShooterComponent>();
	ShooterComp->SetFrom("Player");
	ShooterComp->Disable();

	TapHandler = new UTapHandler(DashCooldown);

	TapHandler->AddListener("ForwardDash", [this]() {
		Dash(Camera->GetForwardVector());
	});

	TapHandler->AddListener("BackwardDash", [this]() {
		Dash(-Camera->GetForwardVector());
	});

	TapHandler->AddListener("RightDash", [this]() {
		Dash(Camera->GetRightVector());
	});

	TapHandler->AddListener("LeftDash", [this]() {
		Dash(-Camera->GetRightVector());
	});

	UCapsuleComponent* Capsule = GetCapsuleComponent();
	PlayerWidth = 2.0 * Capsule->GetScaledCapsuleRadius();
	PlayerHeight = 2.0 * Capsule->GetScaledCapsuleHalfHeight();

	GridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));

	GetCharacterMovement()->MaxWalkSpeed = PlayerSpeed;
	GetCharacterMovement()->JumpZVelocity = JumpForce;
	GetCharacterMovement()->GravityScale = GravityScale;

	Controller = Cast<APlayerController>(GetController());

	SetCursor();

	StartLocation = GetActorLocation();
	HasMoved = false;
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdatePlayerRotation();
	
	if (!HasMoved) HasMoved = !GetActorLocation().Equals(StartLocation, 1.0f);

	LimitSpeed();

	ATile* currentTile = GridManager->GetTileAt(GridManager->WorldToGrid(GetActorLocation()));
	if (!IsDashing && (!currentTile || GetActorLocation().Z < -PlayerHeight)) OnDeath();
	if (EnableTileFall && HasMoved)
	{
		CheckTile(GetActorLocation());
		CheckTile(GetActorLocation() + FVector(PlayerWidth, 0, 0));
		CheckTile(GetActorLocation() - FVector(PlayerWidth, 0, 0));
		CheckTile(GetActorLocation() + FVector(0, PlayerWidth, 0));
		CheckTile(GetActorLocation() - FVector(0, PlayerWidth, 0));
		if (EnableCornerFall) 
		{
			CheckTile(GetActorLocation() - FVector(PlayerWidth, PlayerWidth, 0));
			CheckTile(GetActorLocation() - FVector(-PlayerWidth, PlayerWidth, 0));
			CheckTile(GetActorLocation() - FVector(PlayerWidth, -PlayerWidth, 0));
			CheckTile(GetActorLocation() - FVector(-PlayerWidth, -PlayerWidth, 0));
		}
		if (EnableDebugMode) currentTile->SetColor(FLinearColor::Green);
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindKey(EKeys::W, IE_Pressed, this, &APlayerCharacter::DashForward);
	PlayerInputComponent->BindKey(EKeys::S, IE_Pressed, this, &APlayerCharacter::DashBackward);
	PlayerInputComponent->BindKey(EKeys::A, IE_Pressed, this, &APlayerCharacter::DashLeft);
	PlayerInputComponent->BindKey(EKeys::D, IE_Pressed, this, &APlayerCharacter::DashRight);
	PlayerInputComponent->BindKey(EKeys::LeftShift, IE_Pressed, this, &APlayerCharacter::DashMoveDirection);
	PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &APlayerCharacter::Shoot);
}

void APlayerCharacter::LimitSpeed()
{
	if (!GetCharacterMovement()->IsMovingOnGround() && !IsDashing)
	{
		FVector Velocity = GetVelocity();
		Velocity.X = FMath::Clamp(Velocity.X, -SpeedLimitInAir, SpeedLimitInAir);
		Velocity.Y = FMath::Clamp(Velocity.Y, -SpeedLimitInAir, SpeedLimitInAir);
		GetCharacterMovement()->Velocity = FVector(Velocity.X, Velocity.Y, Velocity.Z);
	}
}

void APlayerCharacter::SetCursor()
{
	if (Controller)
	{
		Controller->bShowMouseCursor = true;
		Controller->bEnableClickEvents = true;
		Controller->bEnableMouseOverEvents = true;
		if (UGameViewportClient* Viewport = GetWorld()->GetGameViewport())
		{
			Viewport->SetMouseLockMode(EMouseLockMode::LockAlways); 
			Viewport->Viewport->CaptureMouse(true);
			Viewport->Viewport->LockMouseToViewport(true);
		}
	}
}

void APlayerCharacter::OnDeath()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

void APlayerCharacter::UpdatePlayerRotation()
{
	if (!Controller) return;

	FVector worldPos, worldDir;

	if (Controller->DeprojectMousePositionToWorld(worldPos, worldDir)) 
	{
		FVector start = worldPos;
		FVector end = worldPos + worldDir * BIG_NUMBER;

		FPlane Plane(FVector(0, 0, PlaneHeight), FVector::UpVector);
		FVector Target = FMath::LinePlaneIntersection(start, end, Plane);

		FVector Direction = (GunMesh->GetComponentLocation() - Target);
		Direction.Z = 0;
		Direction = Direction.GetSafeNormal();

		PlayerMesh->SetRelativeRotation(Direction.Rotation());
	}
}

void APlayerCharacter::CheckTile(FVector pos)
{
	if (GetCharacterMovement()->IsFalling() && !IsDashing) return;
	ATile* tile = GridManager->GetTileAt(GridManager->WorldToGrid(pos));
	if (tile != nullptr) tile->TriggerFall();
}

void APlayerCharacter::OnHit()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

void APlayerCharacter::DashForward()
{
	TapHandler->Invoke("ForwardDash", GetWorld());
}

void APlayerCharacter::DashBackward()
{
	TapHandler->Invoke("BackwardDash", GetWorld());
}

void APlayerCharacter::DashLeft()
{
	TapHandler->Invoke("LeftDash", GetWorld());
}

void APlayerCharacter::DashRight()
{
	TapHandler->Invoke("RightDash", GetWorld());
}

void APlayerCharacter::DashMoveDirection()
{
	Dash(GetLastMovementInputVector());
}

void APlayerCharacter::Shoot()
{
	ShooterComp->Shoot(GetVelocity());
}

bool APlayerCharacter::CanDash()
{
	return !IsDashing && !GetCharacterMovement()->IsFalling();
}

void APlayerCharacter::Dash(FVector Direction)
{
	if (CanDash()) 
	{
		IsDashing = true;
		FVector normal = Direction.GetSafeNormal();
		LaunchCharacter(FVector(normal.X, normal.Y, 0) * DashForce, true, true);
		GetWorldTimerManager().SetTimer(DashTimeHandle, this, &APlayerCharacter::StopDashing, DashCooldown, false);
	}

}

void APlayerCharacter::StopDashing()
{
	IsDashing = false;
}

void APlayerCharacter::MoveForward(float Input)
{
	FVector Forward = Camera->GetForwardVector();
	AddMovementInput(Forward * Input);
}

void APlayerCharacter::MoveRight(float Input)
{
	FVector Right = Camera->GetRightVector();
	AddMovementInput(Right * Input);
}