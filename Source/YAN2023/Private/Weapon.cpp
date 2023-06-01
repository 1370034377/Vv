// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "SlashCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"   //盒体碰撞
#include "Kismet/KismetStringLibrary.h"  //射线检测追踪
#include "HitInterface.h"   //接口
#include "NiagaraComponent.h"

AWeapon::AWeapon()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box")); //创建武器检测盒体，用于攻击时检测命中
	WeaponBox->SetupAttachment(GetRootComponent());  //设置附加到根组件
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);  //设置当前无碰撞（在触发攻击蒙太奇通知才开启）
	WeaponBox->SetCollisionResponseToAllChannels(ECR_Overlap);  //设置响应碰撞通道为重叠
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);  //设置忽略自身对Pawn类型碰撞

	//发射通道追踪的起始结束点位置
	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());
	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	//绑定重叠事件
	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
}

//装备武器，将武器附加到插槽
void AWeapon::Equip(USceneComponent* Inparent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	//将该武器的拥有者设置为自身
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);

	//将武器附加到插槽名称
	AttachMeshToSocket(Inparent, InSocketName);
	
	//将当前枚举切换为已装备类型
	ItemState = EItemState::EIS_Equipped;
	
	//播放元声音
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
	}
	//关闭碰撞
	if (Sphere)
	{
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	//关闭特效
	if (EmbersEffect)
	{
		EmbersEffect->Deactivate();
	}
}

void AWeapon::AttachMeshToSocket(USceneComponent* Inparent, const FName& InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	//将武器附加到插槽
	ItemMesh->AttachToComponent(Inparent, TransformRules, InSocketName);
}

//���忪ʼ�ص��¼�,���ǻ����麯��
void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

}

//�����ص��¼�
void AWeapon::OnComponentEndOver(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnComponentEndOver(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

}

//碰撞盒子Box:重叠事件
void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();
	
	//在检测追踪时,忽略的Actor数组,可以将不需要被检测到对象放入该数组
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}

	//命中结果
	FHitResult BoxHit;

	//按通道进行盒体追踪
	UKismetSystemLibrary::BoxTraceSingle
	(
		this,
		Start,
		End,
		FVector(5.f, 5.f, 5.f),
		BoxTraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		BoxHit,
		true
	);

	if (BoxHit.GetActor())
	{
		//造成伤害事件
		UGameplayStatics::ApplyDamage
		(
			BoxHit.GetActor(),
			Damage,
			GetInstigator()->GetController(),
			this,
			UDamageType::StaticClass()
		);

		//如果命中结果存在且是有命中接口，则发出命中位置消息
		IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
		if (HitInterface)
		{
			HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint);
		}
		//已命中的Actor不再重复触发命中
		IgnoreActors.AddUnique(BoxHit.GetActor());

		//命中时，将变量传给蓝图，可在蓝图调用创建力场事件
		CreateFields(BoxHit.ImpactPoint);
	}
}
