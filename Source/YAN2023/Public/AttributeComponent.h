
//角色属性组件

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class YAN2023_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttributeComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:

	//生命值
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Health;

	//最大生命值
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxHealth;

public:
	//伤害值的变化
	void ReceiveDamage(float Damage);
	//获取当前生命值百分比
	float GetHealthPercent();

	bool IsAlive();
};
