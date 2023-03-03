// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include <Components/CapsuleComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "CombatComponent.h"
#include <Kismet/KismetMathLibrary.h>
#include <Kismet/KismetSystemLibrary.h>	
#include "PlayerAnimInstance.h"
#include "GrabbableActorBase.h"
#include "Kismet/GameplayStatics.h"
#include "AllAVEDamageTypes.h"
#include <Components/SpotLightComponent.h>

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// ĸ��
	GetCapsuleComponent()->InitCapsuleSize(40.f, 90.0f);

	// Į
	Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Katana"));
	Weapon->SetupAttachment(GetMesh(), TEXT("katana3"));
	Weapon->SetCollisionProfileName(TEXT("NoCollision"));

	// Į��
	Scabbard = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Scabbard"));
	Scabbard->SetupAttachment(GetMesh(), TEXT("katana1"));
	Scabbard->SetCollisionProfileName(TEXT("NoCollision"));

	// �⺻ ��������
	DefaultCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("DefaultCameraBoom"));
	DefaultCameraBoom->SetupAttachment(RootComponent);
	DefaultCameraBoom->TargetArmLength = 300.0f;
	DefaultCameraBoom->SocketOffset = FVector(0.f, 0.f, 25.f);
	DefaultCameraBoom->bUsePawnControlRotation = true;
	DefaultCameraBoom->bEnableCameraLag = true;	// ī�޶� �� Ȱ��ȭ
	DefaultCameraBoom->CameraLagSpeed = 8.f;

	// ī�޶�
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(DefaultCameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// ���� ���
	GrabPoint = CreateDefaultSubobject<USceneComponent>(TEXT("GrabPoint"));
	GrabPoint->SetupAttachment(RootComponent);
	GrabPoint->SetRelativeLocation(FVector(100.f, -20.f, 100.f));

	// �⺻ ����
	CharLighting = CreateDefaultSubobject<USpotLightComponent>(TEXT("CharLighting"));
	CharLighting->SetupAttachment(RootComponent);
	CharLighting->SetRelativeLocationAndRotation(FVector(0.f, 0.f, 220.f), FRotator(-90.f, 0.f, 0.f));
	CharLighting->SetOuterConeAngle(24.f);
	CharLighting->SetIntensity(800.f);
	CharLighting->SetAttenuationRadius(350.f);
	CharLighting->SetCastShadows(false);

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
	CombatComp->AttackTrace = TraceTypeQuery4;
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Ÿ������ ���� ���� ȸ��
	if (bIsTargeting && EnemyTarget)
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
	else if (bIsAttacking)
	{
		FVector inputVector = GetLastMovementInputVector();
		if (!inputVector.IsNearlyZero())
		{
			RotateToDirection(inputVector, DeltaTime, 4.f);
		}
	}

	if ((!MoveComp->IsFalling()) && bIsLightningCharged) {
		Groggy(); // �ǰݽÿ��� üũ�ص� �Ǵ� �κ��̸� TakeDamage()�� �ű�°� ���� ������? - �켺
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
	RegeneratePosture(); // �����ӷ���Ʈ ���� ���ֱ� ���� TakeDamage()���� Ÿ�̸� + ���� �ɾ ȣ���ϴ°� ���� ������? - �켺
	SkillCooltime+=DeltaTime;
	SpecialAttackCooltime+=DeltaTime;
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
	PlayerInputComponent->BindAction("Skill", IE_Pressed, this, &APlayerCharacter::Skill);
	PlayerInputComponent->BindAction("ChangeSkill", IE_Pressed, this, &APlayerCharacter::ChangeSkill);
	PlayerInputComponent->BindAction("ChangeSpecialAttack", IE_Pressed, this, &APlayerCharacter::ChangeSpecialAttack);

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
	if (CanJump())
	{
		// ���� ��� ������ ������
		if (bIsGrabbing == true && GrabbedActor)
		{
			DropProp();
		}
		ACharacter::Jump();
	}
}

void APlayerCharacter::Guard()
{
	// ���� ��� ������ ������
	if (bIsGrabbing == true && GrabbedActor)
	{
		DropProp();
	}

	// TODO: ���� ������ �������� üũ
	bIsBlocking = true;

	// 0.3�� ���� �и� ���� �ߵ� 
	bIsParrying = true;
	GetWorldTimerManager().SetTimer(ParryingTimer, this, &APlayerCharacter::OnParryEnd, 0.3f, false);

	// ABP�� ������Ʈ ����
	UPlayerAnimInstance* animIns = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
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
	UPlayerAnimInstance* animIns = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
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
		PullProp();
	}
	else
	{
		if (GrabbedActor)
		{
			if (TryAutoTargeting(800.f) == true)
			{
				PushProp();
			}
			else
			{
				DropProp();
			}
		}
	}
}

