// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"


class USoundBase;   //声音基类
class UBoxComponent;

/**
 * 
 */
UCLASS()
class YAN2023_API AWeapon : public AItem
{
	GENERATED_BODY()
	
public:
	AWeapon();

	//装备武器
	void Equip(USceneComponent* Inparent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);

	//将武器附加到插槽
	void AttachMeshToSocket(USceneComponent* Inparent, const FName& InSocketName);

	//数组存放攻击命中时的Actor，不进行重复触发命中
	TArray<AActor*> IgnoreActors;

protected:
	//���忪ʼ�ص��¼�,���ǻ����麯��
		virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	//�����ص��¼�
		virtual void OnComponentEndOver(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

		virtual void BeginPlay() override;

	//武器重叠时
		UFUNCTION()
		void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

		//创建力场事件
		UFUNCTION(BlueprintImplementableEvent)  //将变量传给蓝图，可在蓝图调用创建力场事件
		void CreateFields(const FVector& FieldLocation);

private:

	//注意： 该类为插件  元声音  的基类
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	USoundBase* EquipSound;

	//声明一个武器攻击时碰撞检测BOX
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UBoxComponent* WeaponBox;

	//发射通道追踪的起始结束点位置
	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceStart;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceEnd;

	//伤害值，攻击力
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Damage = 20.f;

public:
	//获取私有变量武器碰撞盒体
	FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox; }
};
