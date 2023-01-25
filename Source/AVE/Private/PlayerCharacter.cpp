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
#include "GrabbableActorBase.h"

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
	Weapon->SetupAttachment(GetMesh(), TEXT("katana3"));
	Weapon->SetCollisionProfileName(TEXT("NoCollision"));

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
	DefaultCameraBoom->CameraLagSpeed = 4.f;

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
	MoveComp = GetCharacterMovement();
	MoveComp->bOrientRotationToMovement = true;
	MoveComp->RotationRate = FRotator(0.0f, 350.0f, 0.0f);
	MoveComp->JumpZVelocity = 400.f;
	MoveComp->AirControl = 0.35f;
	MoveComp->MaxWalkSpeed = 300.f;
	MoveComp->MinAnalogWalkSpeed = 20.f;
	MoveComp->BrakingDecelerationWalking = 500.f;

	// ���� ������ �����ϴ� ������Ʈ
	CombatComp = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComp"));
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// �Ĺ� ������Ʈ�� ���� ����
	CombatComp->SetupWeapon(Weapon);
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Ÿ������ ���� ���� ȸ��
	if (bIsTargeting == true && EnemyTarget)
	{
		FVector direction = EnemyTarget->GetActorLocation() - GetActorLocation();
		RotateToDirection(direction, DeltaTime, RInterpSpeed);
		// �Ÿ��� �־����� Ÿ���� ����
		if (GetDistanceTo(EnemyTarget) > 800.f)
		{
			bIsTargeting = false;
			EnemyTarget = nullptr;
		}
	}
	// ����Ű �������� ȸ��
	else if (bIsAttacking == true)
	{
		FVector inputVector = GetLastMovementInputVector();
		if (inputVector.IsNearlyZero() == false)
		{
			RotateToDirection(inputVector, DeltaTime, 4.f);
		}
	}

	LastMoveTime += DeltaTime;
	LastAttackTime += DeltaTime;
	// ������ ����Ű ��ǲ�� 0.4�ʸ� ��������
	if (LastMoveTime > MoveResetLimit) {
		LastMoveTime = 0.f;
		Tail = -1;
	}
	// ���� ����� ������ 0.6�ʰ� ������
	if (!bIsAttacking && LastAttackTime > ComboResetLimit) {
		// �޺��� �ʱ�ȭ
		Combo = -1;
		LastAttackTime = 0.f;
	}

}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);
	// ������ Ŀ�ǵ� ������ ���� WASD ������ �̺�Ʈ ���ε�
	PlayerInputComponent->BindAction("WInput", IE_Pressed, this, &APlayerCharacter::WInput);
	PlayerInputComponent->BindAction("SInput", IE_Pressed, this, &APlayerCharacter::SInput);
	PlayerInputComponent->BindAction("DInput", IE_Pressed, this, &APlayerCharacter::DInput);
	PlayerInputComponent->BindAction("AInput", IE_Pressed, this, &APlayerCharacter::AInput);

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
		const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);

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
		const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);

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

void APlayerCharacter::Guard()
{
	// TODO: ���� ������ �������� üũ
	bIsBlocking = true;
	
	// 0.3�� ���� �и� ���� �ߵ� 
	bIsParrying = true;
	GetWorldTimerManager().SetTimer(ParryingTimer, this, &APlayerCharacter::OnParryEnd, 0.3f, false);

	// ABP�� ������Ʈ ����
	auto animIns = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	animIns->bIsBlocking = true;

	// �̵��ӵ� ����
	MoveComp->MaxWalkSpeed = 150.f;
	MoveComp->MaxAcceleration = 512.f;
	MoveComp->BrakingDecelerationWalking = 32.f;
}

void APlayerCharacter::StopGuard()
{
	bIsBlocking = false;

	// �и� ���� ����
	bIsParrying = false;
	GetWorldTimerManager().ClearTimer(ParryingTimer);

	// ABP�� ������Ʈ ����
	auto animIns = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	animIns->bIsBlocking = false;

	// �̵��ӵ� �ʱ�ȭ
	MoveComp->MaxWalkSpeed = 300.f;
	MoveComp->MaxAcceleration = 2048.f;
	MoveComp->BrakingDecelerationWalking = 500.f;
}

