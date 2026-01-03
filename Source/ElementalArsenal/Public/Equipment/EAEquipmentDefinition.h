// Copyright NeoNexus Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "EAEquipmentDefinition.generated.h"

class USkeletalMesh;
class UStaticMesh;

/**
 * Defines a piece of equipment in the Elemental Arsenal system.
 * Contains visuals, gameplay tags (Elements), and abilities to grant.
 */
UCLASS(BlueprintType, Const)
class ELEMENTALARSENAL_API UEAEquipmentDefinition : public UDataAsset
{
	GENERATED_BODY()

public:
	// Option A: Skeletal Mesh (Animated weapons)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
	TSoftObjectPtr<USkeletalMesh> MeshToSpawn;

	// Option B: Static Mesh (Simple weapons like cubes/swords)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
	TSoftObjectPtr<UStaticMesh> StaticMeshToSpawn;

	// socket to attach to
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
	FName AttachSocketName;

	// Abilities to grant to the wearer when equipped
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
	TArray<TSubclassOf<UGameplayAbility>> AbilitiesToGrant;

	// Tags to apply to the owner (e.g., Element.Fire, Equipment.Type.Sword)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
	FGameplayTagContainer EquipmentTags;
};
