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

	// 캡슐
	GetCapsuleComponent()->InitCapsuleSize(40.f, 90.0f);

	// 칼
	Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Katana"));
	Weapon->SetupAttachment(GetMesh(), TEXT("katana3"));
	Weapon->SetCollisionProfileName(TEXT("NoCollision"));

	// 칼집
	Scabbard = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Scabbard"));
	Scabbard->SetupAttachment(GetMesh(), TEXT("katana1"));
	Scabbard->SetCollisionProfileName(TEXT("NoCollision"));

	// 기본 스프링암
	DefaultCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("DefaultCameraBoom"));
	DefaultCameraBoom->SetupAttachment(RootComponent);
	DefaultCameraBoom->TargetArmLength = 300.0f;
	DefaultCameraBoom->SocketOffset = FVector(0.f, 0.f, 25.f);
	DefaultCameraBoom->bUsePawnControlRotation = true;
	DefaultCameraBoom->bEnableCameraLag = true;	// 카메라 랙 활성화
	DefaultCameraBoom->CameraLagSpeed = 8.f;

	// 카메라
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(DefaultCameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// 물건 들기
	GrabPoint = CreateDefaultSubobject<USceneComponent>(TEXT("GrabPoint"));
	GrabPoint->SetupAttachment(RootComponent);
	GrabPoint->SetRelativeLocation(FVector(100.f, -20.f, 100.f));

	// 기본 조명
	CharLighting = CreateDefaultSubobject<USpotLightComponent>(TEXT("CharLighting"));
	CharLighting->SetupAttachment(RootComponent);
	CharLighting->SetRelativeLocationAndRotation(FVector(0.f, 0.f, 220.f), FRotator(-90.f, 0.f, 0.f));
	CharLighting->SetOuterConeAngle(24.f);
	CharLighting->SetIntensity(800.f);
	CharLighting->SetAttenuationRadius(350.f);
	CharLighting->SetCastShadows(false);

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
	CombatComp->SetupWeapon(Weapon);
	CombatComp->AttackTrace = TraceTypeQuery4;
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 타게팅한 적을 향해 회전
	if (bIsTargeting && EnemyTarget)
	{
		FVector direction = EnemyTarget->GetActorLocation() - GetActorLocation();
		RotateToDirection(direction, DeltaTime, RInterpSpeed);
		// 거리가 멀어지면 타게팅 해제
		if (GetDistanceTo(EnemyTarget) > 800.f)
		{
			bIsTargeting = false;
			EnemyTarget = nullptr;
		}
	}
	// 방향키 방향으로 회전
	else if (bIsAttacking)
	{
		FVector inputVector = GetLastMovementInputVector();
		if (!inputVector.IsNearlyZero())
		{
			RotateToDirection(inputVector, DeltaTime, 4.f);
		}
	}

	if ((!MoveComp->IsFalling()) && bIsLightningCharged) {
		Groggy(); // 피격시에만 체크해도 되는 부분이면 TakeDamage()에 옮기는게 낫지 않을까? - 우성
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
	RegeneratePosture(); // 프레임레이트 영향 없애기 위해 TakeDamage()에서 타이머 + 루프 걸어서 호출하는게 낫지 않을까? - 우성
	SkillCooltime+=DeltaTime;
	SpecialAttackCooltime+=DeltaTime;
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
		// 물건 들고 있으면 떨구기
		if (bIsGrabbing == true && GrabbedActor)
		{
			DropProp();
		}
		ACharacter::Jump();
	}
}

void APlayerCharacter::Guard()
{
	// 물건 들고 있으면 떨구기
	if (bIsGrabbing == true && GrabbedActor)
	{
		DropProp();
	}

	// TODO: 가드 가능한 상태인지 체크
	bIsBlocking = true;

	// 0.3초 동안 패링 판정 발동 
	bIsParrying = true;
	GetWorldTimerManager().SetTimer(ParryingTimer, this, &APlayerCharacter::OnParryEnd, 0.3f, false);

	// ABP의 스테이트 변경
	UPlayerAnimInstance* animIns = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
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
	UPlayerAnimInstance* animIns = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	animIns->bIsBlocking = false;

	// 이동속도 초기화
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
		
		// 물건 들고 있으면 떨구기
		if (bIsGrabbing == true && GrabbedActor)
		{
			DropProp();
		}

		// 4방향 회피
		PerformDodge();

		// 이동속도 증가
		MoveComp->MaxWalkSpeed = 500.f;
	}
}

void APlayerCharacter::StopDash()
{
	// 이동속도 초기화
	MoveComp->MaxWalkSpeed = 300.f;
}

