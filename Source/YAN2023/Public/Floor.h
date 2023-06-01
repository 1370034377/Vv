// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterTypes.h"

#include "Floor.generated.h"

class UNiagaraComponent;
class UBoxComponent;
class ASlashCharacter;
class ABreakableActor;
class ARoadblock;
class USoundBase;

UCLASS()
class YAN2023_API AFloor : public AActor
{
	GENERATED_BODY()
	
public:	
	AFloor();
	virtual void Tick(float DeltaTime) override;

	//地板地面
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* FloorMesh;
	
	//生成时粒子特效
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UNiagaraComponent* FloorNiagara1;

	//生成时粒子特效
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UNiagaraComponent* FloorNiagara2;

	//触发生成角色数量的墙1
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* NumberWall_1;

	//触发生成角色数量的墙2
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* NumberWall_2;

	//触发生成新Box1
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UBoxComponent* FloorBox1;

	//触发生成新Box2
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UBoxComponent* FloorBox2;

	//显示的数字
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Text1;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Text2;

	FString SectionName1 = FString("String");
	FString SectionName2 = FString("String");

	//生成墙时，+-*/的数量
	int32 Number1;
	int32 Number2;

	//区分+-*/的枚举类型
	ERule Rule1 = ERule::ER_Add;
	ERule Rule2 = ERule::ER_Add;

	//显示 +-*/数字 的模型,该数字按+-*/从小到大的顺序添加
	UPROPERTY(EditAnywhere, Category = "AAA")
	TArray <class UStaticMesh*> Meshes;

	//是否已经命中
	bool IsBoxHit = false;

	//初始化生成地板时，产生的+-*/数字
	void SpawnNumber1();
	void SpawnNumber2();

	//生成敌人
	UFUNCTION(BlueprintCallable)
	void SpawnAi();
	
	//生成敌人的数量
	UPROPERTY(EditAnywhere, Category = "AAA")
	int32 AiNumber = 3;

	//触发重叠时，更新玩家角色数量
	void UpNumber(ERule Rule, int32 Number);

	//添加一定数量的骨骼网格体
	void AddSkeletalMesh();

	//设置数字模型
	void SetRuleNumberMesh(UStaticMeshComponent* TextMeshComponent,int32 TextNumber);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Box1重叠时
	UFUNCTION()
	void OnBoxOverlap_1(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	//Box2重叠时
	UFUNCTION()
	void OnBoxOverlap_2(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	//随机生成地板
	UPROPERTY(EditAnywhere, Category = "AAA")
	TArray<TSubclassOf<class AFloor>> RandomFloor;

	//检测是否已经生成字体，若没有开始运行时，没有生成字体，则不旋转
	bool CanRotatorText = false;

	//旋转速度
	UPROPERTY(EditAnywhere,Category = "AAA")
	float RotationalSpeed = 100.f;
	//设置字体自动旋转
	void SetTextRotator(float TickTime);

	//设置生命周期时间
	UPROPERTY(EditAnywhere, Category = "AAA")
	float FloorLifeSpan = 10.f;

	//检测生成位置是否有地板
	bool BoxTrace(AActor* actor);

	//触发生成随机事件
	void RandomEvent();

	//生成武器：枪、罐子
	UPROPERTY(EditAnywhere, Category = "AAA")
	TArray<TSubclassOf<class ABreakableActor>> GuanZiQiang;
	
	ABreakableActor* MyGuanZiQiang;
	ASlashCharacter* MyCharacter;
	ARoadblock* MyRoadblock;

	//随机生成敌人
	UPROPERTY(EditAnywhere, Category = "AAA")
	TArray<TSubclassOf<class AEnemy>> RandomEnemy;

	//随机生成障碍物：锤子等
	UPROPERTY(EditAnywhere, Category = "AAA")
	TArray<TSubclassOf<class ARoadblock>> Roadblock;

	//命中时播放的音效
	UPROPERTY(EditAnywhere)
	USoundBase* HitSound;

	//按时间循环执行,计时器句柄
	FTimerHandle PatrolTimer;
	void DestroyTimer();
};