void APlayerCharacter::Dash()
{
	if (CanDash()) {
		bIsDashing = true;
		
		// ���� ��� ������ ������
		if (bIsGrabbing == true && GrabbedActor)
		{
			DropProp();
		}

		// 4���� ȸ��
		PerformDodge();

		// �̵��ӵ� ����
		MoveComp->MaxWalkSpeed = 500.f;
	}
}

void APlayerCharacter::StopDash()
{
	// �̵��ӵ� �ʱ�ȭ
	MoveComp->MaxWalkSpeed = 300.f;
}

void APlayerCharacter::Finisher()
{
	if (CanAttack()) {
		// ��� ã�� �±� Ȯ��
		if (TryAutoTargeting() == true && EnemyTarget->ActorHasTag(TEXT("Broken")))
		{
			PlayFinisherSequence();
			bIsInvincible = true;
			MotionMorph();
			FinishEnemy();
		}
	}
}

bool APlayerCharacter::CanJump()
{
	// TODO : ���� üũ
	return !GetCurrentMontage();
}

bool APlayerCharacter::CanAttack()	
{
	// �������̰ų� ȸ��/��ų���, ����극��ũ ���� �ƴϸ� true ����
	return !(bIsAttacking || bIsInvincible || bIsGuardBroken || bIsHit);
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
	// ȸ�� ���̰ų� ���� ��, �뽬 ��, ����극��ũ ���°� �ƴϸ� true ����
	return !(MoveComp->IsFalling() || bIsInvincible || bIsDashing || bIsGuardBroken || bIsHit);
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
	bIsInvincible = true;
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

void APlayerCharacter::PullProp()
{
	// �ִϸ��̼� ��� ���̸� Ż��
	if (GetMesh()->GetAnimInstance()->IsAnyMontagePlaying()) return;

	FHitResult hit;
	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
	objectTypes.Emplace(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	TArray<AActor*> actorToIgnores;
	FVector loc = GetActorLocation();

	// ���� ���� �ֿ� ���� ã��
	if (UKismetSystemLibrary::SphereTraceSingleForObjects(this, loc, loc, 150.f, objectTypes, false, actorToIgnores,
		EDrawDebugTrace::None, hit, true, FColor::Red, FColor::Green, 5.f))
	{
		// ã�� �����ϸ� �ֿ�
		if (hit.GetActor()->IsA(AGrabbableActorBase::StaticClass()))
		{
			GrabbedActor = Cast<AGrabbableActorBase>(hit.GetActor());
			GrabbedActor->OnGrabbed();

			// ������Ʈ �̵���Ű��
			FLatentActionInfo info;
			info.CallbackTarget = this;
			info.ExecutionFunction = FName("AttachGrabbedActor");	// MoveComponentTo�� �Ϸ�Ǹ� ȣ��
			info.Linkage = 0;

			UKismetSystemLibrary::MoveComponentTo(GrabbedActor->GetRootComponent(), GrabPoint->GetComponentLocation(), GetActorRotation(), true, true, 0.2f, true, EMoveComponentAction::Move, info);

			PlayAnimMontage(InteractionMontages[0]);

			bIsGrabbing = true;
		}
	}
}

void APlayerCharacter::PushProp()
{
	// ��Ÿ�� ���
	RotateToDirection(EnemyTarget->GetActorLocation());
	PlayAnimMontage(InteractionMontages[1]);

	// ������ �����Լ�
	FTimerHandle delayHandle;
	float delayTime = 0.05f;
	GetWorld()->GetTimerManager().SetTimer(delayHandle, FTimerDelegate::CreateLambda([&]()
		{
			GrabbedActor->OnPushed();

			// �߻��ϱ�
			FVector OutLaunchVelocity;
			if (UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, OutLaunchVelocity, GrabbedActor->GetActorLocation(), EnemyTarget->GetActorLocation(), 0.f, 0.9f))
			{
				GrabbedActor->GetMesh()->AddImpulse(OutLaunchVelocity * 64);
				bIsGrabbing = false;
				GrabbedActor = nullptr;
			}
			else
			{
				DropProp();
			}
		}), delayTime, false);
}

void APlayerCharacter::DropProp()
{
	GrabbedActor->OnDropped();

	if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(InteractionMontages[0]))
	{
		GetMesh()->GetAnimInstance()->Montage_Stop(0.25f, InteractionMontages[0]);
	}

	bIsGrabbing = false;
	GrabbedActor = nullptr;
}

