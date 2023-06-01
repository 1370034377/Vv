
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class USphereComponent;

//枚举：是否为装备在角色上
enum class EItemState : uint8
{
	//悬停在空中
	EIS_Hovering,
	//已装备
	EIS_Equipped
};

UCLASS()
class YAN2023_API AItem : public AActor
{
	GENERATED_BODY()

public:
	AItem();

protected:

	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RunningTime;

	void SetCollision_No();
public:

	virtual void Tick(float DeltaTime) override;


	UFUNCTION(BlueprintPure)
	float TransformedSin();


	UFUNCTION(BlueprintPure)
	float TransformedCos();

public:

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* Sphere;

	//粒子特效
	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* EmbersEffect;

	UFUNCTION()
	void XuanZhuan(float DeltaTime);

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnComponentEndOver(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//是否为装备在角色上
	EItemState ItemState = EItemState::EIS_Hovering;

	template<typename T>
	T Avg(T First, T Second);

};

template<typename T>
inline T AItem::Avg(T First, T Second)
{
	return T(First + Second) / 2;
}
