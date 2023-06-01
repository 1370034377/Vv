// Fill out your copyright notice in the Description page of Project Settings.


#include "BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"  //用于破碎的几何体组件
#include "Treasure.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ABreakableActor::ABreakableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//创建破碎几何体组件
	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("Geometry Collection Component"));
	SetRootComponent(GeometryCollection);
	//设置开启重叠事件
	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	//添加胶囊体组件
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(GetRootComponent());
	//设置忽略除Pawn外所有碰撞
	Capsule->SetCollisionResponseToAllChannels(ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

}

// Called when the game starts or when spawned
void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableActor::SpawnTreasure(const FVector& SpawnLocation)
{
	UWorld* World = GetWorld();
	if (World && TreasureClasses.Num() > 0)
	{
		//随机一个整数
		const int32 Selection = FMath::RandRange(0, TreasureClasses.Num() - 1);

		//随机生成一个Actor
		MyTreasure = World->SpawnActor<ATreasure>(TreasureClasses[Selection], SpawnLocation, GetActorRotation());
	}
}

//实现接口
void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint)
{
	if (bBroken) return;
	bBroken = true;
	FVector Location = GetActorLocation();
	Location.Z += 35.f;
	SpawnTreasure(Location);
}