void APlayerCharacter::AttachGrabbedActor()
{
	GrabbedActor->AttachToComponent(GrabPoint, FAttachmentTransformRules::KeepWorldTransform);
}

bool APlayerCharacter::TryAutoTargeting(float SearchRadius)
{
	// ���Ǿ� Ʈ���̽� �غ�
	TArray<AActor*> actorToIgnore;
	FHitResult hit;
	bool bSuccess;

	// ���ڸ����� Ʈ���̽�
	bSuccess = UKismetSystemLibrary::SphereTraceSingle(this, GetActorLocation(), GetActorLocation(), SearchRadius, ETraceTypeQuery::TraceTypeQuery3, false, actorToIgnore,
		EDrawDebugTrace::None, hit, true, FColor::Red, FColor::Green, 1.f);
	// ���� ã���� Ÿ������ ����
	if (bSuccess == true)
	{
		EnemyTarget = hit.GetActor();
		UKismetSystemLibrary::PrintString(GetWorld(), EnemyTarget->GetName());
		bIsTargeting = true;
	}

	// ����Ű �Է��� ���� ���
	if (GetLastMovementInputVector().Size() > 0.f)
	{
		// ����Ű �������� �ٽ� Ʈ���̽�
		FVector loc = GetActorLocation() + GetLastMovementInputVector() * 200.f;
		bSuccess = UKismetSystemLibrary::SphereTraceSingle(this, loc, loc, SearchRadius * 0.5f, ETraceTypeQuery::TraceTypeQuery3, false, actorToIgnore,
			EDrawDebugTrace::None, hit, true, FColor::Red, FColor::Green, 1.f);
		// ���� ã���� Ÿ������ �����ϰ� true ��ȯ
		if (bSuccess == true)
		{
			EnemyTarget = hit.GetActor();
			bIsTargeting = true;
			return true;
		}
	}

	// ���� ��ã���� true, ��ã���� false ��ȯ
	return bSuccess;
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

		// ���� ��� ���� ��쿡�� ���� ��ü
		if (bIsGrabbing == true)
		{
			Interact();
			return;
		}

		// ���� Ÿ�������� Ÿ�� ����
		TryAutoTargeting();

		// �������ϰ� ȸ��
		RInterpSpeed = 5.f;

		// ���� ���̸� ��������
		if (MoveComp->IsFalling()) {
			JumpAttack();
		}
		// ������ Ŀ�ǵ� ��̿� 2�� �̻��� ���Ұ� ������
		else if (Tail > 0) {
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

		if (GetCurrentMontage()) {
			Tail = -1;
			LastAttackTime = 0.f;
			// ���� ������ ��ȯ
			bIsAttacking = true;
		}
	}
}

