// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include <Components/CapsuleComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "CombatComponent.h"
#include <Kismet/KismetMathLibrary.h>
#include <Kismet/KismetSystemLibrary.h>
#include "MeshSlicer.h"
#include "PlayerAnimInstance.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// ĸ��
	GetCapsuleComponent()->InitCapsuleSize(40.f, 90.0f);

	// ���̷�Ż �޽�
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> meshAsset(TEXT("SkeletalMesh'/Game/ThirdPerson/Characters/Mannequin_UE4/Meshes/SK_Mannequin.SK_Mannequin'"));
	if (meshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(meshAsset.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -90.f), FRotator(0.f, -90.f, 0.f));
	}

	// Į
	Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Katana"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> katanaAsset(TEXT("StaticMesh'/Game/MarketplaceAssets/ARPG_Samurai/Demo/Weapon/Mesh/katana.katana'"));
	if (katanaAsset.Succeeded())
	{
		Weapon->SetStaticMesh(katanaAsset.Object);
		Weapon->SetupAttachment(GetMesh(), TEXT("katana3"));
		Weapon->SetCollisionProfileName(TEXT("NoCollision"));
	}

	// Į��
	Scabbard = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Scabbard"));
	Scabbard->SetupAttachment(GetMesh(), TEXT("scabbard1"));
	Scabbard->SetCollisionProfileName(TEXT("NoCollision"));

	// �⺻ ��������
	DefaultCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("DefaultCameraBoom"));
	DefaultCameraBoom->SetupAttachment(RootComponent);
	DefaultCameraBoom->TargetArmLength = 300.0f;
	DefaultCameraBoom->bUsePawnControlRotation = true;
	DefaultCameraBoom->bEnableCameraLag = true;	// ī�޶� �� Ȱ��ȭ
	DefaultCameraBoom->CameraLagSpeed = 10.f;

	// ���� ���̵�� ��������
	LeftCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("LeftCameraBoom"));
	LeftCameraBoom->SetupAttachment(RootComponent);
	LeftCameraBoom->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	LeftCameraBoom->TargetArmLength = 225.f;

	// ���� ���̵�� ��������
	RightCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("RightCameraBoom"));
	RightCameraBoom->SetupAttachment(RootComponent);
	RightCameraBoom->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	RightCameraBoom->TargetArmLength = 225.f;

	// ī�޶�
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(DefaultCameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// ȸ�� ����
	TurnRateGamepad = 50.f;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	// �̵� ����
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 400.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 350.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 500.f;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 350.f, 0.f);

	// ���� ������ �����ϴ� ������Ʈ
	CombatComp = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComp"));
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// �⺻ ����, ���� �β� ����
	CombatComp->SetupWeapon(Weapon, 5.0f);

	// �⺻ ���� �ִ� �޺� �� ����
	MaxAttackCount = Attacks.Num() - 1;
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Ÿ������ �� ������ Yaw ȸ��
	if (bIsTargeting == true)
	{
		RotateToTarget(EnemyTarget, DeltaTime, RInterpSpeed);
		// �Ÿ��� �־����� Ÿ�� ����
		if (GetDistanceTo(EnemyTarget) > 800.f)
		{
			EnemyTarget = nullptr;
			bIsTargeting = false;
		}
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &APlayerCharacter::Attack);
	PlayerInputComponent->BindAction("Guard", IE_Pressed, this, &APlayerCharacter::Guard);
	PlayerInputComponent->BindAction("Guard", IE_Released, this, &APlayerCharacter::StopGuard);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerCharacter::Interact);
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &APlayerCharacter::Dash);
	PlayerInputComponent->BindAction("Dash", IE_Released, this, &APlayerCharacter::StopDash);
	PlayerInputComponent->BindAction("Finisher", IE_Pressed, this, &APlayerCharacter::Finisher);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &APlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &APlayerCharacter::LookUpAtRate);
}

void APlayerCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::Jump()
{
	ACharacter::Jump();
}

void APlayerCharacter::Attack()
{
	// TODO : ���� ������ �������� üũ

	// ���� Ÿ�������� Ÿ�� ����
	TryAutoTargeting();

	// �������ϰ� ȸ��
	RInterpSpeed = 5.f;

	// ���� �б�
	if (GetCharacterMovement()->IsFalling() == true)
	{
		PerformJumpAttack();
	}
	else
	{
		PerformLightAttack();
	}
}

void APlayerCharacter::Guard()
{
	// TODO: ���� ������ �������� üũ
	bIsBlocking = true;

	// ABP�� ������Ʈ ����
	auto animIns = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	animIns->bIsBlocking = true;

	// �̵��ӵ� ����
	GetCharacterMovement()->MaxWalkSpeed = 200.f;
	GetCharacterMovement()->MaxAcceleration = 512.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 32.f;
}

