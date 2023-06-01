#include "Bullet.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"  //抛射物移动组件
#include "DrawDebugHelpers.h"  //绘制调式球
#include "Enemy.h"   //命中敌人
#include "Kismet/GameplayStatics.h"

#define DRAW_SPHERE(Location) if(GetWorld()) DrawDebugSphere(GetWorld(),Location,100.f,12,FColor::Red,false,5.f);


ABullet::ABullet()
{
	PrimaryActorTick.bCanEverTick = true;

	BulletBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BulletBox;

	AttackNiagara1 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("AttackNiagara1"));
	AttackNiagara1->SetupAttachment(GetRootComponent());
	AttackNiagara2 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("AttackNiagara2"));
	AttackNiagara2->SetupAttachment(GetRootComponent());

	AttackAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("AttackAudio"));
	AttackAudio->SetupAttachment(GetRootComponent());

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->InitialSpeed = 5000.f;
	ProjectileMovementComponent->MaxSpeed = 6000.f;
	ProjectileMovementComponent->Velocity.X = 500.f;
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();

	BulletBox->OnComponentBeginOverlap.AddDynamic(this, &ABullet::OnBoxOverlap);
	SetLifeSpan(2.f);
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABullet::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//获取重叠对象是否为敌人
	AEnemy* HitEnemy = Cast<AEnemy>(OtherActor);
	if (HitEnemy)
	{
		//造成伤害事件
		HitEnemy->BulletDamage(Damage);

		//子弹命中时的打击特效
		if (BulletHitNiagara) { UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletHitNiagara, GetActorLocation()); }

		Destroy();
	}
}