void APlayerCharacter::InitState() {
	bIsAttacking = false;
	bIsGuardBroken = false;
	bIsDashing = false;
	bIsHit = false;
}

void APlayerCharacter::InitInvincibility() {
	bIsInvincible = false;
}

void APlayerCharacter::InitCharge() {
	bIsLightningCharged = false;
}

void APlayerCharacter::InitGuard()
{
	StopGuard();
	bIsParrying = false;
}

void APlayerCharacter::JumpAttack() {
	if (bIsLightningCharged) {
		PlayAnimMontage(JumpAttackMontages[1]);
		bIsInvincible = true;
		InitCharge();
	}
	else {
		PlayAnimMontage(JumpAttackMontages[0]);
	}
	Combo = -1;
}

void APlayerCharacter::SpecialAttack() {
	if (SpecialAttackCooltime > 3.f) {
		PlayAnimMontage(SpecialAttackMontages[CurSpecialAttack]);
		Combo = -1;
		SpecialAttackCooltime = 0.f;
	}
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
	if (bIsInvincible) {
		return DamageAmount;
	}
	// �� �������� ȸ��
	RotateToDirection(DamageCauser->GetActorLocation());

	if (DamageCauser && DamageCauser->IsA(ACharacter::StaticClass()))
	{
		EnemyTarget = DamageCauser;
		bIsTargeting = true;
	}
	if (DamageEvent.DamageTypeClass == ULightningDamageType::StaticClass() ) {
		if (MoveComp->IsFalling()) {
			Charge();
		}
		else {
			Groggy();
		}
	}
	else if (bIsParrying) {
		ParryHit(DamageAmount, DamageEvent.DamageTypeClass);
		FHitResult outHit;
		UGameplayStatics::ApplyPointDamage(EnemyTarget,0.f,GetActorLocation(),outHit,GetController(),this,UStandardDamageType::StaticClass());
	}
	else if (bIsBlocking) {
		GuardHit(DamageAmount, DamageEvent.DamageTypeClass);
	}
	else {
		Hit(DamageAmount, DamageEvent.DamageTypeClass);
		// ���� �ֿ�	 ���¿��� �ǰ� �� ���� ����
		if (bIsGrabbing == true && GrabbedActor)
		{
			DropProp();
		}
	}
	bIsHit = true;
	return DamageAmount;
}

void APlayerCharacter::ParryHit(float Damage, TSubclassOf<UDamageType> DamageType) {
	if (DamageType == UStandardDamageType::StaticClass()) {
		Damage = UAVEDamageType::CalculateDamage(Damage, Defense);
		PlayAnimMontage(ParryHitMontages[0]);
	}
	else if (DamageType == UKnockBackDamageType::StaticClass()) {
		Damage = UAVEDamageType::CalculateDamage(Damage, Defense);
		PlayAnimMontage(ParryHitMontages[1]);
	}
	else if (DamageType == UKnockDownDamageType::StaticClass()) {
		Damage = UAVEDamageType::CalculateDamage(Damage, Defense);
		PlayAnimMontage(ParryHitMontages[2]);
	}
	else if (DamageType == UKnockUpDamageType::StaticClass()) {
		Damage = UAVEDamageType::CalculateDamage(Damage, Defense);
		PlayAnimMontage(ParryHitMontages[3]);
	}
	CurPosture -= Damage;
}