void APlayerCharacter::StopGuard()
{
	bIsBlocking = false;

	// ABP�� ������Ʈ ����
	auto animIns = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	animIns->bIsBlocking = false;

	// �̵��ӵ� �ʱ�ȭ
	GetCharacterMovement()->MaxWalkSpeed = 350.f;
	GetCharacterMovement()->MaxAcceleration = 2048.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 500.f;
}

void APlayerCharacter::Interact()
{
	// TODO: ���� �ݱ� ��
}

void APlayerCharacter::Dash()
{
	// TODO: ��� ������ �������� üũ

	bIsDashing = true;

	// 4���� ȸ��
	PerformDodge();

	// �̵��ӵ� ����
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
}

void APlayerCharacter::StopDash()
{
	bIsDashing = false;
	// �̵��ӵ� �ʱ�ȭ
	GetCharacterMovement()->MaxWalkSpeed = 350.f;
}

void APlayerCharacter::Finisher()
{
	if (CanAttack() == false) return;

	// ��� ã��
	// TODO: ü�� ��ġ üũ
	if (TryAutoTargeting() == true)
	{
		MotionMorph();
	}
}

bool APlayerCharacter::CanJump()
{
	// TODO : ���� üũ
	return true;
}

bool APlayerCharacter::CanAttack()
{
	// TODO : ���� üũ
	return true;
}

bool APlayerCharacter::CanGuard()
{
	// TODO : ���� üũ
	return true;
}

bool APlayerCharacter::CanInteract()
{
	// TODO : ���� üũ
	return true;
}

bool APlayerCharacter::CanDash()
{
	// TODO : ���� üũ
	return true;
}

void APlayerCharacter::RotateToTarget(AActor* Target, float DeltaTime, float InterpSpeed)
{
	// Ÿ������ ���� �ִ� ���
	if (Target)
	{
		auto temp = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target->GetActorLocation());

		FRotator newRotation;
		newRotation.Pitch = GetActorRotation().Pitch;
		newRotation.Yaw = FMath::RInterpTo(GetActorRotation(), temp, DeltaTime, InterpSpeed).Yaw;
		newRotation.Roll = GetActorRotation().Roll;
		SetActorRotation(newRotation);
	}
}

float APlayerCharacter::CalculateDirection(const FVector& Velocity, const FRotator& BaseRotation)
{
	// BaseRotation�� �������� Vector���� ���̰��� ��� (-180 ~ +180)
	if (!Velocity.IsNearlyZero())
	{
		FMatrix RotMatrix = FRotationMatrix(BaseRotation);
		FVector ForwardVector = RotMatrix.GetScaledAxis(EAxis::X);
		FVector RightVector = RotMatrix.GetScaledAxis(EAxis::Y);
		FVector NormalizedVel = Velocity.GetSafeNormal2D();

		// get a cos(alpha) of forward vector vs velocity
		float ForwardCosAngle = FVector::DotProduct(ForwardVector, NormalizedVel);
		// now get the alpha and convert to degree
		float ForwardDeltaDegree = FMath::RadiansToDegrees(FMath::Acos(ForwardCosAngle));

		// depending on where right vector is, flip it
		float RightCosAngle = FVector::DotProduct(RightVector, NormalizedVel);
		if (RightCosAngle < 0)
		{
			ForwardDeltaDegree *= -1;
		}

		return ForwardDeltaDegree;
	}

	return 0.f;
}

/* TODO: ��ĥ ����
void APlayerCharacter::PerformLightAttack(int Combo)
{
	// ��Ÿ�� ���
	if (Combo > -1)
	{
		PlayAnimMontage(Attacks[Combo]);
	}
}
*/

void APlayerCharacter::PerformLightAttack()
{
	// ��Ÿ�� ���
	PlayAnimMontage(Attacks[AttackCount]);

	// �޺� ī��Ʈ
	if (AttackCount < MaxAttackCount)
	{
		AttackCount++;
	}
	else
	{
		AttackCount = 0;
	}
}

void APlayerCharacter::PerformJumpAttack()
{
	bIsAttacking = true;

	// ��Ÿ�� ���
	PlayAnimMontage(JumpAttacks[0]);
}

void APlayerCharacter::PerformDodge()
{
	// ȸ�� ���� ���
	FVector inputVector = GetLastMovementInputVector();
	FRotator baseRotation = GetActorRotation();
	float dodgeAngle = CalculateDirection(inputVector, baseRotation);

	if (bIsTargeting == true)
	{
		// ���� ���� ȸ��
		if (dodgeAngle >= -45.0f && dodgeAngle <= 45.f)
		{
			PlayAnimMontage(Dodges[0]); // ������
		}
		else if (dodgeAngle > 45.f && dodgeAngle <= 135.f)
		{
			PlayAnimMontage(Dodges[1]); // ����������
		}
		else if (dodgeAngle > -135.f && dodgeAngle < -45.f)
		{
			PlayAnimMontage(Dodges[2]); // ��������
		}
		else // (dodgeAngle 135.f > && dodgeAngle < -135.f)
		{
			PlayAnimMontage(Dodges[3]); // �ڷ�
		}
	}
	else // ������ ���� ȸ��
	{
		FRotator newRotation = FRotationMatrix::MakeFromX(GetLastMovementInputVector()).Rotator();
		SetActorRotation(newRotation);	// ���� ��ȯ
		PlayAnimMontage(Dodges[0]);		// ������
	}
}

