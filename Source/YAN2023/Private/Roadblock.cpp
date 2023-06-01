
#include "Roadblock.h"
#include "BaseCharacter.h"
#include "Kismet/GameplayStatics.h"


void ARoadblock::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARoadblock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetRoadblockRotator(DeltaTime);
	
}

//旋转模型
void ARoadblock::SetRoadblockRotator(float TickTime)
{
	if (ItemMesh)
	{
		//设置模型转动
		float AddTime = 0.f;
		AddTime += TickTime;
		float Man = FMath::Sin(AddTime) * MeshSpeed;
		float Kuai = (FMath::Abs(FMath::Sin(AddTime)) + 0.2) * MeshSpeed;
		FRotator TextRotatorMan(FRotator(0.f, 0.f, Man));
		FRotator TextRotatorKuai(FRotator(0.f, 0.f, Kuai));
		if (ItemMesh->GetRelativeRotation().Roll <= 135.f && ItemMesh->GetRelativeRotation().Roll > (10.f))
		{
			ItemMesh->AddLocalRotation(TextRotatorKuai);
			
			//仅播放一次
			if (IsSound)
			{
				IsSound = false;
				//生成音效
				if (RoadblockSound) { UGameplayStatics::PlaySoundAtLocation(this, RoadblockSound, GetActorLocation()); }
			}

		}
		else
		{
			ItemMesh->AddLocalRotation(TextRotatorMan);

			IsSound = true;
		}
	}
}

void ARoadblock::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	ABaseCharacter* HitCharacter = Cast<ABaseCharacter>(OtherActor);
	if (HitCharacter)
	{
		HitCharacter->LaunchCharacter(FVector(0.f, -1000.f, 1000.f), false, false);
	}
}