// Fill out your copyright notice in the Description page of Project Settings.


#include "UpgradeManager.h"
#include "Profile/PlayerProfile.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "Runtime/Engine/Classes/Engine/AssetManager.h"
#include "Structs.h"
#include "ImpossibleGameInstance.h"
#include "Kismet/GameplayStatics.h"

// Fix: a lot of code replication in this class

// Sets default values for this component's properties
UUpgradeManager::UUpgradeManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	

	static ConstructorHelpers::FObjectFinder<UDataTable> UpgradeTableEngineObject(TEXT("DataTable'/Game/Uluc/DataTables/EngineUpgradeDataTable.EngineUpgradeDataTable'"));
	if (UpgradeTableEngineObject.Succeeded())
	{
		UpgradeTableEngine = UpgradeTableEngineObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> UpgradeTableGearBoxObject(TEXT("DataTable'/Game/Uluc/DataTables/GearBoxUpgradeDataTable.GearBoxUpgradeDataTable'"));
	if (UpgradeTableGearBoxObject.Succeeded())
	{
		UpgradeTableGearBox = UpgradeTableGearBoxObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> UpgradeTableSteeringObject(TEXT("'/Game/Uluc/DataTables/SteeringUpgradeDataTable.SteeringUpgradeDataTable'"));
	if (UpgradeTableSteeringObject.Succeeded())
	{
		UpgradeTableSteering = UpgradeTableSteeringObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> UpgradeTableBrakesObject(TEXT("'/Game/Uluc/DataTables/BrakesUpgradeDataTable.BrakesUpgradeDataTable'"));
	if (UpgradeTableBrakesObject.Succeeded())
	{
		UpgradeTableBrakes = UpgradeTableBrakesObject.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UDataTable> UpgradeTableNitroObject(TEXT("'/Game/Uluc/DataTables/NitroUpgradeDataTable.NitroUpgradeDataTable'"));
	if (UpgradeTableNitroObject.Succeeded())
	{
		UpgradeTableNitro = UpgradeTableNitroObject.Object;
	}


}


// Called when the game starts
void UUpgradeManager::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UUpgradeManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UUpgradeManager::SetActiveCar(FString CarName)
{
	ActiveCar = CarName;
	UpdateUpgrades();
}

int32 UUpgradeManager::GetUpgradeLevel(EUpgrades UpgradeType)
{
	uint8 UpgradeIndex = (uint8)UpgradeType;

	TMap<FString, int32> CarIndexes = {};
	CarIndexes.Add("Pantera", 0);
	CarIndexes.Add("DeLorean", 1);
	CarIndexes.Add("Testarossa", 2);


	if (CarIndexes.Contains(ActiveCar))
	{
		int32 ActiveCarIndex = CarIndexes[ActiveCar];
		auto Bought = OwnerProfile->CarInformation.CarBought[ActiveCarIndex];

		if (Bought)
		{
			auto x = OwnerProfile->CarInformation.Upgrades[ActiveCar].UpgradeLevels[0];
			//UE_LOG(LogTemp, Warning, TEXT("UUpgradeManager::GetUpgradeLevel Active Car Upgrade[0]: %d"), x);

			return OwnerProfile->CarInformation.Upgrades[ActiveCar].UpgradeLevels[UpgradeIndex];
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
	

}

void UUpgradeManager::Upgrade(EUpgrades UpgradeType)
{
	uint8 UpgradeIndex = (uint8)UpgradeType;
	UE_LOG(LogTemp, Warning, TEXT("UUpgradeManager::Upgrade  ActiveCar: %s"), *ActiveCar);
	OwnerProfile->CarInformation.Upgrades[ActiveCar].UpgradeLevels[UpgradeIndex] += 1;
}

void UUpgradeManager::UpdateUpgrades()
{
	const FString ContextString(TEXT("Upgrade Context"));
	
	if (GetUpgradeLevel(EUpgrades::Engine) > 0)
	{
		FUpgradeTableRow* DataEngine = UpgradeTableEngine->FindRow<FUpgradeTableRow>(*FString::FromInt(GetUpgradeLevel(EUpgrades::Engine)), ContextString, true);
		ExtraEnginePower = DataEngine->UpgradeAmounts[1];
		ExtraEngineRPM = DataEngine->UpgradeAmounts[2];
		ExtraGearRatio = DataEngine->UpgradeAmounts[3];
	}
	else
	{
		ExtraEnginePower = 0.f;
		ExtraEngineRPM = 0.f;
		ExtraGearRatio = 0.f;
	}
	
	
	if (GetUpgradeLevel(EUpgrades::Gearbox) > 0)
	{
		FUpgradeTableRow* DataGearBox = UpgradeTableGearBox->FindRow<FUpgradeTableRow>(*FString::FromInt(GetUpgradeLevel(EUpgrades::Gearbox)), ContextString, true);
		ExtraGearChangeSpeed = DataGearBox->UpgradeAmounts[1];
	}
	else
	{
		ExtraGearChangeSpeed = 0.f;
	}
	
	
	if (GetUpgradeLevel(EUpgrades::Steering) > 0)
	{
		FUpgradeTableRow* DataSteering = UpgradeTableSteering->FindRow<FUpgradeTableRow>(*FString::FromInt(GetUpgradeLevel(EUpgrades::Steering)), ContextString, true);
		ExtraSteeringMultiplier = DataSteering->UpgradeAmounts[1];
	}
	else
	{
		ExtraSteeringMultiplier = 0.f;
	}

	
	if (GetUpgradeLevel(EUpgrades::Brakes) > 0)
	{
		FUpgradeTableRow* DataBrakes = UpgradeTableBrakes->FindRow<FUpgradeTableRow>(*FString::FromInt(GetUpgradeLevel(EUpgrades::Brakes)), ContextString, true);
		ExtraBrakePower = DataBrakes->UpgradeAmounts[1];
	}
	else
	{
		ExtraBrakePower = 0.f;
	}
		

	if (GetUpgradeLevel(EUpgrades::Nitro) > 0)
	{
		FUpgradeTableRow* DataNitro = UpgradeTableNitro->FindRow<FUpgradeTableRow>(*FString::FromInt(GetUpgradeLevel(EUpgrades::Nitro)), ContextString, true);
		NitroTime = DataNitro->UpgradeAmounts[1];
	}
	else
	{
		NitroTime = 0.f;
	}

}


int32 UUpgradeManager::GetEngineUpgrade()
{
	return GetUpgradeLevel(EUpgrades::Engine);
}

int32 UUpgradeManager::GetGearBoxUpgrade()
{
	return GetUpgradeLevel(EUpgrades::Gearbox);
}

int32 UUpgradeManager::GetSteeringUpgrade()
{
	return GetUpgradeLevel(EUpgrades::Steering);
}

int32 UUpgradeManager::GetBrakesUpgrade()
{
	return GetUpgradeLevel(EUpgrades::Brakes);
}

int32 UUpgradeManager::GetNitroUpgrade()
{
	return GetUpgradeLevel(EUpgrades::Nitro);
}


float UUpgradeManager::GetEnginePrice()
{
	int32 EngineUpgrade = GetUpgradeLevel(EUpgrades::Engine);
	if (EngineUpgrade < 29)
	{
		const FString ContextString(TEXT("Price Context"));
		FUpgradeTableRow* Data = UpgradeTableEngine->FindRow<FUpgradeTableRow>(*FString::FromInt(EngineUpgrade + 1), ContextString, true);

		return FMath::FloorToFloat(Data->UpgradeAmounts[0] * PriceMultipliers[0]);
	}
	else
	{
		return 0.f;
	}
	
}

float UUpgradeManager::GetGearBoxPrice()
{
	int32 GearBoxUpgrade = GetUpgradeLevel(EUpgrades::Gearbox);
	if (GearBoxUpgrade < 29)
	{
		const FString ContextString(TEXT("Price Context"));
		FUpgradeTableRow* Data = UpgradeTableGearBox->FindRow<FUpgradeTableRow>(*FString::FromInt(GearBoxUpgrade + 1), ContextString, true);

		return FMath::FloorToFloat(Data->UpgradeAmounts[0] * PriceMultipliers[1]);
	}
	else
	{
		return 0.f;
	}

}


float UUpgradeManager::GetSteeringPrice()
{
	int32 SteeringUpgrade = GetUpgradeLevel(EUpgrades::Steering);
	if (SteeringUpgrade < 29)
	{
		const FString ContextString(TEXT("Price Context"));
		FUpgradeTableRow* Data = UpgradeTableSteering->FindRow<FUpgradeTableRow>(*FString::FromInt(SteeringUpgrade + 1), ContextString, true);

		return FMath::FloorToFloat(Data->UpgradeAmounts[0] * PriceMultipliers[2]);
	}
	else
	{
		return 0.f;
	}

}

float UUpgradeManager::GetBrakesPrice()
{
	int32 BrakesUpgrade = GetUpgradeLevel(EUpgrades::Brakes);
	if (BrakesUpgrade < 29)
	{
		const FString ContextString(TEXT("Price Context"));
		FUpgradeTableRow* Data = UpgradeTableBrakes->FindRow<FUpgradeTableRow>(*FString::FromInt(BrakesUpgrade + 1), ContextString, true);

		return FMath::FloorToFloat(Data->UpgradeAmounts[0] * PriceMultipliers[3]);

		
	}
	else
	{
		return 0.f;
	}
	
}

float UUpgradeManager::GetNitroPrice()
{
	int32 NitroUpgrade = GetUpgradeLevel(EUpgrades::Nitro);
	if (NitroUpgrade < 29)
	{
		const FString ContextString(TEXT("Price Context"));
		FUpgradeTableRow* Data = UpgradeTableNitro->FindRow<FUpgradeTableRow>(*FString::FromInt(NitroUpgrade + 1), ContextString, true);

		return FMath::FloorToFloat(Data->UpgradeAmounts[0] * PriceMultipliers[4]);
	}
	else
	{
		return 0.f;
	}
}


bool UUpgradeManager::UpgradeEngine()
{
	int32 EngineUpgrade = GetUpgradeLevel(EUpgrades::Engine);

	if (EngineUpgrade + 1 > MaxUpgrade) return false;

	const FString ContextString(TEXT("Upgrade Context"));
	FUpgradeTableRow* Data = UpgradeTableEngine->FindRow<FUpgradeTableRow>(*FString::FromInt(EngineUpgrade + 1), ContextString, true);
	float PriceEngineUpgrade = Data->UpgradeAmounts[0];

	float FinalPrice = FMath::FloorToFloat(PriceEngineUpgrade * PriceMultipliers[0]);

	bool Success = false;
	if (OwnerProfile->Money >= FinalPrice)
	{
		Success = true;
		Upgrade(EUpgrades::Engine);
		ExtraEnginePower = Data->UpgradeAmounts[1];
		ExtraEngineRPM = Data->UpgradeAmounts[2];
		ExtraGearRatio = Data->UpgradeAmounts[3];
		OwnerProfile->Money -= FinalPrice;
	}

	return Success;
}

bool UUpgradeManager::UpgradeGearBox()
{
	int32 GearBoxUpgrade = GetUpgradeLevel(EUpgrades::Gearbox);

	if (GearBoxUpgrade + 1 > MaxUpgrade) return false;

	const FString ContextString(TEXT("Upgrade Context"));
	FUpgradeTableRow* Data = UpgradeTableGearBox->FindRow<FUpgradeTableRow>(*FString::FromInt(GearBoxUpgrade + 1), ContextString, true);
	float PriceGearBoxUpgrade = Data->UpgradeAmounts[0];

	float FinalPrice = FMath::FloorToFloat(PriceGearBoxUpgrade * PriceMultipliers[1]);

	bool Success = false;
	if (OwnerProfile->Money >= FinalPrice)
	{
		Success = true;
		Upgrade(EUpgrades::Gearbox);
		ExtraGearChangeSpeed = Data->UpgradeAmounts[1];
		OwnerProfile->Money -= FinalPrice;
	}

	//UpdateUpgrades();

	return Success;
}

bool UUpgradeManager::UpgradeSteering()
{
	int32 SteeringUpgrade = GetUpgradeLevel(EUpgrades::Steering);

	if (SteeringUpgrade + 1 > MaxUpgrade) return false;

	const FString ContextString(TEXT("Upgrade Context"));
	FUpgradeTableRow* Data = UpgradeTableSteering->FindRow<FUpgradeTableRow>(*FString::FromInt(SteeringUpgrade + 1), ContextString, true);
	float PriceSteeringUpgrade = Data->UpgradeAmounts[0];

	float FinalPrice = FMath::FloorToFloat(PriceSteeringUpgrade * PriceMultipliers[2]);

	bool Success = false;
	if (OwnerProfile->Money >= FinalPrice)
	{
		Success = true;
		Upgrade(EUpgrades::Steering);
		ExtraSteeringMultiplier = Data->UpgradeAmounts[1];
		OwnerProfile->Money -= FinalPrice;
	}

	//UpdateUpgrades();

	return Success;
}

bool UUpgradeManager::UpgradeBrakes()
{
	int32 BrakesUpgrade = GetUpgradeLevel(EUpgrades::Brakes);

	if (BrakesUpgrade + 1 > MaxUpgrade) return false;

	const FString ContextString(TEXT("Upgrade Context"));
	FUpgradeTableRow* Data = UpgradeTableBrakes->FindRow<FUpgradeTableRow>(*FString::FromInt(BrakesUpgrade + 1), ContextString, true);
	float PriceBrakesUpgrade = Data->UpgradeAmounts[0];

	float FinalPrice = FMath::FloorToFloat(PriceBrakesUpgrade * PriceMultipliers[3]);

	bool Success = false;
	if (OwnerProfile->Money >= FinalPrice)
	{
		Success = true;
		Upgrade(EUpgrades::Brakes);
		ExtraBrakePower = Data->UpgradeAmounts[1];
		OwnerProfile->Money -= FinalPrice;
	}

	//UpdateUpgrades();

	return Success;
}

bool UUpgradeManager::UpgradeNitro()
{
	int32 NitroUpgrade = GetUpgradeLevel(EUpgrades::Nitro);

	if (NitroUpgrade + 1 > MaxUpgradeNitro) return false;

	bool Success = false;

	const FString ContextString(TEXT("Upgrade Context"));
	FUpgradeTableRow* Data = UpgradeTableNitro->FindRow<FUpgradeTableRow>(*FString::FromInt(NitroUpgrade + 1), ContextString, true);
	float PriceNitroUpgrade = Data->UpgradeAmounts[0];

	float FinalPrice = FMath::FloorToFloat(PriceNitroUpgrade * PriceMultipliers[4]);

	if (OwnerProfile->Money >= FinalPrice)
	{
		Success = true;
		Upgrade(EUpgrades::Nitro);
		NitroTime = Data->UpgradeAmounts[1];
		OwnerProfile->Money -= FinalPrice;
	}

	//UpdateUpgrades();

	return Success;
}

void UUpgradeManager::ResetUpgrades()
{
	// TODO: will reset to different values according to car 

	TArray<FString> Cars = OwnerProfile->GameInstance_->CarList;

	for (auto CarName : Cars)
	{
		int32 NumUpgrades = OwnerProfile->CarInformation.Upgrades[CarName].UpgradeLevels.Num();
		for (int32 i = 0; i < NumUpgrades; i++)
		{
			OwnerProfile->CarInformation.Upgrades[CarName].UpgradeLevels[i] = 0;
		}
		
	}

}