// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "SlashCharacter.h"
#include "NiagaraComponent.h"

//�Զ���꣬���Ƶ�����
#define DRAW_SPHERE(Location) if(GetWorld()) DrawDebugSphere(GetWorld(),Location,10.f,12,FColor::Red,false,-1.f);
//������
#define DRAW_Xian(StartLocation,EndLocation) if(GetWorld()) DrawDebugLine(GetWorld(), StartLocation, EndLocation,FColor::Red,false,-1.f,0,1.f);

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	RootComponent = ItemMesh;
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());
	
	EmbersEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Embers Effect"));
	EmbersEffect->SetupAttachment(GetRootComponent());
}

void AItem::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnComponentEndOver);
}

//设置响应碰撞通道为忽略
void AItem::SetCollision_No()
{
	if (ItemMesh && Sphere)
	{
		ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Sphere->DestroyComponent();
		if (EmbersEffect) { EmbersEffect->DestroyComponent(); }
	}
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//如果没有装备武器，则可在空中旋转
	if (ItemState == EItemState::EIS_Hovering)
	{
		XuanZhuan(DeltaTime);
	}
	
}

float AItem::TransformedSin()
{
	return FMath::Sin(RunningTime);
}

float AItem::TransformedCos()
{
	return FMath::Cos(RunningTime);
}

//���λ��ƫ��,��ת
void AItem::XuanZhuan(float DeltaTime)
{
	RunningTime += DeltaTime;
	AddActorWorldOffset(FVector(TransformedCos(), TransformedSin(), 0.f));
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(OtherActor);
	
	if (SlashCharacter)
	{
		SlashCharacter->SetOverlappingItem(this);
	}
}

void AItem::OnComponentEndOver(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(OtherActor);
	if (SlashCharacter)
	{
		SlashCharacter->SetOverlappingItem(nullptr);
	}
}

