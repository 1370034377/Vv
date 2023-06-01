
#include "Floor.h"
#include "SlashCharacter.h"
#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"  //射线检测追踪
#include "DrawDebugHelpers.h"	//绘制调试线
#include "SlashCharacter.h"
#include "Enemy.h"
#include "BreakableActor.h"    //罐子打破生成道具、枪、金币等
#include "Roadblock.h"   //障碍物


#define DRAW_SPHERE(Location) if(GetWorld()) DrawDebugSphere(GetWorld(),Location,100.f,12,FColor::Red,false,5.f);

AFloor::AFloor()
{
	PrimaryActorTick.bCanEverTick = true;

	FloorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorMeshComponent"));
	RootComponent = FloorMesh;

	NumberWall_1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NumberWall_1"));
	NumberWall_1->SetupAttachment(GetRootComponent());

	NumberWall_2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NumberWall_2"));
	NumberWall_2->SetupAttachment(GetRootComponent());

	FloorBox1 = CreateDefaultSubobject<UBoxComponent>(TEXT("FloorBox1"));
	FloorBox1->SetupAttachment(NumberWall_1);

	FloorBox2 = CreateDefaultSubobject<UBoxComponent>(TEXT("FloorBox2"));
	FloorBox2->SetupAttachment(NumberWall_2);

	NumberWall_1->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	NumberWall_2->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Text1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Text1"));
	Text1->SetupAttachment(NumberWall_1);
	Text1->SetRelativeScale3D(FVector(20.f));

	Text2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Text2"));
	Text2->SetupAttachment(NumberWall_2);
	Text2->SetRelativeScale3D(FVector(20.f));

	FloorNiagara1 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FloorNiagara1"));
	FloorNiagara1->SetupAttachment(Text1);
	FloorNiagara2 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FloorNiagara2"));
	FloorNiagara2->SetupAttachment(Text2);
}

void AFloor::BeginPlay()
{
	Super::BeginPlay();

	//生成数字
	SpawnNumber1();
	SpawnNumber2();

	//绑定重叠事件
	FloorBox1->OnComponentBeginOverlap.AddDynamic(this, &AFloor::OnBoxOverlap_1);
	FloorBox2->OnComponentBeginOverlap.AddDynamic(this, &AFloor::OnBoxOverlap_2);
	
	//生成音效
	if (HitSound){UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());}
	
	//生成随机事件
	RandomEvent();
}

void AFloor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CanRotatorText){SetTextRotator(DeltaTime);}
}

void AFloor::SetTextRotator(float DeltaTime)
{
	if (Text1 && Text2)
	{
		if (!IsBoxHit)
		{
			//设置字体模型转动
			float RunningTime = 0.f;
			RunningTime += DeltaTime;
			FRotator TextRotatorMan(FRotator(0.f, FMath::Sin(RunningTime) * RotationalSpeed, 0.f));
			FRotator TextRotatorKuai(FRotator(0.f, (FMath::Abs(FMath::Sin(RunningTime)) + 0.2) * RotationalSpeed, 0.f));
			if (Text1->GetRelativeRotation().Yaw <= 60.f && Text1->GetRelativeRotation().Yaw > (-45.f))
			{
				Text1->AddLocalRotation(TextRotatorMan);
				Text2->AddLocalRotation(TextRotatorMan);
			}
			else
			{
				Text1->AddLocalRotation(TextRotatorKuai);
				Text2->AddLocalRotation(TextRotatorKuai);
			}
		}
	}
}

void AFloor::OnBoxOverlap_1(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	MyCharacter = Cast<ASlashCharacter>(OtherActor);

	if(MyCharacter && BoxTrace(OtherActor))
	{
		//获取数组长度
		const int32 NumRandomFloor = RandomFloor.Num();
		if (NumRandomFloor > 0)
		{
			//获取数组中随机一个
			const int32 intRandomFloor = FMath::RandRange(0, NumRandomFloor - 1);

			FVector FloorSpawnLocation = GetActorLocation();
			FloorSpawnLocation.X += 2000.f;
			GetWorld()->SpawnActor<AFloor>(RandomFloor[intRandomFloor], FloorSpawnLocation, GetActorRotation());

			UpNumber(Rule1, Number1);
		}
	}
}

