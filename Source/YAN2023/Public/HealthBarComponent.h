
//在玩家角色头顶显示
//生命值UI进度条

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthBarComponent.generated.h"

/**
 * 
 */
UCLASS()
class YAN2023_API UHealthBarComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	//设置生命值
	void SetHealthPercent(float Percent);

private:
	//用户控件界面
	UPROPERTY()
	class UHealthBar* HealthBarWidget;
};
