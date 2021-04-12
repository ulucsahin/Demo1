// Fill out your copyright notice in the Description page of Project Settings.


#include "CarCustomisationManager.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "Runtime/Engine/Classes/Engine/AssetManager.h"
#include "Materials/MaterialInstance.h"
#include "../Garage/CarSelector.h"
#include "../Cars/CustomCarUluc/CustomCarUluc.h"
#include "Kismet/GameplayStatics.h"
#include "../ImpossibleGameInstance.h"
#include "../Profile/PlayerProfile.h"
#include "../Cars/CustomCarUluc/CarProperties.h"
#include "../Garage/CarInteriorDesigner.h"

// Sets default values
ACarCustomisationManager::ACarCustomisationManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	static ConstructorHelpers::FObjectFinder<UDataTable> UpgradeTableEngineObject(TEXT("DataTable'/Game/Uluc/DataTables/CarMaterialsDataTable.CarMaterialsDataTable'"));
	if (UpgradeTableEngineObject.Succeeded())
	{
		CarMaterialsDataTable = UpgradeTableEngineObject.Object;
	}


	
	static ConstructorHelpers::FObjectFinder<UDataTable> InteriorDataTableObject(TEXT("DataTable'/Game/Uluc/DataTables/InteriorMaterialsDataTable.InteriorMaterialsDataTable'"));
	if (InteriorDataTableObject.Succeeded())
	{
		InteriorMaterialsDataTable = InteriorDataTableObject.Object;
	}

}

// Called when the game starts or when spawned
void ACarCustomisationManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACarCustomisationManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACarCustomisationManager::LoadDefaultMaterials()
{	
	for (auto KV : CarSelector->CurrentCarProperties->MaterialsMenu)
	{
		auto SlotName = KV.Key;
		//UE_LOG(LogTemp, Warning, TEXT("ACarCustomisationManager::LoadDefaultMaterials SlotName: %s"), *SlotName.ToString());
		auto MaterialInstance = KV.Value;
		CarSelector->Mesh->SetMaterialByName(SlotName, MaterialInstance);
	}

	
}

void ACarCustomisationManager::SelectBodyMaterial(int32 BodyMaterialIndex, bool ForMenu, bool ShouldSave)
{
	// Get material
	const FString ContextString(TEXT("Car Context"));
	FString RowName = FString("Body").Append(FString::FromInt(BodyMaterialIndex));
	FCarMaterialsTableRow* Data = CarMaterialsDataTable->FindRow<FCarMaterialsTableRow>(*RowName, ContextString, true);
	UMaterialInstance* BodyMaterial = Data->Material;

	// Get slots for body materials 
	if (ForMenu) // for menu car
	{
		auto MaterialSlotNames = CarSelector->Mesh->GetMaterialSlotNames();
		// assign material
		for (auto SlotName : MaterialSlotNames)
		{
			if (SlotName.ToString().Contains("Body"))
			{
				CarSelector->Mesh->SetMaterialByName(SlotName, BodyMaterial);
			}
		}
	}
	else // for gameplay car
	{
		auto Player_ = Cast<ACustomCarUluc>(UGameplayStatics::GetPlayerPawn(World_, 0));
		auto MaterialSlotNames = Player_->MeshComponent->GetMaterialSlotNames();
		// assign material
		for (auto SlotName : MaterialSlotNames)
		{
			if (SlotName.ToString().Contains("Body"))
			{
				Player_->MeshComponent->SetMaterialByName(SlotName, BodyMaterial);
			}
		}
	}
		
}

void ACarCustomisationManager::SelectInteriorMaterial(int32 InteriorMaterialIndex, UDataTable* InteriorMaterialsDataTable_, bool ForMenu, bool ShouldSave)
{
	UE_LOG(LogTemp, Warning, TEXT("ACarCustomisationManager::SelectInteriorMaterial"));

	auto GameInstance_ = Cast<UImpossibleGameInstance>(UGameplayStatics::GetGameInstance(World_));
	auto SelectedInteriorMaterials = GameInstance_->PlayerProfile->CarInformation.SelectedInteriorMaterials;

	// Get Interior Materials Data Table for current car
	//InteriorMaterialsDataTable_ = CarSelector->CurrentCarProperties->InteriorMaterialsDataTable_;

	// Get material
	const FString ContextString(TEXT("Car Context"));
	FString RowName = FString("Interior").Append(FString::FromInt(InteriorMaterialIndex));
	FCarMaterialsTableRow* Data = InteriorMaterialsDataTable_->FindRow<FCarMaterialsTableRow>(*RowName, ContextString, true);
	UMaterialInstance* InteriorMaterial = Data->Material;
	
	// Get slots for interior materials 
	if (ForMenu) // for menu car
	{
		auto MaterialSlotNames = InteriorDesigner->Mesh->GetMaterialSlotNames();
		// assign material
		for (auto SlotName : MaterialSlotNames)
		{
			InteriorDesigner->Mesh->SetMaterialByName(SlotName, InteriorMaterial);
		}
	}
	else
	{
		auto Player_ = Cast<ACustomCarUluc>(UGameplayStatics::GetPlayerPawn(World_, 0));
		auto MaterialSlotNames = Player_->Interior->GetMaterialSlotNames();

		// assign material
		for (auto SlotName : MaterialSlotNames)
		{

			Player_->Interior->SetMaterialByName(SlotName, InteriorMaterial);
			
		}
	}

}

void ACarCustomisationManager::RegisterMaterial(int32 CarIndex, int32 MaterialIndex)
{
	auto GameInstance_ = Cast<UImpossibleGameInstance>(UGameplayStatics::GetGameInstance(World_));
	GameInstance_->PlayerProfile->CarInformation.SelectedBodyMaterials[CarIndex] = MaterialIndex;
}

void ACarCustomisationManager::RegisterInteriorMaterial(int32 CarIndex, int32 MaterialIndex)
{
	auto GameInstance_ = Cast<UImpossibleGameInstance>(UGameplayStatics::GetGameInstance(World_));
	GameInstance_->PlayerProfile->CarInformation.SelectedInteriorMaterials[CarIndex] = MaterialIndex;
}

void ACarCustomisationManager::OnProfileChange()
{
	auto GameInstance_ = Cast<UImpossibleGameInstance>(UGameplayStatics::GetGameInstance(World_));
	int32 SelectedCar = GameInstance_->PlayerProfile->SelectedCar;
	SelectBodyMaterial(GameInstance_->PlayerProfile->CarInformation.SelectedBodyMaterials[SelectedCar], true, false);
	SelectInteriorMaterial(GameInstance_->PlayerProfile->CarInformation.SelectedInteriorMaterials[SelectedCar], CarSelector->CurrentCarProperties->InteriorMaterialsDataTable, true, false);
}

// to be used in menus
FLinearColor ACarCustomisationManager::GetColorFromTable(int32 RowIndex)
{
	// Get color
	const FString ContextString(TEXT("Car Context"));
	FString RowName = FString("Body").Append(FString::FromInt(RowIndex));
	FCarMaterialsTableRow* Data = CarMaterialsDataTable->FindRow<FCarMaterialsTableRow>(*RowName, ContextString, true);
	FLinearColor MenuColor = Data->ColorDisplayed;

	return MenuColor;
}