// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "HitInterface.h"   //命中接口
#include "Treasure.generated.h"

/**
 * 
 */
UCLASS()
class YAN2023_API ATreasure : public AItem, public IHitInterface
{
	GENERATED_BODY()

protected:
	//覆盖基类item的该函数
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	//碰撞道具的声音
	UPROPERTY(EditAnywhere, Category = Sounds)
	USoundBase* PickupSound;	
	
	//枪的声音
	UPROPERTY(EditAnywhere, Category = Sounds)
	USoundBase* AttackSound;
	void PlaySound();

	//枪的特效
	UPROPERTY(EditAnywhere, Category = "AAA")
	class UParticleSystem* Particle_1;
	void PlayParticle_1();

	//获得的金币数量
	UPROPERTY(EditAnywhere, Category="AAA")
	int32 Gold;

	//覆盖接口
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

public:
	//枪发射的子弹
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AAA")
	TSubclassOf<class ABullet> AttackBullet;
};
