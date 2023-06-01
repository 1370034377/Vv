// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Roadblock.generated.h"

/**
 * 
 */
UCLASS()
class YAN2023_API ARoadblock : public AItem
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;


	//旋转模型
	void SetRoadblockRotator(float TickTime);

	//旋转速度
	UPROPERTY(EditAnywhere, Category = "AAA")
	float MeshSpeed = 100.f;

	//锤子的挥动声音
	UPROPERTY(EditAnywhere)
	USoundBase* RoadblockSound;
	bool IsSound = true;
};	

