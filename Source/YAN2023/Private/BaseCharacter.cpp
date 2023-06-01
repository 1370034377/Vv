#include "BaseCharacter.h"
#include "Components/BoxComponent.h"   //胶囊体
#include "Weapon.h"
#include "AttributeComponent.h"


// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//添加玩家角色属性组件
	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));

}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::Attack()
{
}

void ABaseCharacter::Die()
{
}

//播放命中时的蒙太奇
void ABaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

//由攻击方向播放不同蒙太奇
void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	//获取角色向前单位向量
	const FVector Forward = GetActorForwardVector();
	//命中点位置与角色位置统一高度
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	//获取角色到命中点的单位向量
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();
	//向量点积
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	//求反余弦值
	double Theta = FMath::Acos(CosTheta);
	//转换为角度值
	Theta = FMath::RadiansToDegrees(Theta);

	//根据命中角度判断攻击方向
	FName Section("Hit_Hou");
	if (Theta >= -45.f && Theta < 45.f)
	{
		Section = FName("Hit_Qian");
	}
	else
	{
		if (Theta >= -135.f && Theta < -45.f)
		{
			Section = FName("Hit_Zuo");
		}
		else
		{
			if (Theta >= 45.f && Theta < 135.f)
			{
				Section = FName("Hit_You");
			}
		}
	}
	PlayHitReactMontage(Section);
}

void ABaseCharacter::PlayAttackMontage()
{
}

bool ABaseCharacter::CanAttack()
{
	return false;
}

void ABaseCharacter::AttackEnd()
{
}

//设置武器Box碰撞，蓝图可调用
void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		//在攻击完成后重置可忽略检测数组Actor
		EquippedWeapon->IgnoreActors.Empty();
	}
}
