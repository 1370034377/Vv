// Fill out your copyright notice in the Description page of Project Settings.


#include "SlashAnimInstance.h"


#include "SlashCharacter.h"    //��ȡ��ҽ�ɫ
#include "GameFramework/CharacterMovementComponent.h"   //��ȡ��ɫ�ƶ����
#include "Kismet/KismetMathLibrary.h"     //��ѧ��


//��ʼ������ȡ��ҽ�ɫ���ƶ����
void USlashAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	SlashCharacter = Cast<ASlashCharacter>(TryGetPawnOwner());

	if (SlashCharacter)
	{
		SlashCharacterMovement = SlashCharacter->GetCharacterMovement();

	}
}

//���¶����¼�
void USlashAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (SlashCharacterMovement)
	{
		//���ٶ���xy��������
		GroundSpeed = UKismetMathLibrary::VSizeXY(SlashCharacterMovement->Velocity);

		//�Ƿ�Ӧ������
		IsFalling = SlashCharacterMovement->IsFalling();

		CharacterState = SlashCharacter->GetCharacterState();
	}
}
