
#include "Treasure.h"
#include "SlashCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "Bullet.h"

//重载基类Item的重叠事件
void ATreasure::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//类型转换为玩家角色
	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(OtherActor);
	if (SlashCharacter)
	{	
		SlashCharacter->AddQiang(this);

		//获取时的声音，获取时关闭特效
		if (PickupSound)
		{	
			UGameplayStatics::PlaySoundAtLocation(this, PickupSound, SlashCharacter->GetActorLocation());
		}
		SetCollision_No();
	}
}

//实现接口:被调用时触发
void ATreasure::GetHit_Implementation(const FVector& ImpactPoint)
{
	//碰撞，尝试调整生成位置
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.Owner = Owner;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;	
	//确定子弹飞行方向
	FRotator BulletRotation = ItemMesh->GetComponentRotation();
	BulletRotation.Yaw = BulletRotation.Yaw + 90.f;

	if (GetWorld() && AttackBullet && ItemMesh)
	{
		GetWorld()->SpawnActor<ABullet>(AttackBullet, ItemMesh->GetSocketLocation(TEXT("F")), BulletRotation, ActorSpawnParams);
	}
}

void ATreasure::PlaySound()
{
	if (AttackSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation());
	}
}

void ATreasure::PlayParticle_1()
{
	if (Particle_1)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, Particle_1, GetActorLocation());
	}
}


//GEngine->AddOnScreenDebugMessage
//(
//	-1,
//	2, 			//	显示的时间/秒
//	FColor::Blue, 	//	显示的颜色
//	FString::Printf(TEXT("%d"), SlashCharacter->SkeletalMeshComponent.Num())     //显示的信息
//);