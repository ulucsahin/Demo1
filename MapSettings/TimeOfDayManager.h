// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimeOfDayManager.generated.h"

//class UPostProcessComponent;
class APostProcessVolume;
class UImpossibleGameInstance;
//FPostProcessSettings

UENUM(BlueprintType)
enum class ETimeOfDay : uint8 {
	Morning = 0		 UMETA(DisplayName = "Morning"),
	Noon = 1 		 UMETA(DisplayName = "Noon"),
	Evening = 2      UMETA(DisplayName = "Evening"),
	Night = 3		 UMETA(DisplayName = "Night"),
};

UCLASS()
class THEIMPOSSIBLE_API ATimeOfDayManager : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ATimeOfDayManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void EnableTimeOfDay(ETimeOfDay TimeOfDay);

	void AdjustSunLocation();
	void AdjustDirectionalLight();
	void AdjustSkylight();
	void AdjustExponentialHeightFog();
	void AdjustHorizonColor();
	void AdjustWorldObjects();

	UPROPERTY()
	UImpossibleGameInstance* GameInstance;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PostProcess")
	APostProcessVolume* PostProcessVolumeMorning;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PostProcess")
	APostProcessVolume* PostProcessVolumeNoon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PostProcess")
	APostProcessVolume* PostProcessVolumeEvening;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PostProcess")
	APostProcessVolume* PostProcessVolumeNight;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TMaps")
	TMap<ETimeOfDay, FRotator> SunRotations = {};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TMaps")
	TMap<ETimeOfDay, FRotator> LightRotations = {};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TMaps")
	TMap<ETimeOfDay, FColor> DefaultLightColors = {};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Materials")
	UMaterialInstance* SkyMaterialMorning;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Materials")
	UMaterialInstance* SkyMaterialNoon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Materials")
	UMaterialInstance* SkyMaterialEvening;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Materials")
	UMaterialInstance* SkyMaterialNight;
};