void APlayerCharacter::Interact()
{
	if (CanInteract() == false) return;

	if (bIsGrabbing == false)
	{
		TryGrab();
	}
	else
	{
		if (GrabbedObject)
		{
			TryThrow();
		}
	}
}

void APlayerCharacter::Dash()
{
	// ��� ������ �������� üũ
	if (CanDash() == false) return;

	// ���� ����
	InitState();
	bIsDashing = true;

	// 4���� ȸ��
	PerformDodge();

	// �̵��ӵ� ����
	MoveComp->MaxWalkSpeed = 500.f;
}

void APlayerCharacter::StopDash()
{
	// ���� ����
	bIsDashing = false;

	// �̵��ӵ� �ʱ�ȭ
	MoveComp->MaxWalkSpeed = 300.f;
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
	if (bIsAttacking == true) return false;
	if (bIsBlocking == true) return false;

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
	if (MoveComp->IsFalling() == true) false;

	return true;
}

void APlayerCharacter::RotateToDirection(FVector Direction, float DeltaTime, float InterpSpeed)
{
	FRotator temp = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), GetActorLocation() + Direction);

	FRotator newRotation;
	newRotation.Pitch = GetActorRotation().Pitch;
	newRotation.Yaw = FMath::RInterpTo(GetActorRotation(), temp, DeltaTime, InterpSpeed).Yaw;
	newRotation.Roll = GetActorRotation().Roll;

	SetActorRotation(newRotation);
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

void APlayerCharacter::OnParryEnd()
{
	bIsParrying = false;
}

void APlayerCharacter::PerformDodge()
{
	// �ܻ� ����
	SpawnGhostTrail();

	// ȸ�� ���� ���
	FVector inputVector = GetLastMovementInputVector();
	FRotator baseRotation = GetActorRotation();
	float dodgeAngle = CalculateDirection(inputVector, baseRotation);

	if (bIsTargeting == true)
	{
		// ���� ���� ȸ��
		if (dodgeAngle >= -45.0f && dodgeAngle <= 45.f)
		{
			PlayAnimMontage(DodgeMontages[0]); // ������
		}
		else if (dodgeAngle > 45.f && dodgeAngle <= 135.f)
		{
			PlayAnimMontage(DodgeMontages[1]); // ����������
		}
		else if (dodgeAngle > -135.f && dodgeAngle < -45.f)
		{
			PlayAnimMontage(DodgeMontages[2]); // ��������
		}
		else // (dodgeAngle 135.f > && dodgeAngle < -135.f)
		{
			PlayAnimMontage(DodgeMontages[3]); // �ڷ�
		}
	}
	else // ������ ���� ȸ��
	{
		if (GetLastMovementInputVector().IsNearlyZero() == false)
		{
			FRotator newRotation = FRotationMatrix::MakeFromX(GetLastMovementInputVector()).Rotator();
			SetActorRotation(newRotation);			// ���� ��ȯ}
		}
		PlayAnimMontage(DodgeMontages[0]);		// ������
	}
}

void APlayerCharacter::FinishEnemy()
{
	PlayAnimMontage(FinisherMontages[0]);
}

