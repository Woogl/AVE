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

	// 캡슐
	GetCapsuleComponent()->InitCapsuleSize(40.f, 90.0f);

	// 스켈레탈 메쉬 블루프린트에서 설정 되나 테스트 예정
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> meshAsset(TEXT("SkeletalMesh'/Game/ThirdPerson/Characters/Mannequin_UE4/Meshes/SK_Mannequin.SK_Mannequin'"));
	if (meshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(meshAsset.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -90.f), FRotator(0.f, -90.f, 0.f));
	}

	// 칼
	Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Katana"));
	Weapon->SetupAttachment(GetMesh(), TEXT("katana3"));
	Weapon->SetCollisionProfileName(TEXT("NoCollision"));

	// 칼집
	Scabbard = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Scabbard"));
	Scabbard->SetupAttachment(GetMesh(), TEXT("scabbard1"));
	Scabbard->SetCollisionProfileName(TEXT("NoCollision"));

	// 기본 스프링암
	DefaultCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("DefaultCameraBoom"));
	DefaultCameraBoom->SetupAttachment(RootComponent);
	DefaultCameraBoom->TargetArmLength = 300.0f;
	DefaultCameraBoom->bUsePawnControlRotation = true;
	DefaultCameraBoom->bEnableCameraLag = true;	// 카메라 랙 활성화
	DefaultCameraBoom->CameraLagSpeed = 4.f;

	// 좌측 사이드뷰 스프링암
	LeftCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("LeftCameraBoom"));
	LeftCameraBoom->SetupAttachment(RootComponent);
	LeftCameraBoom->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	LeftCameraBoom->TargetArmLength = 225.f;

	// 우측 사이드뷰 스프링암
	RightCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("RightCameraBoom"));
	RightCameraBoom->SetupAttachment(RootComponent);
	RightCameraBoom->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	RightCameraBoom->TargetArmLength = 225.f;

	// 카메라
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(DefaultCameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// 회전 설정
	TurnRateGamepad = 50.f;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	// 이동 설정
	MoveComp = GetCharacterMovement();
	MoveComp->bOrientRotationToMovement = true;
	MoveComp->RotationRate = FRotator(0.0f, 350.0f, 0.0f);
	MoveComp->JumpZVelocity = 400.f;
	MoveComp->AirControl = 0.35f;
	MoveComp->MaxWalkSpeed = 300.f;
	MoveComp->MinAnalogWalkSpeed = 20.f;
	MoveComp->BrakingDecelerationWalking = 500.f;

	// 공격 판정을 관리하는 컴포넌트
	CombatComp = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComp"));
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// 컴뱃 컴포넌트에 무기 설정
	CombatComp->SetupWeapon(Weapon, 5.0f);
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 타게팅한 적 있으면 Yaw 회전
	if (bIsTargeting == true)
	{
		RotateToTarget(EnemyTarget, DeltaTime, RInterpSpeed);
		// 거리가 멀어지면 타게팅 해제
		if (GetDistanceTo(EnemyTarget) > 800.f)
		{
			bIsTargeting = false;
			EnemyTarget = nullptr;
		}
	}
	else if (bIsAttacking == true)
	{
		// 방향키 방향으로 부드럽게 회전
		RotateToInputDirection(DeltaTime, 5.0f);
	}

	LastMoveTime += DeltaTime;
	LastAttackTime += DeltaTime;
	// 마지막 방향키 인풋이 0.4초를 지났으면
	if (LastMoveTime > MoveResetLimit) {
		LastMoveTime = 0.f;
		Tail = -1;
	}
	// 공격 모션이 끝나고 0.6초가 지나면
	if (!bIsAttacking && LastAttackTime > ComboResetLimit) {
		// 콤보를 초기화
		Combo = -1;
		LastAttackTime = 0.f;
	}

}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);
	// 움직임 커맨드 생성을 위한 WASD 프레스 이벤트 바인딩
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
	// TODO: 가드 가능한 상태인지 체크
	bIsBlocking = true;
	
	// 0.3초 동안 패링 판정 발동 
	bIsParrying = true;
	GetWorldTimerManager().SetTimer(ParryingTimer, this, &APlayerCharacter::OnParryEnd, 0.3f, false);

	// ABP의 스테이트 변경
	auto animIns = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	animIns->bIsBlocking = true;

	// 이동속도 감소
	MoveComp->MaxWalkSpeed = 150.f;
	MoveComp->MaxAcceleration = 512.f;
	MoveComp->BrakingDecelerationWalking = 32.f;
}

