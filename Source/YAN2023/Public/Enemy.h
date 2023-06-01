// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "CharacterTypes.h"

#include "Enemy.generated.h"

class UHealthBarComponent;  //自定义生命值组件
class UPawnSensingComponent; //感知组件
class UCameraShakeBase;   //摄像机晃动基础

UCLASS()
class YAN2023_API AEnemy : public ABaseCharacter
{	GENERATED_BODY()

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	//检测巡逻目标
	void CheckPatrolTarget();

	//检测战斗目标
	void CheckCombatTarget();

	//覆盖接口
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

	//伤害事件
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	//播放死亡蒙太奇
	virtual void Die() override;

	//播放对应的死亡动画姿势,枚举
	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose = EDeathPose::EDP_Alive;
	
	//到目标的范围半径
	bool InTargetRange(AActor* Target, double Radius);

	//使AI移动到目标
	void MoveToTarget(AActor* Target);

	//随机选择移动的目标
	AActor* ChoosePatrolTarget();

	//看见玩家后，Ai执行的操作
	//UFUNCTION()
	//void PawnSeen(APawn* SeenPawn);

private:

	//生命条UI组件
	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	//感知Ai组件
	//UPROPERTY(VisibleAnywhere)
	//UPawnSensingComponent* PawnSensing;

	//攻击目标
	UPROPERTY()
	AActor* CombatTarget;

	//检测进入攻击状态的追踪半径
	UPROPERTY(EditAnywhere)
	double CombatRadius = 500.f;

	//攻击半径
	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.f;

	//寻路结束的可接受半径
	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;

	//导航巡逻的目标
	UPROPERTY(EditAnywhere, Category = "AI Navigation", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	AActor* PatrolTarget;
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	//等待停留的时间（最大最小值）
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMin = 5.f;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMax = 10.f;

	//按时间循环执行,计时器句柄
	FTimerHandle PatrolTimer;
	void PatrolTimerFinished();

	//向前声明Ai控制器变量
	UPROPERTY()
	class AAIController* EnemyController;

	//敌人状态，默认为巡逻中
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> WeaponClass;


//******--------------------------************//

private:
	//自爆攻击
	void InfestedTerran(bool IsHit);
	
	//是否命中
	bool Isfested = false;
	
	//死亡效果
	void NewDie();
	
public:
	//被子弹命中
	float BulletDamage(float Damage);

	//像机晃动
	UPROPERTY(EditAnywhere, Category = "AAA")
	TSubclassOf<class UCameraShakeBase> CameraShake;
}; 
