// Copyright NeoNexus Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayAbilitySpecHandle.h"
#include "EAEquipmentComponent.generated.h"

class UEAEquipmentDefinition;
class UAbilitySystemComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ELEMENTALARSENAL_API UEAEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEAEquipmentComponent();

	// Equips a new item, destroying the old one if it exists
	UFUNCTION(BlueprintCallable, Category = "Elemental Arsenal|Equipment")
	void EquipItem(const UEAEquipmentDefinition* ItemDef);

	// Unequips the current item
	UFUNCTION(BlueprintCallable, Category = "Elemental Arsenal|Equipment")
	void UnequipItem();

	UFUNCTION(BlueprintPure, Category = "Elemental Arsenal|Equipment")
	AActor* GetSpawnedEquipmentActor() const { return SpawnedEquipmentMesh; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	// The currently equipped definition
	UPROPERTY()
	const UEAEquipmentDefinition* CurrentEquipmentDef;

	// The actual spawned mesh actor (could be a full actor, but for simplicity we assume we might just spawn a mesh or a lightweight actor. 
	// For this portfolio piece, let's spawn a simple StaticMeshActor or just attach a component. 
	// Actually, best practice is usually to spawn an Actor so it can have its own logic, but let's keep it simple: 
	// We'll spawn a SkeletalMeshComponent attached to the owner.)
	// Wait, if we want it to be separate, spawning an Actor is cleaner.
	UPROPERTY()
	AActor* SpawnedEquipmentMesh;

	// Handles to the granted abilities, so we can remove them later
	TArray<FGameplayAbilitySpecHandle> GrantedAbilityHandles;

	// Helper to get Owner's ASC
	UAbilitySystemComponent* GetAbilitySystemComponent() const;
};
