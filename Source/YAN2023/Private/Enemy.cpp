// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"  //移动组件
#include "Perception/PawnSensingComponent.h"  //感知Ai组件
#include "AttributeComponent.h"
#include "HealthBarComponent.h"
#include "SlashCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "Camera/CameraShakeBase.h"


// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//设置自身的碰撞类型为世界动态
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	//设置碰撞响应通道
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);     //生成重叠事件
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	//添加血条UI控件
	HealthBarWidget=CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	//设置控制旋转
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	//感知Ai组件
	//PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Pawn Sensing"));
	//PawnSensing->SightRadius = 4000.f;
	//PawnSensing->SetPeripheralVisionAngle(45.f);

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	PatrolTarget = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	//获取AI控制器
	EnemyController = Cast<AAIController>(GetController());
	
	//默认生命值UI条不可见
	if (HealthBarWidget){HealthBarWidget->SetVisibility(false);}
	//函数绑定到事件
	//if (PawnSensing){PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);}

	//延迟后执行
	GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, 0.5f, false);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PatrolTarget && !Isfested)
	{
		float Size = (PatrolTarget->GetActorLocation() - GetActorLocation()).Size();
		//如果在攻击范围内，则自爆
		InfestedTerran(Size <= AttackRadius);
		if (Size > 20000.f)
		{
			//清除定时器
			GetWorldTimerManager().ClearTimer(PatrolTimer);
			Destroy();
		}
	}
}

//检测巡逻目标
void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		//在范围内随机选择一个目标
		PatrolTarget = ChoosePatrolTarget();

		const float WaitTime = FMath::RandRange(WaitMin, WaitMax);

		//延迟后，执行绑定函数：移动到目标位置
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);
	}
}

//检测战斗目标
void AEnemy::CheckCombatTarget()
{
	//计算目标位置是否在范围内
	if (!InTargetRange(CombatTarget, CombatRadius))
	{
		//不在攻击检测范围内
		CombatTarget = nullptr;
		if (HealthBarWidget)
		{
			//设置生命值UI不可见
			HealthBarWidget->SetVisibility(false);
		}
		EnemyState = EEnemyState::EES_Patrolling;
		GetCharacterMovement()->MaxWalkSpeed = 125.f;
		MoveToTarget(PatrolTarget);
	}
	else if (!InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Chasing)
	{
		//在检测可攻击的范围内，追踪状态
		EnemyState = EEnemyState::EES_Chasing;
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		MoveToTarget(CombatTarget);
	}
	else if (InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Attacking)
	{
		//在攻击范围内，进入攻击状态
		EnemyState = EEnemyState::EES_Attacking;
	}
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

//播放死亡动画
void AEnemy::Die()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
		const int32 Selection = FMath::RandRange(0, 1);
		FName SectionName = FName();
		//随机播放一个死亡动画
		switch (Selection)
		{
		case 0:
			SectionName = FName("Death_01");
			DeathPose = EDeathPose::EDP_Death1;
			break;
		case 1:
			SectionName = FName("Death_02");
			DeathPose = EDeathPose::EDP_Death2;
			break;
		default:
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);
	}
	//关闭生命值UI
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}
	//关闭碰撞
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//设置生命周期为3s
	SetLifeSpan(3.f);
}

//计算目标位置是否在导航范围内
bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr)return false;
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	return DistanceToTarget <= Radius;
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController || Target)
	{
		EnemyController->MoveTo(Target);
	}
}

//选择移动的目标
AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> ValidTargets;
	for (auto Target : PatrolTargets)
	{
		//非当前的有效目标
		if (Target != PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}
	//获取数组长度
	const int32 NumPatrolTargets = ValidTargets.Num();
	if (NumPatrolTargets > 0)
	{
		//获取数组中随机一个
		const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);
		return ValidTargets[TargetSelection];

	}
	return nullptr;
}