void AFloor::OnBoxOverlap_2(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	MyCharacter = Cast<ASlashCharacter>(OtherActor);
	if (MyCharacter && BoxTrace(OtherActor))
	{
		//获取数组长度
		const int32 NumRandomFloor = RandomFloor.Num();
		if (NumRandomFloor > 0)
		{
			//获取数组中随机一个地板
			const int32 intRandomFloor = FMath::RandRange(0, NumRandomFloor - 1);

			FVector FloorSpawnLocation = GetActorLocation();
			FloorSpawnLocation.X += 2000.f;
			if (!GetWorld()) return;
			GetWorld()->SpawnActor<AFloor>(RandomFloor[intRandomFloor], FloorSpawnLocation, GetActorRotation());

			UpNumber(Rule2, Number2);
		}
	}
}

void AFloor::SetRuleNumberMesh(UStaticMeshComponent* TextMeshComponent, int32 TextNumber)
{
	TextMeshComponent->SetStaticMesh(Meshes[TextNumber]);
}

bool AFloor::BoxTrace(AActor* actor)
{
	const FVector Start(FloorMesh->GetComponentLocation().X + 2000.f, FloorMesh->GetComponentLocation().Y, FloorMesh->GetComponentLocation().Z);
	const FVector End = Start;
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(actor);

	//命中结果
	FHitResult BoxHit;

	//按通道进行盒体追踪
	UKismetSystemLibrary::BoxTraceSingle
	(
		this,
		Start,
		End,
		FVector(300.f, 300.f, 90.f),
		FloorMesh->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		BoxHit,
		true
	);
	return BoxHit.GetActor() == nullptr;
}

//生成随机事件
void AFloor::RandomEvent()
{
	int32 EventNumber = FMath::RandRange(0, 9);  //生成随机事件的概率
	if (EventNumber <= 6)
	{
		//生成敌人的事件
		for (int32 i = 0; i < AiNumber; i++)  //生成敌人的数量
		{
			SpawnAi();
		}

		//生成罐子、枪、道具的事件
		int32 GuanZi = FMath::RandRange(0, 2); //生成枪、罐子的概率
		if (GuanZi > 0)
		{
			if (GuanZiQiang.Num() > 0)
			{
				int32 RangeQiang = FMath::RandRange(0, GuanZiQiang.Num() - 1);  //随机数组元素
				FVector QiangLocation = GetActorLocation();
				QiangLocation.Z = FMath::FRandRange(30.f, 150.f);
				MyGuanZiQiang = GetWorld()->SpawnActor<ABreakableActor>(GuanZiQiang[RangeQiang], QiangLocation, FRotator(0.f, 180.f, 0.f));
			}
		}
	}
	else
	{
		//生成障碍物事件
		if (Roadblock.Num() > 0)
		{
			int32 RangeRoadblock = FMath::RandRange(0, Roadblock.Num() - 1);  //随机数组元素
			FVector RoadblockLocation = GetActorLocation();
			RoadblockLocation.Z = 800.f;
			MyRoadblock = GetWorld()->SpawnActor<ARoadblock>(Roadblock[RangeRoadblock], RoadblockLocation, FRotator(0.f, 0.f, 0.f));
		}
	}
}

void AFloor::SpawnNumber1()
{
	if (Text1)
	{
		const int32 Selection = FMath::RandRange(0, 3);
		switch (Selection)
		{
		case 0:
			SectionName1 = FString("+");
			Rule1 = ERule::ER_Add;
			Number1 = FMath::RandRange(1, 5);
			SetRuleNumberMesh(Text1, Number1 - 1);
			break;
		case 1:
			SectionName1 = FString("-");
			Rule1 = ERule::ER_Subtract;
			Number1 = FMath::RandRange(1, 5);
			SetRuleNumberMesh(Text1, Number1 + 4);
			break;
		case 2:
			SectionName1 = FString("*");
			Rule1 = ERule::ER_Multiply;
			Number1 = FMath::RandRange(2, 3);
			SetRuleNumberMesh(Text1, Number1 + 8);
			break;
		case 3:
			SectionName1 = FString("/");
			Rule1 = ERule::ER_Divide;
			Number1 = FMath::RandRange(2, 3);
			SetRuleNumberMesh(Text1, Number1 + 10);
			break;
		default:
			break;
		}
		CanRotatorText = true;
	}
}

