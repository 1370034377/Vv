// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

class UParticleSystemComponent;
class UNiagaraComponent;
class UBoxComponent;
class UAudioComponent;
class UProjectileMovementComponent;

UCLASS()
class YAN2023_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//重叠事件
	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//伤害值，攻击力
	UPROPERTY(EditAnywhere, Category = "AAA")
	float Damage = 20.f;

private:
	//命中音效
	UPROPERTY(EditAnywhere)
	USoundBase* AttackSound;

	//命中音效
	UPROPERTY(EditAnywhere)
	UParticleSystem* BulletHitNiagara;

	//枪发射的特效
	UPROPERTY(EditAnywhere)
	UParticleSystemComponent* AttackNiagara1;

	//枪发射的特效
	UPROPERTY(EditAnywhere)
	UParticleSystemComponent* AttackNiagara2;

	//枪发射音效
	UPROPERTY(EditAnywhere)
	UAudioComponent* AttackAudio;

	//子弹抛射物组件
	UPROPERTY(EditAnywhere)
	UProjectileMovementComponent* ProjectileMovementComponent;

	//触发生成新Box1
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* BulletBox;
};