void APlayerCharacter::GuardHit(float Damage, TSubclassOf<UDamageType> DamageType) {
	if (DamageType == UStandardDamageType::StaticClass()) {
		Damage = UAVEDamageType::CalculateDamage(Damage, Defense);
		PlayAnimMontage(GuardHitMontages[0]);
	}
	else if (DamageType == UKnockBackDamageType::StaticClass()) {
		Damage = UAVEDamageType::CalculateDamage(Damage, Defense);
		PlayAnimMontage(GuardHitMontages[1]);
	}
	else if (DamageType == UKnockDownDamageType::StaticClass()) {
		Damage = UAVEDamageType::CalculateDamage(Damage, Defense);
		PlayAnimMontage(GuardHitMontages[2]);
	}
	else if (DamageType == UKnockUpDamageType::StaticClass()) {
		Damage = UAVEDamageType::CalculateDamage(Damage, Defense);
		PlayAnimMontage(GuardHitMontages[3]);
	}
	CurPosture -= Damage;
	if (CurPosture <= 0) {
		GuardBreak();
		return;
	}
}

void APlayerCharacter::Hit(float Damage, TSubclassOf<UDamageType> DamageType) {
	if (DamageType == UStandardDamageType::StaticClass()) {
		Damage = UAVEDamageType::CalculateDamage(Damage, Defense);
		PlayAnimMontage(HitReactionMontages[0]);
	}
	else if (DamageType == UKnockBackDamageType::StaticClass()) {
		Damage = UAVEDamageType::CalculateDamage(Damage, Defense);
		PlayAnimMontage(HitReactionMontages[1]);
	}
	else if (DamageType == UKnockDownDamageType::StaticClass()) {
		Damage = UAVEDamageType::CalculateDamage(Damage, Defense);
		PlayAnimMontage(HitReactionMontages[2]);
	}
	else if (DamageType == UKnockUpDamageType::StaticClass()) {
		Damage = UAVEDamageType::CalculateDamage(Damage, Defense);
		PlayAnimMontage(HitReactionMontages[3]);
	}
	CurPosture -= Damage * 0.4f;
	CurHealth -= Damage;
	if (CurHealth <= 0) {
		Die();
		return;
	}
}

void APlayerCharacter::GuardBreak() {
	PlayAnimMontage(GuardBreakMontage);
	bIsGuardBroken = true;
}

void APlayerCharacter::Groggy() {
	InitCharge();
	PlayAnimMontage(GroggyMontage);
	bIsGuardBroken = true;
}

void APlayerCharacter::Charge() {
	PlayAnimMontage(ChargeMontage);
	bIsLightningCharged = true;
}

void APlayerCharacter::Die() {
	PlayAnimMontage(DieMontage);
	bIsDead = true;
}

void APlayerCharacter::Skill() {
	if (CanAttack() && CurKatasiro > 0 && SkillCooltime > 5.f) {
		PlayAnimMontage(SkillMontages[CurSkill]);
		bIsAttacking = true;
		bIsInvincible = true;
		CurKatasiro--;
		SkillCooltime = 0.f;
	}
}

void APlayerCharacter::ChangeSkill() {
	CurSkill++;
	if (CurSkill >= SkillMontages.Num())
		CurSkill = 0;
}

void APlayerCharacter::ChangeSpecialAttack() {
	CurSpecialAttack++;
	if (CurSpecialAttack >= SpecialAttackMontages.Num())
		CurSpecialAttack = 0;
}

void APlayerCharacter::MoveWeaponLeft() {
	Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("katana2"));
	Scabbard->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("katana2"));
}

void APlayerCharacter::MoveWeaponRight() {
	Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("katana3"));
	Scabbard->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("katana1"));
}

void APlayerCharacter::RegeneratePosture() {
	if (!(bIsHit || bIsGuardBroken) && CurPosture < 100.f) {
		CurPosture += 0.05f;
	}
}

void APlayerCharacter::SpreadAoEDamage(TSubclassOf<UDamageType> AttackDamageType) {
	TArray<AActor*> IgnoreList;
	IgnoreList.Add(this);
	UGameplayStatics::ApplyRadialDamage(GetWorld(), 50, GetActorLocation(), 1000.f, AttackDamageType, IgnoreList);
}

void APlayerCharacter::PlayWetFootstepSound(FVector Location) {
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), WetFootstep, Location, 0.5f);
}

void APlayerCharacter::PlayDryFootstepSound(FVector Location) {
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), DryFootstep, Location, 0.5f);
}