void APlayerCharacter::TryGrab()
{
	// �ִϸ��̼� ��� ���̸� Ż��
	if (GetMesh()->GetAnimInstance()->IsAnyMontagePlaying()) return;
	
	FHitResult hit;
	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
	objectTypes.Emplace(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	TArray<AActor*> actorToIgnores;

	// ���� ���� �ֿ� ���� ã��
	if (UKismetSystemLibrary::SphereTraceSingleForObjects(this, GetActorLocation(), GetActorLocation(), 150.f, objectTypes, false, actorToIgnores,
			EDrawDebugTrace::ForDuration, hit, true, FColor::Red, FColor::Green, 1.f))
	{
		// ã�� �����ϸ� �ֿ�
		GrabbedObject = Cast<AGrabbableActorBase>(hit.GetActor());
		GrabbedObject->OnGrabbed(this);

		PlayAnimMontage(GrabMontage);
		bIsGrabbing = true;
	}
}

void APlayerCharacter::TryThrow()
{
	if (bIsGrabbing == true && GrabbedObject)
	{
		// ������ ���� ������
		if (TryAutoTargeting(800.f) == true)
		{
			PlayAnimMontage(ThrowMontage);
		}
		// ���� ������
		else
		{
			PerformDiscard();
		}
	}
}

void APlayerCharacter::PerformThrow()
{
	if (bIsGrabbing == true && GrabbedObject)
	{
		GrabbedObject->OnThrown(EnemyTarget->GetActorLocation());
	}
	bIsGrabbing = false;
	GrabbedObject = nullptr;
}

void APlayerCharacter::PerformDiscard()
{
	GrabbedObject->OnDiscard();
	bIsGrabbing = false;
	GrabbedObject = nullptr;
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

bool APlayerCharacter::TryAutoTargeting(float SearchRadius)
{
	// ���Ǿ� Ʈ���̽� �غ�
	TArray<AActor*> actorToIgnore;
	FHitResult hit;
	bool bSuccess = false;

	// ����Ű �Է��� ���� ���
	if (GetLastMovementInputVector().Size() > 0.f)
	{
		// ����Ű ���⿡�� ���� ���Ǿ� Ʈ���̽�
		FVector loc = GetActorLocation() + GetLastMovementInputVector() * 200.f;
		bSuccess = UKismetSystemLibrary::SphereTraceSingle(this, loc, loc, SearchRadius * 0.5f, TraceTypeQuery3, false, actorToIgnore,
			EDrawDebugTrace::None, hit, true, FColor::Red, FColor::Green, 1.f);

		// ���� ã���� Ÿ������ �����ϰ� true ��ȯ
		if (bSuccess == true)
		{
			EnemyTarget = hit.GetActor();
			bIsTargeting = true;
			return true;
		}
		else // ����Ű ���⿡�� ���� ��ã���� ���
		{
			// ���ڸ����� �а� ���Ǿ� Ʈ���̽�
			bSuccess = UKismetSystemLibrary::SphereTraceSingle(this, GetActorLocation(), GetActorLocation(), SearchRadius, TraceTypeQuery3, false, actorToIgnore,
				EDrawDebugTrace::None, hit, true, FColor::Red, FColor::Green, 1.f);

			// ���� ã���� Ÿ������ �����ϰ� true ��ȯ
			if (bSuccess == true)
			{
				EnemyTarget = hit.GetActor();
				bIsTargeting = true;
				return true;
			}
		}
	}
	// ����Ű �Է� ���� ���
	else
	{
		// ���ڸ����� �а� ���Ǿ� Ʈ���̽�
		bSuccess = UKismetSystemLibrary::SphereTraceSingle(this, GetActorLocation(), GetActorLocation(), SearchRadius, TraceTypeQuery3, false, actorToIgnore,
			EDrawDebugTrace::None, hit, true, FColor::Red, FColor::Green, 1.f);

		// ���� ã���� Ÿ������ �����ϰ� true ��ȯ
		if (bSuccess == true)
		{
			EnemyTarget = hit.GetActor();
			bIsTargeting = true;
			return true;
		}
	}

	// ���� ��ã���� false ��ȯ
	return false;
}

void APlayerCharacter::WInput() {
	CreateMoveCommand(FVector2D(1, 0));
}

void APlayerCharacter::SInput() {
	CreateMoveCommand(FVector2D(-1, 0));
}

void APlayerCharacter::DInput() {
	CreateMoveCommand(FVector2D(0, 1));
}

void APlayerCharacter::AInput() {
	CreateMoveCommand(FVector2D(0, -1));
}

void APlayerCharacter::CreateMoveCommand(FVector2D InputDirection) {
	// ������ Ŀ�ǵ� ��̰� ������
	if (Tail >= 9) {
		// �� ������ ���Ҹ� �� ������ ����� �������� ����Ű�� �ε������� 0������
		FVector2D tempMovement = MoveCommands[Tail];
		MoveCommands.EmplaceAt(0, tempMovement);
		Tail = 0;
	}
	// ���� ���� ������ Ŀ�ǵ带 ������ �ε��� + 1��° ���ҷ�
	MoveCommands.EmplaceAt(++Tail, InputDirection);
	// ���������� ������ �ð� ����
	LastMoveTime = 0.f;
}

void APlayerCharacter::Attack() {
	// ���� ���� �ƴϸ�
	if (CanAttack()) {
		// ���� ������ ��ȯ
		bIsAttacking = true;
		// ���� Ÿ�������� Ÿ�� ����
		TryAutoTargeting();

		// �������ϰ� ȸ��
		RInterpSpeed = 5.f;

		// ���� ���̸� ��������
		if (MoveComp->IsFalling()) {
			JumpAttack();
			return;
		}
		// ������ Ŀ�ǵ� ��̿� 2�� �̻��� ���Ұ� ������
		if (Tail > 0) {
			// �������� �������� �տ� �ִ� ���Ҹ� ��ġ�� ���̸� ����
			float vectorLength = (MoveCommands[Tail] + MoveCommands[Tail - 1]).Size();
			if (vectorLength <= 0) {
				SpecialAttack();
			}
			else if (vectorLength >= 2) {
				DashAttack();
			}
			else {
				ComboAttack();
			}
		}
		else {
			ComboAttack();
		}
		Tail = -1;
		LastAttackTime = 0.f;
	}
}

void APlayerCharacter::InitState() {
	StopAnimMontage();
	bIsAttacking = false;
	bIsBlocking = false;
	bIsDashing = false;
	bIsParrying = false;
}

void APlayerCharacter::JumpAttack() {
	PlayAnimMontage(JumpAttackMontage);
	Combo = -1;
}

void APlayerCharacter::SpecialAttack() {
	PlayAnimMontage(SpecialAttackMontages[SpecialAttackIndex]);
	Combo = -1;
}

void APlayerCharacter::DashAttack() {
	PlayAnimMontage(DashAttackMontage);
	Combo = -1;
}

void APlayerCharacter::ComboAttack() {
	Combo++;
	if (Combo > 3) {
		Combo = 0;
	}
	PlayAnimMontage(ComboAttackMontages[Combo]);
}

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) {
	
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	FHitResult outHit;
	FVector outImpulse;
	DamageEvent.GetBestHitInfo(this,DamageCauser,outHit,outImpulse);
	// �� �������� ȸ��
	RotateToDirection(DamageCauser->GetActorLocation(), 0.f, 0.f);
	if (bIsParrying) {
		ParryHit(DamageAmount,outHit.Item);
	}
	else if (bIsBlocking) {
		GuardHit(DamageAmount, outHit.Item);
	}
	else {
		Hit(DamageAmount, outHit.Item);
		// ���� �ֿ� ���¿��� �ǰ� �� ���� ����
		if (bIsGrabbing == true && GrabbedObject)
		{
			PerformDiscard();
		}
	}
	return DamageAmount;
}

void APlayerCharacter::ParryHit(float Damage, int DamageType) {
	Damage *= 0.2f;
	CurPosture -= Damage;
	PlayAnimMontage(ParryHitMontages[DamageType]);
}

void APlayerCharacter::GuardHit(float Damage, int DamageType) {
	Damage *= 1.2f;
	CurPosture -= Damage;
	if (CurPosture <= 0) {
		GuardBreak();
		return;
	}
	PlayAnimMontage(GuardHitMontages[DamageType]);
}

void APlayerCharacter::Hit(float Damage, int DamageType) {
	CurPosture -= Damage * 0.4f;
	CurHealth -= Damage;
	if (CurHealth <= 0) {
		Die();
		return;
	}
	PlayAnimMontage(HitReactionMontages[DamageType]);
}

void APlayerCharacter::GuardBreak() {
	bGuardBroken = true;
	PlayAnimMontage(GuardBreakMontage);
}

void APlayerCharacter::Die() {
	bIsDead = true;
	PlayAnimMontage(DieMontage);
}
