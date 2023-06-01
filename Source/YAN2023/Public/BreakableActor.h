// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HitInterface.h"  //接口
#include "BreakableActor.generated.h"

class UGeometryCollectionComponent;   //破碎几何体组件
class UCapsuleComponent;
class ATreasure;

UCLASS()
class YAN2023_API ABreakableActor : public AActor, public IHitInterface
{
	GENERATED_BODY()
	
public:	
	ABreakableActor();

	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	//破碎几何体组件
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UGeometryCollectionComponent* GeometryCollection;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UCapsuleComponent* Capsule;

	UFUNCTION(BlueprintCallable)
	void SpawnTreasure(const FVector& SpawnLocation);

public:	
	//添加接口实现
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

private:
	//生成的道具
	UPROPERTY(EditAnywhere, Category = "Breakable Properties")
	TArray<TSubclassOf<class ATreasure>> TreasureClasses;

	ATreasure* MyTreasure;
	bool bBroken = false;
};
