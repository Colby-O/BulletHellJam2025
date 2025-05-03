#include "BulletHellJam2025/Player/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BulletHellJam2025/Core/TapHandler.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	TapHandler = new UTapHandler(DashCooldown);

	TapHandler->AddListener("ForwardDash", [this]() {
		Dash(GetActorForwardVector());
	});

	TapHandler->AddListener("BackwardDash", [this]() {
		Dash(-GetActorForwardVector());
	});

	TapHandler->AddListener("RightDash", [this]() {
		Dash(GetActorRightVector());
	});

	TapHandler->AddListener("LEftDash", [this]() {
		Dash(-GetActorRightVector());
	});

	// Setup for movement parameters
	GetCharacterMovement()->MaxWalkSpeed = PlayerSpeed;
	GetCharacterMovement()->JumpZVelocity = JumpForce;
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindKey(EKeys::W, IE_Pressed, this, &APlayerCharacter::OnWPressed);
	PlayerInputComponent->BindKey(EKeys::S, IE_Pressed, this, &APlayerCharacter::OnSPressed);
	PlayerInputComponent->BindKey(EKeys::A, IE_Pressed, this, &APlayerCharacter::OnAPressed);
	PlayerInputComponent->BindKey(EKeys::D, IE_Pressed, this, &APlayerCharacter::OnDPressed);
}

void APlayerCharacter::OnWPressed()
{
	TapHandler->Invoke("ForwardDash", GetWorld());
}

void APlayerCharacter::OnSPressed()
{
	TapHandler->Invoke("BackwardDash", GetWorld());
}

void APlayerCharacter::OnAPressed()
{
	TapHandler->Invoke("LeftDash", GetWorld());
}

void APlayerCharacter::OnDPressed()
{
	TapHandler->Invoke("RightDash", GetWorld());
}

void APlayerCharacter::MoveForward(float Input)
{
	FVector Forward = GetActorForwardVector();
	AddMovementInput(Forward * Input);
}

void APlayerCharacter::MoveRight(float Input)
{
	FVector Right = GetActorRightVector();
	AddMovementInput(Right * Input);
}

void APlayerCharacter::Dash(FVector Direction)
{
	LaunchCharacter(Direction.GetSafeNormal() * DashForce, true, true);
}

