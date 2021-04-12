// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "UpgradeManager.generated.h"

// TODO: Nitro upgrades

class UPlayerProfile;

UENUM()
enum class EUpgrades : uint8
{
	Engine = 0		UMETA(DisplayName =  "Engine"),
	Gearbox = 1		UMETA(DisplayName = "Gearbox"),
	Steering = 2	UMETA(DisplayName = "Steering"),
	Brakes = 3		UMETA(DisplayName = "Brakes"),
	Nitro = 4		UMETA(DisplayName = "Nitro")

};




USTRUCT(BlueprintType)
struct FUpgradeTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

/// <summary>
/// UpgradeAmounts array explanation for each upgrade type (cumulative except price):
/// 
/// Engine: price-power-rpm-extra gear ratio
/// GearBox: price-gear change speed decrease
/// ...
/// </summary>

public:
	FUpgradeTableRow() : UpgradeLevel(0), UpgradeAmounts({}) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LevelUp)
	int32 UpgradeLevel;

	// first element of this array is price, rest are properties that will change depending on upgraded part type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LevelUp)
	TArray<float> UpgradeAmounts;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THEIMPOSSIBLE_API UUpgradeManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UUpgradeManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetActiveCar(FString CarName);

	int32 GetUpgradeLevel(EUpgrades UpgradeType);

	void Upgrade(EUpgrades UpgradeType);

	UFUNCTION(BlueprintCallable)
	void UpdateUpgrades();

	UFUNCTION(BlueprintCallable)
	int32 GetEngineUpgrade();

	UFUNCTION(BlueprintCallable)
	int32 GetGearBoxUpgrade();

	UFUNCTION(BlueprintCallable)
	int32 GetSteeringUpgrade();

	UFUNCTION(BlueprintCallable)
	int32 GetBrakesUpgrade();

	UFUNCTION(BlueprintCallable)
	int32 GetNitroUpgrade();

	UFUNCTION(BlueprintCallable)
	float GetEnginePrice();

	UFUNCTION(BlueprintCallable)
	float GetGearBoxPrice();

	UFUNCTION(BlueprintCallable)
	float GetSteeringPrice();

	UFUNCTION(BlueprintCallable)
	float GetBrakesPrice();

	UFUNCTION(BlueprintCallable)
	float GetNitroPrice();

	UFUNCTION(BlueprintCallable)
	bool UpgradeEngine();

	UFUNCTION(BlueprintCallable)
	bool UpgradeGearBox();

	UFUNCTION(BlueprintCallable)
	bool UpgradeSteering();

	UFUNCTION(BlueprintCallable)
	bool UpgradeBrakes();

	UFUNCTION(BlueprintCallable)
	bool UpgradeNitro();

	UFUNCTION()
	void ResetUpgrades();

	UPROPERTY()
	UPlayerProfile* OwnerProfile;

	UPROPERTY()
	FString ActiveCar = "Pantera";

	//UPROPERTY()
	//TArray<int32> ActiveUpgradeLevels = { 0, 0, 0, 0, 0 };

	// Car Upgrade Stuff
	float BasePrice = 10.f;

	TArray<float> PriceMultipliers = { 0.8f, 2.4f, 2.4f, 2.4f, 2.4f }; // engine, gearbox, steering, brakes, nitro

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "General")
	int32 MaxUpgrade = 29;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "General")
	int32 MaxUpgradeNitro = 29;

	//int32 EngineUpgrade = 0;
	//int32 GearBoxUpgrade = 0;
	//int32 SteeringUpgrade = 0;
	//int32 BrakesUpgrade = 0;
	//int32 NitroUpgrade = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "General")
	float ExtraEnginePower = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "General")
	float ExtraEngineRPM = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "General")
	float ExtraGearRatio = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "General")
	float ExtraGearChangeSpeed = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "General")
	float ExtraSteeringMultiplier = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "General")
	float ExtraBrakePower = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "General")
	float NitroTime = 0.f;

	float EngineUpgradePriceMultiplier = 5.f;
	float GearBoxUpgradePriceMultiplier = 5.f;
	float SteeringUpgradePriceMultiplier = 5.f;
	float BrakesUpgradePriceMultiplier = 5.f;
	float NitroUpgradePriceMultiplier = 25.f;

	class UDataTable* UpgradeTableEngine;
	class UDataTable* UpgradeTableGearBox;
	class UDataTable* UpgradeTableSteering;
	class UDataTable* UpgradeTableBrakes;
	class UDataTable* UpgradeTableNitro;
};
