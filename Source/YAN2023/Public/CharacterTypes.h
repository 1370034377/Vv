#pragma once



//无装备、单手装备、双手装备
UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),  
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One Handed Weapon"),  
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two Handed Weapon")    
};


//攻击模式状态
UENUM(BlueprintType)
enum class EActionState :uint8
{
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),   //空闲中
	EAS_Attacking UMETA(DisplayName = "Attacking"),   //攻击中
	EAS_EquippingWeapon UMETA(DisplayName = "Equipping Weapon")   //更换武器中
};


//死亡状态
UENUM(BlueprintType)
enum class EDeathPose :uint8
{
	EDP_Alive UMETA(DisplayName = "Alive"),//活着
	EDP_Death1 UMETA(DisplayName = "Death1"),//死亡动画1
	EDP_Death2 UMETA(DisplayName = "Death2"),//死亡动画2

};


//敌人状态
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_Patrolling UMETA(DisplayName = "Patrolling"),  //巡逻中
	EES_Chasing UMETA(DisplayName = "Chasing"),   //追逐中
	EES_Attacking UMETA(DisplayName = "Attacking")   //攻击中
};


//运算法则 + - * /
UENUM(BlueprintType)
enum class ERule : uint8
{
	ER_Add UMETA(DisplayName = "Add"),  // +
	ER_Subtract UMETA(DisplayName = "Subtract"),   // -
	ER_Multiply UMETA(DisplayName = "Multiply"),   // *
	ER_Divide UMETA(DisplayName = "Divide")   // /
};