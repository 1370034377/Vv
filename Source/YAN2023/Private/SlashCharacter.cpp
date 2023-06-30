// Fill out your copyright notice in the Description page of Project Settings.


#include "SlashCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Item.h"
#include "Weapon.h"
#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"   //胶囊体
#include "Treasure.h"
//#include "HitInterface.h"   //命中接口

// Sets default values
ASlashCharacter::ASlashCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	//��ӵ��ɱ�
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.f;

	//������
	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("View Camera"));
	ViewCamera->SetupAttachment(SpringArm);

	//���ÿ������0
	AutoPossessPlayer = EAutoReceiveInput::Player0;

}

// Called when the game starts or when spawned
void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();

	//添加标签
	Tags.Add(FName("SlashCharacter"));
}

// Called every frame
void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//绑定轴按键
	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ASlashCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("Turn"), this, &ASlashCharacter::Turn);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &ASlashCharacter::LookUp);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &ASlashCharacter::MoveRight);
	//绑定Action按键
	PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed,this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(FName("Equip"), IE_Pressed, this, &ASlashCharacter::EKeyPressed);
	PlayerInputComponent->BindAction(FName("Attack"), IE_Pressed, this, &ASlashCharacter::Attack);
}

//输入移动
void ASlashCharacter::MoveForward(float Value)
{
	//如果不在闲置状态，则禁止输入移动
	if (ActionState != EActionState::EAS_Unoccupied) return;
	if (Controller && (Value != 0.f))
	{
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
		
		//��ȡ������Yaw������ת�ĵ�λ����
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
		
	}
}

//�����ƶ�
void ASlashCharacter::MoveRight(float Value)
{
	//如果不在空闲状态，则禁止输入移动
	if (ActionState != EActionState::EAS_Unoccupied) return;
	if (Controller && (Value != 0.f))
	{
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
		
		//��ȡ������Yaw������ת�ĵ�λ����
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		
		//����ƶ�����
		AddMovementInput(Direction, Value);
	}
}

//�����ת����
void ASlashCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

//������ת��
void ASlashCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

//按E键捡起并装备上武器
void ASlashCharacter::EKeyPressed()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)   //如果重叠则按E键附加到手
	{
		//调用Weapon武器里面的Equip函数进行附加到骨骼
		OverlappingWeapon->Equip(GetMesh(), FName("WuQi"),this,this);
		
		//切换为单手武器状态
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
		OverlappingItem = nullptr;
		EquippedWeapon = OverlappingWeapon;
	}
	else
	{	
		//如果不在攻击状态并且手上有装备
		if (CanDisarm())
		{
			PlayEquipMontage(FName("UnEquip"));
			CharacterState = ECharacterState::ECS_Unequipped;
			ActionState = EActionState::EAS_EquippingWeapon;
		}
		else
		{
			if (CanArm())
			{
				PlayEquipMontage(FName("Equip"));
				CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
				ActionState = EActionState::EAS_EquippingWeapon;
			}
		}
	}
}

//攻击状态
void ASlashCharacter::Attack()
{
	//如果手上没有武器并且不在攻击状态，则设置当前为攻击状态, 播放攻击蒙太奇
	if (CanAttack())
	{
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}

	for (auto Qiang_i : QiangMeshArray)
	{
		if (Qiang_i)
		{
			//如果命中结果存在且是有命中接口，则发出命中位置消息,该接口继承基类.h文件
			IHitInterface* HitInterface = Cast<IHitInterface>(Qiang_i);
			if (HitInterface)
			{
				HitInterface->Execute_GetHit(Qiang_i, GetActorLocation());   //执行接口，运行开枪功能
			}
		}
	}
}

//随机播放一个攻击蒙太奇
void ASlashCharacter::PlayAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage);
		const int32 Selection = FMath::RandRange(0, 1);
		FName SectionName = FName();
		switch (Selection)
		{
		case 0:
			SectionName = FName("b1");
			break;
		case 1:
			SectionName = FName("b2");
			break;
		default:
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
	}
}

//切换回默认站姿
void ASlashCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

//是否能攻击
bool ASlashCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

//切换武器蒙太奇
void ASlashCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

//是否能切换武器
bool ASlashCharacter::CanDisarm()
{
	return ActionState == EActionState::EAS_Unoccupied && 
		CharacterState != ECharacterState::ECS_Unequipped;
}

//是否能从背包再装备武器
bool ASlashCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_Unequipped && EquippedWeapon;
}

void ASlashCharacter::Disarm()
{
	if (EquippedWeapon)
	{
		//将武器附加到插槽
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void ASlashCharacter::Arm()
{
	if (EquippedWeapon)
	{
		//将武器附加到插槽
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("WuQi"));
	}
}

void ASlashCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

//生成骨骼网格体模型的排列位置
void ASlashCharacter::InitializationLocation(int MeshNum, TArray<FVector>& LocArray)
{
	MeshTransform = GetMesh()->GetRelativeTransform();
	FVector Loc = MeshTransform.GetLocation();  //计算位置
	const FVector Loc2 = Loc;  //原固定位置
	int32 MaxNumber = MeshNum;	//最大循环生成点数量

	if (MaxNumber < 9)
	{
		MaxNumber = 3;
	}
	else
	{
		MaxNumber = sqrt(MaxNumber) + 1;
	}
	for (size_t i = 0; i < MaxNumber; i++)
	{
		if (i < MaxNumber * 0.5f) { Loc.Y = Loc2.Y - i * JianJu; }
		else { Loc.Y = Loc2.Y + (MaxNumber - i) * JianJu; }

		for (size_t j = 0; j < MaxNumber; j++)
		{
			if (j < MaxNumber * 0.5f) { Loc.X = Loc2.X - j * JianJu; }
			else { Loc.X = Loc2.X + (MaxNumber - j) * JianJu; }
			LocArray.Add(Loc);
		}
	}
}

//更新骨骼网格体模型数量
void ASlashCharacter::LoopUpMesh()
{
	TArray<FVector> LocArray;
	InitializationLocation(MeshNumber, LocArray);
	for (auto SkeletaComponent : SkeletalMeshComponent)
	{
		if (!SkeletaComponent) return;
		SkeletaComponent->DestroyComponent();
	}
	SkeletalMeshComponent.Empty();

	for (int32 i = 1; i < MeshNumber; i++)
	{
		if (LocArray.Num() - 1 < i) return;
		MeshTransform.SetLocation(LocArray[i]);
		AddSkeletalComponent(MeshTransform);
	}
}

//将枪模型附加到角色
void ASlashCharacter::AddQiang(AActor* Qiang)
{
	JiQiang = Cast<ATreasure>(Qiang);
	if (JiQiang == nullptr) return;
	QiangMeshArray.Add(JiQiang);
	//附加到组件所提供的规则
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	UStaticMeshComponent* QiangMesh = JiQiang->ItemMesh;

	if (SkeletalMeshComponent.Num() > 0)
	{
		QiangMesh->AttachToComponent(SkeletalMeshComponent[FMath::RandRange(0, SkeletalMeshComponent.Num() - 1)], TransformRules, TEXT("Qiang"));
	}
	else
	{
		QiangMesh->AttachToComponent(GetMesh(), TransformRules, TEXT("Qiang"));
	}
}
