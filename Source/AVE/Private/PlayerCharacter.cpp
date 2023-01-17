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

	// 스켈레탈 메쉬
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> meshAsset(TEXT("SkeletalMesh'/Game/ThirdPerson/Characters/Mannequin_UE4/Meshes/SK_Mannequin.SK_Mannequin'"));
	if (meshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(meshAsset.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -90.f), FRotator(0.f, -90.f, 0.f));
	}

	// 칼
	Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Katana"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> katanaAsset(TEXT("StaticMesh'/Game/MarketplaceAssets/ARPG_Samurai/Demo/Weapon/Mesh/katana.katana'"));
	if (katanaAsset.Succeeded())
	{
		Weapon->SetStaticMesh(katanaAsset.Object);
		Weapon->SetupAttachment(GetMesh(), TEXT("katana3"));
		Weapon->SetCollisionProfileName(TEXT("NoCollision"));
	}

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
	DefaultCameraBoom->CameraLagSpeed = 10.f;

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
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 400.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 350.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 500.f;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 350.f, 0.f);

	// 공격 판정을 관리하는 컴포넌트
	CombatComp = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComp"));
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// 기본 무기, 무기 두께 설정
	CombatComp->SetupWeapon(Weapon, 5.0f);

	// 기본 무기 최대 콤보 수 설정
	MaxAttackCount = Attacks.Num() - 1;
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 타겟팅한 적 있으면 Yaw 회전
	if (bIsTargeting == true)
	{
		RotateToTarget(EnemyTarget, DeltaTime, RInterpSpeed);
		// 거리가 멀어지면 타겟 해제
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
	// TODO : 공격 가능한 상태인지 체크

	// 오토 타겟팅으로 타겟 지정
	TryAutoTargeting();

	// 스무스하게 회전
	RInterpSpeed = 5.f;

	// 공격 분기
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
	// TODO: 가드 가능한 상태인지 체크
	bIsBlocking = true;

	// ABP의 스테이트 변경
	auto animIns = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	animIns->bIsBlocking = true;

	// 이동속도 감소
	GetCharacterMovement()->MaxWalkSpeed = 200.f;
	GetCharacterMovement()->MaxAcceleration = 512.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 32.f;
}

void APlayerCharacter::StopGuard()
{
	bIsBlocking = false;

	// ABP의 스테이트 변경
	auto animIns = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	animIns->bIsBlocking = false;

	// 이동속도 초기화
	GetCharacterMovement()->MaxWalkSpeed = 350.f;
	GetCharacterMovement()->MaxAcceleration = 2048.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 500.f;
}

void APlayerCharacter::Interact()
{
	// TODO: 물건 줍기 등
}

void APlayerCharacter::Dash()
{
	// TODO: 대시 가능한 상태인지 체크

	bIsDashing = true;

	// 4방향 회피
	PerformDodge();

	// 이동속도 증가
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
}

void APlayerCharacter::StopDash()
{
	bIsDashing = false;
	// 이동속도 초기화
	GetCharacterMovement()->MaxWalkSpeed = 350.f;
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
	// TODO : 상태 체크
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
	// TODO : 상태 체크
	return true;
}

void APlayerCharacter::RotateToTarget(AActor* Target, float DeltaTime, float InterpSpeed)
{
	// 타겟팅한 적이 있는 경우
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

/* TODO: 합칠 예정
void APlayerCharacter::PerformLightAttack(int Combo)
{
	// 몽타주 재생
	if (Combo > -1)
	{
		PlayAnimMontage(Attacks[Combo]);
	}
}
*/

void APlayerCharacter::PerformLightAttack()
{
	// 몽타주 재생
	PlayAnimMontage(Attacks[AttackCount]);

	// 콤보 카운트
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

	// 몽타주 재생
	PlayAnimMontage(JumpAttacks[0]);
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
			PlayAnimMontage(Dodges[0]); // 앞으로
		}
		else if (dodgeAngle > 45.f && dodgeAngle <= 135.f)
		{
			PlayAnimMontage(Dodges[1]); // 오른쪽으로
		}
		else if (dodgeAngle > -135.f && dodgeAngle < -45.f)
		{
			PlayAnimMontage(Dodges[2]); // 왼쪽으로
		}
		else // (dodgeAngle 135.f > && dodgeAngle < -135.f)
		{
			PlayAnimMontage(Dodges[3]); // 뒤로
		}
	}
	else // 비조준 상태 회피
	{
		FRotator newRotation = FRotationMatrix::MakeFromX(GetLastMovementInputVector()).Rotator();
		SetActorRotation(newRotation);	// 방향 전환
		PlayAnimMontage(Dodges[0]);		// 앞으로
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
	// 거리 + 각도 숫자가 가장 가까운 Pawn을 EnemyTarget으로 지정
	if (SearchEnemies() == true)
	{
		ScoreEnemies();
		SetEnemyTarget();
		ClearScores();
		bIsTargeting = true;
		return true;
	}

	// 찾은 적이 없으면 false
	bIsTargeting = false;
	return false;
}

bool APlayerCharacter::SearchEnemies()
{
	// "Targeting" 채널로 트레이스
	TArray<AActor*> actorToIgnore;
	bool outValue = UKismetSystemLibrary::SphereTraceMulti(this, GetActorLocation(), GetActorLocation(), 300.f, TraceTypeQuery3, false, actorToIgnore, 
		EDrawDebugTrace::ForDuration, SearchHits, true, FColor::Red, FColor::Green, 1.f);

	// 범위 내 적을 못찾으면 false 반환
	if (outValue == false)
		return false;
	
	// 범위 내 모든 적을 찾아 배열에 추가하고 true 반환
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
		// 거리 점수 기록하기
		float dist = FVector::Distance(SearchedEnemies[i]->GetActorLocation(), GetActorLocation());
		DistanceScores.Add(dist);

		// 각도 점수 기록하기
		FVector inputVector = GetLastMovementInputVector();
		FVector direction = SearchedEnemies[i]->GetActorLocation() - GetActorLocation();
		direction.Normalize();
		float deltaRadian = FMath::Acos(FVector::DotProduct(inputVector, direction));
		float deltaDegree = FMath::RadiansToDegrees(deltaRadian);
		AngleScores.Add(deltaDegree);

		// 합계 점수 기록하기
		TotalScores.Add(DistanceScores[i] + AngleScores[i] * 4);

		// 디버그
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString::Printf(TEXT("Name:%s,    Dist:%f,    Angle:%f,    Total:%f"), 
			*SearchHits[i].GetActor()->GetName(), DistanceScores[i], AngleScores[i], TotalScores[i]));
	}
}

void APlayerCharacter::SetEnemyTarget()
{
	// 최저점 찾기
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

	// 최저점 나온 에너미를 타겟으로 지정
	EnemyTarget = SearchedEnemies[outIndex];

	// 디버그
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("%s"), *EnemyTarget->GetName()));
}

void APlayerCharacter::ClearScores()
{
	// 사용한 배열 초기화
	SearchHits.Empty();
	SearchedEnemies.Empty();
	DistanceScores.Empty();
	AngleScores.Empty();
	TotalScores.Empty();
}