void APlayerCharacter::StopGuard()
{
	bIsBlocking = false;

	// 패링 판정 종료
	bIsParrying = false;
	GetWorldTimerManager().ClearTimer(ParryingTimer);

	// ABP의 스테이트 변경
	auto animIns = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	animIns->bIsBlocking = false;

	// 이동속도 초기화
	MoveComp->MaxWalkSpeed = 300.f;
	MoveComp->MaxAcceleration = 2048.f;
	MoveComp->BrakingDecelerationWalking = 500.f;
}

void APlayerCharacter::Interact()
{
	// TODO: 물건 줍기 등
}

void APlayerCharacter::Dash()
{
	// 대시 가능한 상태인지 체크
	if (CanDash() == false) return;

	// 상태 변경
	bIsDashing = true;
	bIsAttacking = false;

	// 4방향 회피
	PerformDodge();

	// 이동속도 증가
	MoveComp->MaxWalkSpeed = 500.f;
}

void APlayerCharacter::StopDash()
{
	// 상태 변경
	bIsDashing = false;

	// 이동속도 초기화
	MoveComp->MaxWalkSpeed = 300.f;
}

void APlayerCharacter::Finisher()
{
	if (CanAttack() == false) return;

	// 대상 찾기
	// TODO: 체간 수치 체크
	if (TryAutoTargeting() == true)
	{
		MotionMorph();
	}
}

bool APlayerCharacter::CanJump()
{
	// TODO : 상태 체크
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
	// TODO : 상태 체크
	return true;
}

bool APlayerCharacter::CanInteract()
{
	// TODO : 상태 체크
	return true;
}

bool APlayerCharacter::CanDash()
{
	if (MoveComp->IsFalling() == true) false;

	return true;
}

void APlayerCharacter::RotateToTarget(AActor* Target, float DeltaTime, float InterpSpeed)
{
	// 타겟팅한 적이 있는 경우
	if (Target)
	{
		FRotator temp = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target->GetActorLocation());

		FRotator newRotation;
		newRotation.Pitch = GetActorRotation().Pitch;
		newRotation.Yaw = FMath::RInterpTo(GetActorRotation(), temp, DeltaTime, InterpSpeed).Yaw;
		newRotation.Roll = GetActorRotation().Roll;
		SetActorRotation(newRotation);
	}
}

void APlayerCharacter::RotateToInputDirection(float DeltaTime, float InterpSpeed)
{
	FVector inputVector = GetLastMovementInputVector();
	if (inputVector.IsNearlyZero() == false)
	{
		FRotator temp = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), GetActorLocation() + inputVector);

		FRotator newRotation;
		newRotation.Pitch = GetActorRotation().Pitch;
		newRotation.Yaw = FMath::RInterpTo(GetActorRotation(), temp, DeltaTime, InterpSpeed).Yaw;
		newRotation.Roll = GetActorRotation().Roll;
		SetActorRotation(newRotation);
	}
}

float APlayerCharacter::CalculateDirection(const FVector& Velocity, const FRotator& BaseRotation)
{
	// BaseRotation을 기준으로 Vector와의 사이각을 계산 (-180 ~ +180)
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
	// 회피 방향 계산
	FVector inputVector = GetLastMovementInputVector();
	FRotator baseRotation = GetActorRotation();
	float dodgeAngle = CalculateDirection(inputVector, baseRotation);

	if (bIsTargeting == true)
	{
		// 조준 상태 회피
		if (dodgeAngle >= -45.0f && dodgeAngle <= 45.f)
		{
			PlayAnimMontage(DodgeMontages[0]); // 앞으로
		}
		else if (dodgeAngle > 45.f && dodgeAngle <= 135.f)
		{
			PlayAnimMontage(DodgeMontages[1]); // 오른쪽으로
		}
		else if (dodgeAngle > -135.f && dodgeAngle < -45.f)
		{
			PlayAnimMontage(DodgeMontages[2]); // 왼쪽으로
		}
		else // (dodgeAngle 135.f > && dodgeAngle < -135.f)
		{
			PlayAnimMontage(DodgeMontages[3]); // 뒤로
		}
	}
	else // 비조준 상태 회피
	{
		FRotator newRotation = FRotationMatrix::MakeFromX(GetLastMovementInputVector()).Rotator();
		SetActorRotation(newRotation);			// 방향 전환
		PlayAnimMontage(DodgeMontages[0]);		// 앞으로
	}
}

