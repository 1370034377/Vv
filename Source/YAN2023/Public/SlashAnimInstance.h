// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterTypes.h"    //�Ƿ�װ��������ö��

#include "SlashAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class YAN2023_API USlashAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	

public:

	//�麯������ʼ�����������Ǹ���
	virtual void NativeInitializeAnimation() override;
	
	//���¶����¼������ǻ���ĸ��麯��
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	//��ǰ��������
	UPROPERTY(BlueprintReadOnly)
	class ASlashCharacter* SlashCharacter;
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	class UCharacterMovementComponent* SlashCharacterMovement;

	//�ƶ��ٶ�
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float GroundSpeed;

	//�Ƿ�����
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool IsFalling;

	//是否手持武器，该枚举从玩家角色获取
	UPROPERTY(BlueprintReadOnly, Category = "Movement | Character State")
	ECharacterState CharacterState;
};
