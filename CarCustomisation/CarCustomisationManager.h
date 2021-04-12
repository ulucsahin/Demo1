// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "Math/Color.h"
#include "CarCustomisationManager.generated.h"

class ACarSelector;
class ACarInteriorDesigner;

USTRUCT(BlueprintType)
struct FCarMaterialsTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FCarMaterialsTableRow() : Material(nullptr), MaterialType("") {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaterialProperties")
	UMaterialInstance* Material;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MaterialProperties")
	FString MaterialType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MenuProperties")
	FLinearColor ColorDisplayed;

};


UCLASS()
class THEIMPOSSIBLE_API ACarCustomisationManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACarCustomisationManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void LoadDefaultMaterials();

	UFUNCTION(BlueprintCallable)
	void SelectBodyMaterial(int32 BodyMaterialIndex, bool ForMenu, bool ShouldSave);

	UFUNCTION(BlueprintCallable)
	void SelectInteriorMaterial(int32 InteriorMaterialIndex, UDataTable* InteriorMaterialsDataTable, bool ForMenu, bool ShouldSave);

	UFUNCTION(BlueprintCallable)
	void RegisterMaterial(int32 CarIndex, int32 MaterialIndex);

	UFUNCTION(BlueprintCallable)
	void RegisterInteriorMaterial(int32 CarIndex, int32 MaterialIndex);

	void OnProfileChange();

	UPROPERTY()
	ACarSelector* CarSelector;

	UPROPERTY()
	ACarInteriorDesigner* InteriorDesigner;

	UPROPERTY()
	class UDataTable* CarMaterialsDataTable;

	UPROPERTY()
	class UDataTable* InteriorMaterialsDataTable;

	UPROPERTY()
	UWorld* World_;

	UFUNCTION(BlueprintCallable)
	FLinearColor GetColorFromTable(int32 RowIndex);


};