void AFloor::SpawnNumber2()
{
	if (Text2)
	{
		const int32 Selection = FMath::RandRange(0, 3);
		switch (Selection)
		{
		case 0:
			SectionName2 = FString("+");
			Rule2 = ERule::ER_Add;
			Number2 = FMath::RandRange(1, 5);
			SetRuleNumberMesh(Text2, Number2 - 1);
			break;
		case 1:
			SectionName2 = FString("-");
			Rule2 = ERule::ER_Subtract;
			Number2 = FMath::RandRange(1, 5);
			SetRuleNumberMesh(Text2, Number2 + 4);
			break;
		case 2:
			SectionName2 = FString("*");
			Rule2 = ERule::ER_Multiply;
			Number2 = FMath::RandRange(2, 3);
			SetRuleNumberMesh(Text2, Number2 + 8);
			break;
		case 3:
			SectionName2 = FString("/");
			Rule2 = ERule::ER_Divide;
			Number2 = FMath::RandRange(2, 3);
			SetRuleNumberMesh(Text2, Number2 + 10);
			break;
		default:
			break;
		}
		CanRotatorText = true;
	}
}

//生成敌人
void AFloor::SpawnAi()
{
	if (RandomEnemy.Num() > 0)
	{	
		if (FloorMesh)
		{
			//平面内随机一个点
			float Max_X = FloorMesh->Bounds.Origin.X + FloorMesh->Bounds.BoxExtent.X * 0.9f;
			float Min_X = FloorMesh->Bounds.Origin.X + FloorMesh->Bounds.BoxExtent.X * -0.9f;
			float Max_Y = FloorMesh->Bounds.Origin.Y + FloorMesh->Bounds.BoxExtent.Y * 0.9f;
			float Min_Y = FloorMesh->Bounds.Origin.Y + FloorMesh->Bounds.BoxExtent.Y * -0.9f;
			const float RandRange_X = FMath::RandRange(Min_X, Max_X);
			const float RandRange_Y = FMath::RandRange(Min_Y, Max_Y);
			const float RandRange_Z = FloorMesh->GetComponentLocation().Z + 300.f;
			FVector SpawnAiLocation(RandRange_X, RandRange_Y, RandRange_Z);
			if (!GetWorld()) return;
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.Owner = Owner;
			//碰撞，尝试调整生成位置
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			const int32 RandRangeEnemy = FMath::RandRange(0, RandomEnemy.Num()-1);
			AEnemy* Enemy1 = GetWorld()->SpawnActor<AEnemy>(RandomEnemy[RandRangeEnemy], SpawnAiLocation, FRotator(0.f,180.f,0.f), ActorSpawnParams);
			Enemy1->SpawnDefaultController();
		}
	}
}

void AFloor::UpNumber(ERule Rule, int32 Number)
{
	if (MyCharacter == nullptr) return;

	switch (Rule)
	{
	case ERule::ER_Add:

		MyCharacter->MeshNumber = MyCharacter->MeshNumber + Number;
		
		AddSkeletalMesh();
		
		break;
	case ERule::ER_Subtract:

		MyCharacter->MeshNumber = MyCharacter->MeshNumber - Number;
		if (MyCharacter->MeshNumber <= 0) MyCharacter->MeshNumber = 1;

		AddSkeletalMesh();
	
		break;
	case ERule::ER_Multiply:

		MyCharacter->MeshNumber = MyCharacter->MeshNumber * Number;
		
		AddSkeletalMesh();

		break;	
	case ERule::ER_Divide:
		MyCharacter->MeshNumber = MyCharacter->MeshNumber / Number;
		if (MyCharacter->MeshNumber <= 0) MyCharacter->MeshNumber = 1;
		
		AddSkeletalMesh();

		break;
	default:
		break;
	}
}

void AFloor::AddSkeletalMesh()
{
	//生成骨骼网格体模型的排列位置
	if(!MyCharacter) return;
	MyCharacter->LoopUpMesh();

	//销毁组件
	IsBoxHit = true;
	if (NumberWall_1) { NumberWall_1->DestroyComponent(); }
	if (NumberWall_2) { NumberWall_2->DestroyComponent(); }
	if (Text1) { Text1->DestroyComponent(); }
	if (Text2) { Text2->DestroyComponent(); }

	//延迟后执行
	GetWorldTimerManager().SetTimer(PatrolTimer, this, &AFloor::DestroyTimer, 8.f, false);
}

void AFloor::DestroyTimer()
{
	if (MyRoadblock)
	{
		MyRoadblock->Destroy();
	}
	if (MyGuanZiQiang)
	{
		MyGuanZiQiang->Destroy();
	}

	//设置生命周期
	Destroy();
}

// 
// GEngine->AddOnScreenDebugMessage
// (
// 	-1,
// 	2, 			//	显示的时间/秒
// 	FColor::Blue, 	//	显示的颜色
// 	FString::Printf(TEXT("%s%d = %d"), *Name, AddNumber, MyCharacter->MeshNumber)     //显示的信息
// );
