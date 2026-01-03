// Copyright NeoNexus Studios. All Rights Reserved.

#include "Equipment/EAEquipmentComponent.h"
#include "Equipment/EAEquipmentDefinition.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"
#include "Animation/SkeletalMeshActor.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/AssetManager.h"
#include "ElementalArsenal.h" // For Log Category

UEAEquipmentComponent::UEAEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	CurrentEquipmentDef = nullptr;
	SpawnedEquipmentMesh = nullptr;
}

void UEAEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UEAEquipmentComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnequipItem();
	Super::EndPlay(EndPlayReason);
}

UAbilitySystemComponent* UEAEquipmentComponent::GetAbilitySystemComponent() const
{
	AActor* Owner = GetOwner();
	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Owner))
	{
		return ASI->GetAbilitySystemComponent();
	}
	return Owner ? Owner->FindComponentByClass<UAbilitySystemComponent>() : nullptr;
}

void UEAEquipmentComponent::EquipItem(const UEAEquipmentDefinition* ItemDef)
{
	if (!ItemDef)
	{
		UnequipItem();
		return;
	}

	if (CurrentEquipmentDef == ItemDef)
	{
		return; // Already equipped
	}

	UnequipItem(); // Clean up old item
	CurrentEquipmentDef = ItemDef;

	// DEBUG ON SCREEN
	if (GEngine)
	{
		FString DebugMsg = FString::Printf(TEXT("Equipping Item: %s"), *ItemDef->GetName());
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, DebugMsg);
	}

	AActor* Owner = GetOwner();
	if (!Owner) return;

	// Prepare Spawn Params
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = Owner;

	// 1. Visuals: Spawn the Mesh
	// Option A: Skeletal Mesh

	if (USkeletalMesh* SkelMeshAsset = ItemDef->MeshToSpawn.LoadSynchronous())
	{
		ASkeletalMeshActor* NewActor = GetWorld()->SpawnActor<ASkeletalMeshActor>(ASkeletalMeshActor::StaticClass(), FTransform::Identity, SpawnParams);
		if (NewActor)
		{
			NewActor->GetSkeletalMeshComponent()->SetSkeletalMesh(SkelMeshAsset);
			NewActor->GetSkeletalMeshComponent()->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
			NewActor->GetSkeletalMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			NewActor->GetSkeletalMeshComponent()->SetGenerateOverlapEvents(false); // <--- NUCLEAR OPTION
			NewActor->SetActorEnableCollision(false);
			
			SpawnedEquipmentMesh = NewActor;
			UE_LOG(LogElementalArsenal, Log, TEXT("EquipItem: Spawned SkeletalMeshActor %s"), *NewActor->GetName());
		}
		else
		{
			UE_LOG(LogElementalArsenal, Error, TEXT("EquipItem: Failed to spawn SkeletalMeshActor!"));
		}
	}
	// Option B: Static Mesh (Fallback)
	else
	{
		if (UStaticMesh* StaticMeshAsset = ItemDef->StaticMeshToSpawn.LoadSynchronous())
		{
			if (AStaticMeshActor* NewActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FTransform::Identity, SpawnParams))
			{
				NewActor->GetStaticMeshComponent()->SetStaticMesh(StaticMeshAsset);
				NewActor->GetStaticMeshComponent()->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
				NewActor->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				NewActor->GetStaticMeshComponent()->SetGenerateOverlapEvents(false); // <--- NUCLEAR OPTION
				NewActor->SetActorEnableCollision(false);
				
				SpawnedEquipmentMesh = NewActor;
				UE_LOG(LogElementalArsenal, Log, TEXT("EquipItem: Spawned StaticMeshActor %s"), *NewActor->GetName());
			}
		}
		else
		{
			UE_LOG(LogElementalArsenal, Warning, TEXT("EquipItem: No Mesh found in Definition for %s"), *ItemDef->GetName());
		}
	}

	// Attach whatever we spawned
	if (SpawnedEquipmentMesh)
	{
		bool bAttached = false;
		if (ACharacter* Char = Cast<ACharacter>(Owner))
		{
			bAttached = SpawnedEquipmentMesh->AttachToComponent(Char->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, ItemDef->AttachSocketName);
		}
		else
		{
			bAttached = SpawnedEquipmentMesh->AttachToActor(Owner, FAttachmentTransformRules::SnapToTargetNotIncludingScale, ItemDef->AttachSocketName);
		}

		if (bAttached)
		{
			UE_LOG(LogElementalArsenal, Log, TEXT("EquipItem: Attached %s to socket %s"), *SpawnedEquipmentMesh->GetName(), *ItemDef->AttachSocketName.ToString());
		}
		else
		{
			UE_LOG(LogElementalArsenal, Error, TEXT("EquipItem: Failed to attach %s to socket %s. Check if socket exists!"), *SpawnedEquipmentMesh->GetName(), *ItemDef->AttachSocketName.ToString());
		}
	}

	// 2. Gameplay: Grant Abilities & Tags
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		// Grant Abilities
		for (TSubclassOf<UGameplayAbility> AbilityClass : ItemDef->AbilitiesToGrant)
		{
			if (AbilityClass)
			{
				FGameplayAbilitySpec Spec(AbilityClass, 1, INDEX_NONE, this);
				// Add Tags to Source Object (not dynamic tags, as that API changed, simpler to just rely on Owner Tags)
				
				FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(Spec);
				GrantedAbilityHandles.Add(Handle);
			}
		}

		// Apply Loose Tags
		ASC->AddLooseGameplayTags(ItemDef->EquipmentTags);
	}
}

void UEAEquipmentComponent::UnequipItem()
{
	if (!CurrentEquipmentDef) return;

	// 1. Remove Visuals
	if (SpawnedEquipmentMesh)
	{
		SpawnedEquipmentMesh->Destroy();
		SpawnedEquipmentMesh = nullptr;
	}

	// 2. Remove Gameplay (Abilities & Tags)
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		// Remove Abilities
		for (FGameplayAbilitySpecHandle Handle : GrantedAbilityHandles)
		{
			ASC->ClearAbility(Handle);
		}
		GrantedAbilityHandles.Empty();

		// Remove Loose Tags
		ASC->RemoveLooseGameplayTags(CurrentEquipmentDef->EquipmentTags);
	}

	CurrentEquipmentDef = nullptr;
}