////需要绑定到Ai感知组件的看见事件
//void AEnemy::PawnSeen(APawn* SeenPawn)
//{
//	if (EnemyState == EEnemyState::EES_Chasing) return;
//	//通过标签检测，发现玩家时，开始追踪目标
//	if (SeenPawn->ActorHasTag(FName("SlashCharacter")))
//	{
//		if (EnemyState != EEnemyState::EES_Attacking)
//		{
//			EnemyState = EEnemyState::EES_Chasing;
//			MoveToTarget(CombatTarget);
//
//		}
//		
//		//清除定时器
//		GetWorldTimerManager().ClearTimer(PatrolTimer);
//		GetCharacterMovement()->MaxWalkSpeed = 300.f;
//		CombatTarget = SeenPawn;
//	}
//}

//按时间循环执行移动
void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget); 		

}

//接口:被命中时触发
void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
	if (HealthBarWidget)
	{	//设置生命值UI条可见
		HealthBarWidget->SetVisibility(true);
	}

	if (Attributes && Attributes->IsAlive())
	{	//如果生命值大于0，则播放受击动画
		//由攻击方向播放不同蒙太奇
		DirectionalHitReact(ImpactPoint);
	}
	else
	{
		//播放死亡蒙太奇
		Die();
	}

	//打击音效
	if (HitSound)
	{	
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}

	//打击特效
	if (HitParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation
		(
			GetWorld(),
			HitParticles,
			ImpactPoint
		);
	}
}

//应用伤害事件
float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Attributes && HealthBarWidget)
	{
		//传递伤害到属性组件
		Attributes->ReceiveDamage(DamageAmount);
		
		//更新生命条UI
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
	//设定攻击目标
	CombatTarget = EventInstigator->GetPawn();

	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	MoveToTarget(CombatTarget);

	return DamageAmount;
}

//子弹伤害事件
float AEnemy::BulletDamage(float Damage)
{
	if (Attributes && Attributes->IsAlive())
	{
		if (Attributes && HealthBarWidget)
		{
			//设置生命值UI条可见
			HealthBarWidget->SetVisibility(true);
			//传递伤害到属性组件
			Attributes->ReceiveDamage(Damage);
			//更新生命条UI
			HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
			if(!Attributes->IsAlive())
			{
				//设置生命值UI条不可见
				HealthBarWidget->SetVisibility(false);
			}
		}
		if (HitSound){UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());}	                //打击音效
		if (HitParticles){UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, GetActorLocation());}	//打击特效
	}
	else
	{
		//已命中并死亡
		Isfested = true;
		NewDie();
	}
	return Damage;
}

//敌人自爆攻击
void AEnemy::InfestedTerran(bool IsHit)
{
	if (IsHit)
	{
		//已命中，自爆
		Isfested = true;

		ASlashCharacter* MyCharacter = Cast<ASlashCharacter>(PatrolTarget);
		if (MyCharacter && MyCharacter->MeshNumber > 1)
		{
			MyCharacter->MeshNumber--;
			const int32 range = FMath::RandRange(0, MyCharacter->SkeletalMeshComponent.Num()-1);
			if (MyCharacter->SkeletalMeshComponent[range])
			{
				MyCharacter->SkeletalMeshComponent[range]->DestroyComponent();
				MyCharacter->SkeletalMeshComponent.RemoveAt(range);
			}
			else
			{
				MyCharacter->LoopUpMesh();
				MyCharacter->SkeletalMeshComponent[FMath::RandRange(0, MyCharacter->SkeletalMeshComponent.Num() - 1)]->DestroyComponent();
				MyCharacter->SkeletalMeshComponent.RemoveAt(range);
			}
		}
		NewDie();
	}
}

void AEnemy::NewDie()
{
	//自爆音效
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
	}

	//自爆特效
	if (HitParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation
		(
			GetWorld(),
			HitParticles,
			GetActorLocation(),
			GetActorRotation(),
			FVector(3.f)
		);
	}

	//像机晃动
	if (CameraShake)
	{
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), CameraShake, GetActorLocation(), 0.f, 3000.f);
	}

	//关闭生命值UI
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}

	//关闭碰撞
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->MaxWalkSpeed = 0.f;
	
	GetMesh()->SetVisibility(false);

	//清除定时器
	GetWorldTimerManager().ClearTimer(PatrolTimer);
	
	//设置生命周期
	SetLifeSpan(0.2f);
}