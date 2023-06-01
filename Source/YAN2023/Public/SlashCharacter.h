// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "CharacterTypes.h"

#include "SlashCharacter.generated.h"


UCLASS()
class YAN2023_API ASlashCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASlashCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	//���ɱ�
	UPROPERTY(VisibleAnywhere)
		class USpringArmComponent* SpringArm;

	//���
	UPROPERTY(VisibleAnywhere)
		class UCameraComponent* ViewCamera;

	//收起、装备武器
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	class UAnimMontage* EquipMontage;


	//����һ�������࣬���ڵ����ڼ���ص�ʱ�洢������
	//�����⵽��ҽ�ɫ����Ѹõ��ߴ�����ҽ�ɫ���������������С�
	UPROPERTY(VisibleInstanceOnly)
		class AItem* OverlappingItem;
public:    //����������������ȡ���߱�����
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }

	//内联函数，仅获取玩家状态
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }

protected:
	//�ƶ�����
	void MoveForward(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void MoveRight(float Value);

	//按E键捡起并装备上武器
	void EKeyPressed();
	
	//攻击模式
	virtual void Attack() override;

	//播放攻击蒙太奇
	virtual void PlayAttackMontage() override;

	//结束攻击
	virtual void AttackEnd() override;

	//是否能攻击
	virtual bool CanAttack() override;

	//切换武器蒙太奇
	void PlayEquipMontage(const FName& SectionName);
	bool CanDisarm();//是否能解除武器
	bool CanArm();//是否能从背包再装备武器

	UFUNCTION(BlueprintCallable)
	void Disarm();//将武器附加到背部插槽
	UFUNCTION(BlueprintCallable)
	void Arm();  //将武器附加到手上
	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

private:
	//�Ƿ�װ��������ö��
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	//攻击状态，枚举
	EActionState ActionState = EActionState::EAS_Unoccupied;






/***********--------------------------------*****************/

	//新生成角色部分
private:

	//生成骨骼网格体模型的排列位置
	FTransform MeshTransform;
	//生成间距
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float JianJu = 100.f;
	//生成骨骼位置的数组
	//TArray<FVector> LocArray;
	void InitializationLocation(int MeshNum, TArray<FVector> &LocArray);
public:

	//添加骨骼网格体
	UFUNCTION(BlueprintImplementableEvent)   //蓝图中可实现
	void AddSkeletalComponent(const FTransform& Transform);

	//角色数量
	UPROPERTY(BlueprintReadWrite)
	int32 MeshNumber = 1;

	//角色的骨骼网格体
	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<USkeletalMeshComponent>> SkeletalMeshComponent;

	//生成骨骼网格体模型的排列位置
	void LoopUpMesh();

	//附加枪到角色
	void AddQiang(AActor* Qiang);
	class ATreasure* JiQiang;

	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<AActor>> QiangMeshArray;
/***********--------------------------------*****************/
};