void APlayerCharacter::FinishEnemy()
{
	PlayAnimMontage(FinisherMontages[0]);
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
	// "Targeting" 채널로 트레이스 (1차, 제자리에서 크게)
	TArray<AActor*> actorToIgnore;
	FHitResult hit;
	bool outValue = UKismetSystemLibrary::SphereTraceSingle(this, GetActorLocation(), GetActorLocation(), 300.f, TraceTypeQuery3, false, actorToIgnore,
		EDrawDebugTrace::ForDuration, hit, true, FColor::Red, FColor::Green, 1.f);
	
	// 범위 내 적을 못찾으면 false 반환
	if (outValue == false)
		return false;

	// 적을 찾으면 타겟으로 지정
	EnemyTarget = hit.GetActor();
	bIsTargeting = true;

	// 인풋 방향이 있으면 한번 더 트레이스 (2차, 인풋 방향으로 작게)
	if (GetLastMovementInputVector().Size() > 0.f)
	{
		FVector start = GetActorLocation() + GetLastMovementInputVector() * 200.f;
		FVector end = GetActorLocation() + GetLastMovementInputVector() * 200.f;
		bool outValue2 = UKismetSystemLibrary::SphereTraceSingle(this, start, end, 100.f, TraceTypeQuery3, false, actorToIgnore,
			EDrawDebugTrace::ForDuration, hit, true, FColor::Red, FColor::Green, 1.f);

		// 범위 내 적을 찾으면 찾은 적을 갱신
		if (outValue2 == true)
		{
			EnemyTarget = hit.GetActor();
		}
	}
	return true;
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
	// 움직임 커맨드 어레이가 꽉차면
	if (Tail >= 9) {
		// 맨 마지막 원소를 맨 앞으로 땡기고 마지막을 가리키는 인덱스값을 0번으로
		FVector2D tempMovement = MoveCommands[Tail];
		MoveCommands.EmplaceAt(0, tempMovement);
		Tail = 0;
	}
	// 새로 들어온 움직임 커맨드를 마지막 인덱스 + 1번째 원소로
	MoveCommands.EmplaceAt(++Tail, InputDirection);
	// 마지막으로 움직인 시간 리셋
	LastMoveTime = 0.f;
}

void APlayerCharacter::Attack() {
	// 공격 중이 아니면
	if (CanAttack()) {
		// 공격 중으로 전환
		bIsAttacking = true;
		// 오토 타겟팅으로 타겟 지정
		TryAutoTargeting();

		// 스무스하게 회전
		RInterpSpeed = 5.f;

		// 점프 중이면 점프공격
		if (MoveComp->IsFalling()) {
			JumpAttack();
			return;
		}
		// 움직임 커맨드 어레이에 2개 이상의 원소가 있으면
		if (Tail > 0) {
			// 마지막과 마지막의 앞에 있는 원소를 합치고 길이를 저장
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
	
	FHitResult outHit;
	FVector outImpulse;
	DamageEvent.GetBestHitInfo(this,DamageCauser,outHit,outImpulse);
	// 적 방향으로 회전
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), DamageCauser->GetActorLocation()));
	if (bIsParrying) {
		ParryHit(DamageAmount,outHit.Item);
	}
	else if (bIsBlocking) {
		GuardHit(DamageAmount, outHit.Item);
	}
	else {
		Hit(DamageAmount, outHit.Item);
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
}