void APlayerCharacter::Finisher()
{
	if (CanAttack()) {
		// 대상 찾고 태그 확인
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
	// TODO : 상태 체크
	return !GetCurrentMontage();
}

bool APlayerCharacter::CanAttack()	
{
	// 공격중이거나 회피/스킬사용, 가드브레이크 중이 아니면 true 리턴
	return !(bIsAttacking || bIsInvincible || bIsGuardBroken || bIsHit);
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
	// 회피 중이거나 낙하 중, 대쉬 중, 가드브레이크 상태가 아니면 true 리턴
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
	// 잔상 생성
	SpawnGhostTrail();

	// 회피 방향 계산
	FVector inputVector = GetLastMovementInputVector();
	FRotator baseRotation = GetActorRotation();
	float dodgeAngle = CalculateDirection(inputVector, baseRotation);
	bIsInvincible = true;
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
		if (GetLastMovementInputVector().IsNearlyZero() == false)
		{
			FRotator newRotation = FRotationMatrix::MakeFromX(GetLastMovementInputVector()).Rotator();
			SetActorRotation(newRotation);			// 방향 전환}
		}
		PlayAnimMontage(DodgeMontages[0]);		// 앞으로
	}
}

void APlayerCharacter::FinishEnemy()
{
	PlayAnimMontage(FinisherMontages[0]);
}

void APlayerCharacter::PullProp()
{
	// 애니메이션 재생 중이면 탈출
	if (GetMesh()->GetAnimInstance()->IsAnyMontagePlaying()) return;

	FHitResult hit;
	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
	objectTypes.Emplace(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	TArray<AActor*> actorToIgnores;
	FVector loc = GetActorLocation();

	// 범위 내에 주울 물건 찾기
	if (UKismetSystemLibrary::SphereTraceSingleForObjects(this, loc, loc, 150.f, objectTypes, false, actorToIgnores,
		EDrawDebugTrace::None, hit, true, FColor::Red, FColor::Green, 5.f))
	{
		// 찾기 성공하면 주움
		if (hit.GetActor()->IsA(AGrabbableActorBase::StaticClass()))
		{
			GrabbedActor = Cast<AGrabbableActorBase>(hit.GetActor());
			GrabbedActor->OnGrabbed();

			// 컴포넌트 이동시키기
			FLatentActionInfo info;
			info.CallbackTarget = this;
			info.ExecutionFunction = FName("AttachGrabbedActor");	// MoveComponentTo가 완료되면 호출
			info.Linkage = 0;

			UKismetSystemLibrary::MoveComponentTo(GrabbedActor->GetRootComponent(), GrabPoint->GetComponentLocation(), GetActorRotation(), true, true, 0.2f, true, EMoveComponentAction::Move, info);

			PlayAnimMontage(InteractionMontages[0]);

			bIsGrabbing = true;
		}
	}
}

void APlayerCharacter::PushProp()
{
	// 몽타주 재생
	RotateToDirection(EnemyTarget->GetActorLocation());
	PlayAnimMontage(InteractionMontages[1]);

	// 딜레이 람다함수
	FTimerHandle delayHandle;
	float delayTime = 0.05f;
	GetWorld()->GetTimerManager().SetTimer(delayHandle, FTimerDelegate::CreateLambda([&]()
		{
			GrabbedActor->OnPushed();

			// 발사하기
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
	// 스피어 트레이스 준비
	TArray<AActor*> actorToIgnore;
	FHitResult hit;
	bool bSuccess;

	// 제자리에서 트레이스
	bSuccess = UKismetSystemLibrary::SphereTraceSingle(this, GetActorLocation(), GetActorLocation(), SearchRadius, ETraceTypeQuery::TraceTypeQuery3, false, actorToIgnore,
		EDrawDebugTrace::None, hit, true, FColor::Red, FColor::Green, 1.f);
	// 적을 찾으면 타겟으로 지정
	if (bSuccess == true)
	{
		EnemyTarget = hit.GetActor();
		UKismetSystemLibrary::PrintString(GetWorld(), EnemyTarget->GetName());
		bIsTargeting = true;
	}

	// 방향키 입력이 있을 경우
	if (GetLastMovementInputVector().Size() > 0.f)
	{
		// 방향키 방향으로 다시 트레이스
		FVector loc = GetActorLocation() + GetLastMovementInputVector() * 200.f;
		bSuccess = UKismetSystemLibrary::SphereTraceSingle(this, loc, loc, SearchRadius * 0.5f, ETraceTypeQuery::TraceTypeQuery3, false, actorToIgnore,
			EDrawDebugTrace::None, hit, true, FColor::Red, FColor::Green, 1.f);
		// 적을 찾으면 타겟으로 지정하고 true 반환
		if (bSuccess == true)
		{
			EnemyTarget = hit.GetActor();
			bIsTargeting = true;
			return true;
		}
	}

	// 적을 못찾으면 true, 못찾으면 false 반환
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

		// 물건 들고 있을 경우에는 공격 대체
		if (bIsGrabbing == true)
		{
			Interact();
			return;
		}

		// 오토 타겟팅으로 타겟 지정
		TryAutoTargeting();

		// 스무스하게 회전
		RInterpSpeed = 5.f;

		// 점프 중이면 점프공격
		if (MoveComp->IsFalling()) {
			JumpAttack();
		}
		// 움직임 커맨드 어레이에 2개 이상의 원소가 있으면
		else if (Tail > 0) {
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

		if (GetCurrentMontage()) {
			Tail = -1;
			LastAttackTime = 0.f;
			// 공격 중으로 전환
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
	// 적 방향으로 회전
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
		// 물건 주운	 상태에서 피격 시 물건 떨굼
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