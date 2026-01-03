// Copyright NeoNexus Studios. All Rights Reserved.

#pragma once

#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "EAAttributeSet.generated.h"

// Macro for defining getters/setters
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class ELEMENTALARSENAL_API UEAAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UEAAttributeSet();

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	// --- Attributes ---

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UEAAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UEAAttributeSet, MaxHealth)

	// "Meta Attribute" for incoming damage. Not persistent.
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UEAAttributeSet, IncomingDamage)

protected:
	// Helper to handle reactions
	void ProcessElementalReaction(const FGameplayEffectModCallbackData& Data);

	// Guard to prevent infinite recursion during reaction processing
	bool bIsProcessingReaction;
};
