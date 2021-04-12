// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CityGenerator.generated.h"

class ABuilding;
class AGrid;
class AEnvironmentGenerator;
class UInstancedStaticMeshComponent;

UCLASS()
class THEIMPOSSIBLE_API ACityGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACityGenerator(const FObjectInitializer& ObjectInitializer);

	void SpawnBuilding(int32 Type, FVector Location, FVector Scale, FActorSpawnParameters SpawnParameters);
	int32 SpawnRandomBuilding(FVector Location, float OffsetX, float OffsetY, int32 CurrentQuarter);
	void SpawnBlock(FVector Start, FVector End, float Intensity, int32 CurrentQuarter);

	UFUNCTION(BlueprintCallable)
	void SpawnCity(FVector Start, float CityRadius_);
	void SpawnCityQuarter(FRotator Rotation, AGrid* CityGrid, int32 Quarter);
	


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//UPROPERTY(VisibleAnywhere)
	//UInstancedStaticMeshComponent* InstancedStaticMeshComponent;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Buildings")
	//TArray<UStaticMesh*> BuildingMeshes;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Buildings")
	TArray<TSubclassOf<AActor>> Buildings;

	UPROPERTY()
	TArray<FVector> BuildingSizes;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkylineCurves")
	UCurveFloat* SkylineCurveX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkylineCurves")
	UCurveFloat* SkylineCurveY;

	UPROPERTY()
	AEnvironmentGenerator* EnvironmentGenerator;

	UPROPERTY()
	FVector CityCenter;

	float CurrentBlockDistanceToCenter;
	float CurrentBlockDistanceToCenterX;
	float CurrentBlockDistanceToCenterY;

	float TimeRequiredToBuildCity;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GeneratorSettings")
	float CityRadius = 50000.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GeneratorSettings")
	float DistanceBetweenBuildings = 300.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GeneratorSettings")
	float DistanceBetweenBlocks = 10000.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GeneratorSettings")
	float BuildingSpawnFrequency = 0.1f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GeneratorSettings")
	bool IsMenuCity = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CityShape")
	float BuildingIntensity = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CityShape")
	float CurveApplyRatio = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CityShape")
	float EllipseYRadiusDivident = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CityShape")
	float EllipseXRadiusDivident = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RoadSettings")
	float RoadStart = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RoadSettings")
	float RoadEnd = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "RoadSettings")
	int32 RoadDirection = 0;



	UPROPERTY()
	TArray<AActor*> BuildingsOfCity;

	
	TArray<TTuple<float, float>> BlockDistances;

	
	TArray <TTuple<FVector, FVector>> BlockLocations;

	UPROPERTY()
	FTimerHandle BuildingSpawnTimer;

	
	TQueue<TTuple<float, float>> BlockDistancesQueue;
	TQueue<TTuple<FVector, FVector>> BlockLocationsQueue;
	TQueue<int32> BelongingQuarterQueue;

	UPROPERTY()
	TArray<AGrid*> Grids = {};

	void SpawnCitySlowly();
	void UpdateCityCenter(FVector Difference);

	FVector LastOriginOffset = FVector(0.f,0.f,0.f);

	float CityConnectionPointRange = 600000.f;
private:
	void GetBuildingSizes();
	float MaxBuildingSize;
	int32 DEBUG_TotalCount = 0;
	int32 RemaniningSpawns = 0;
};




// ============================================================ Another Thread ============================================================

class AGrid;

class CityGenerateTask : public FNonAbandonableTask
{
public:
	CityGenerateTask(AGrid* CityGrid_, ACityGenerator* CityGenerator_, FRotator Rotation_, int32 Quarter_, bool IsMenuCity_);

	~CityGenerateTask();

	// required by UE4
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(SplineGenerateTask, STATGROUP_ThreadPoolAsyncTasks)
	}

	// automatically called 
	void DoWork();

	UPROPERTY()
	AGrid* CityGrid;
	UPROPERTY()
	ACityGenerator* CityGenerator;

	int32 GeneratedBlockCount;

	bool IsMenuCity = false;

	UPROPERTY()
	FRotator Rotation;

	int32 Quarter;
	TQueue<TTuple<float, float>> BlockDistancesQueue;
	TQueue<TTuple<FVector, FVector>> BlockLocationsQueue;
	TQueue<int32> BelongingQuarterQueue;


	// TEST
	int32 CurrentIteration = 0;
	int32 MaxIteration = 1000000;

	//FVector BlockStart, BlockEnd;
	bool CanSpawn;
};
