
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HitInterface.h"   //命中接口

#include "BaseCharacter.generated.h"

class AWeapon;
class UAnimMontage;
class UAttributeComponent;//自定义属性组件

UCLASS()
class YAN2023_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:

	ABaseCharacter();

	virtual void Tick(float DeltaTime) override;

	//设置武器Box碰撞，蓝图可调用
	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	//属性组件
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

protected:

	virtual void BeginPlay() override;

	virtual void Attack();

	//播放死亡蒙太奇
	virtual void Die();

	//播放命中时的蒙太奇
	void PlayHitReactMontage(const FName& SectionName);

	//由攻击方向播放不同蒙太奇
	void DirectionalHitReact(const FVector& ImpactPoint);

	//播放攻击蒙太奇
	virtual void PlayAttackMontage();

	//是否能攻击
	virtual bool CanAttack();

	//是否攻击结束
	UFUNCTION(BlueprintCallable)
	virtual	void AttackEnd();

	//持有的武器
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AWeapon* EquippedWeapon;

	//攻击蒙太奇
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* AttackMontage;

	//播放命中时的蒙太奇
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* HitReactMontage;

	//播放死亡蒙太奇
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* DeathMontage;

	//命中时播放的音效
	UPROPERTY(EditAnywhere)
	USoundBase* HitSound;

	//命中特效
	UPROPERTY(EditAnywhere, Category = VisualEffects)
	UParticleSystem* HitParticles;

};