void APlayerCharacter::FinishEnemy()
{
	if (CanAttack() == false) return;

	PlayAnimMontage(Finishers[0]);
}

void APlayerCharacter::MoveCamera(ECameraPosition CameraPosition)
{
	if (CameraPosition == ECameraPosition::ECP_Default)
	{
		FollowCamera->AttachToComponent(DefaultCameraBoom, FAttachmentTransformRules::KeepWorldTransform);
	}
	else if (CameraPosition == ECameraPosition::ECP_LeftSideView)
	{
		FollowCamera->AttachToComponent(LeftCameraBoom, FAttachmentTransformRules::KeepWorldTransform);
	}
	else if (CameraPosition == ECameraPosition::ECP_RightSideView)
	{
		FollowCamera->AttachToComponent(RightCameraBoom, FAttachmentTransformRules::KeepWorldTransform);
	}

	FLatentActionInfo info;
	info.CallbackTarget = this;
	UKismetSystemLibrary::MoveComponentTo(FollowCamera, FVector(0.f), FRotator(0.f), false, false, 0.4f, true, EMoveComponentAction::Move, info);
}

void APlayerCharacter::SpawnMeshSlicer()
{
	FActorSpawnParameters spawnParams;
	FTransform spawnTransform = Weapon->GetComponentTransform();
	GetWorld()->SpawnActor<AMeshSlicer>(AMeshSlicer::StaticClass(), spawnTransform, spawnParams);
}

bool APlayerCharacter::TryAutoTargeting()
{
	// �Ÿ� + ���� ���ڰ� ���� ����� Pawn�� EnemyTarget���� ����
	if (SearchEnemies() == true)
	{
		ScoreEnemies();
		SetEnemyTarget();
		ClearScores();
		bIsTargeting = true;
		return true;
	}

	// ã�� ���� ������ false
	bIsTargeting = false;
	return false;
}

bool APlayerCharacter::SearchEnemies()
{
	// "Targeting" ä�η� Ʈ���̽�
	TArray<AActor*> actorToIgnore;
	bool outValue = UKismetSystemLibrary::SphereTraceMulti(this, GetActorLocation(), GetActorLocation(), 300.f, TraceTypeQuery3, false, actorToIgnore, 
		EDrawDebugTrace::ForDuration, SearchHits, true, FColor::Red, FColor::Green, 1.f);

	// ���� �� ���� ��ã���� false ��ȯ
	if (outValue == false)
		return false;
	
	// ���� �� ��� ���� ã�� �迭�� �߰��ϰ� true ��ȯ
	for (int i = 0; i < SearchHits.Num(); i++)
	{
		SearchedEnemies.Add(SearchHits[i].GetActor());
	}
	return true;
}

void APlayerCharacter::ScoreEnemies()
{
	for (int i = 0; i < SearchHits.Num(); i++)
	{
		// �Ÿ� ���� ����ϱ�
		float dist = FVector::Distance(SearchedEnemies[i]->GetActorLocation(), GetActorLocation());
		DistanceScores.Add(dist);

		// ���� ���� ����ϱ�
		FVector inputVector = GetLastMovementInputVector();
		FVector direction = SearchedEnemies[i]->GetActorLocation() - GetActorLocation();
		direction.Normalize();
		float deltaRadian = FMath::Acos(FVector::DotProduct(inputVector, direction));
		float deltaDegree = FMath::RadiansToDegrees(deltaRadian);
		AngleScores.Add(deltaDegree);

		// �հ� ���� ����ϱ�
		TotalScores.Add(DistanceScores[i] + AngleScores[i] * 4);

		// �����
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString::Printf(TEXT("Name:%s,    Dist:%f,    Angle:%f,    Total:%f"), 
			*SearchHits[i].GetActor()->GetName(), DistanceScores[i], AngleScores[i], TotalScores[i]));
	}
}

void APlayerCharacter::SetEnemyTarget()
{
	// ������ ã��
	float minScore = 5000.f;
	int index = 0;
	int outIndex = 0;
	for (index; index < SearchHits.Num(); index++)
	{
		if (TotalScores[index] < minScore)
		{
			minScore = TotalScores[index];
			outIndex = index;
		}
	}

	// ������ ���� ���ʹ̸� Ÿ������ ����
	EnemyTarget = SearchedEnemies[outIndex];

	// �����
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("%s"), *EnemyTarget->GetName()));
}

void APlayerCharacter::ClearScores()
{
	// ����� �迭 �ʱ�ȭ
	SearchHits.Empty();
	SearchedEnemies.Empty();
	DistanceScores.Empty();
	AngleScores.Empty();
	TotalScores.Empty();
}
