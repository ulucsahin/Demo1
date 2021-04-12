// Fill out your copyright notice in the Description page of Project Settings.

#include "CityGenerator.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "../City/Building.h"
#include "DrawDebugHelpers.h"
#include "../City/Grid.h"
#include "../Cars/CustomCarUluc/CameraShakeUluc.h"
#include "Containers/Queue.h"
#include "../SplineStuff/SplineConnectionPoint.h"
#include "../Environment/EnvironmentGenerator.h"
#include "Components/InstancedStaticMeshComponent.h"

// TODO: Properly calculate required time to build city.
// TODO: Properly add meshes from editor instead of string references (currently crashes if we add from editor)
// TODO: Pooling.

// Sets default values
ACityGenerator::ACityGenerator(const FObjectInitializer& ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void ACityGenerator::BeginPlay()
{
	Super::BeginPlay();

	CityCenter = GetActorLocation();
	GetBuildingSizes();
	
	TimeRequiredToBuildCity = 8.f; // in seconds.

}

// Called every frame
void ACityGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ACityGenerator::GetBuildingSizes()
{
	// Save building sizes into separate array and find max building size.

	MaxBuildingSize = 0.f;

	FActorSpawnParameters s = FActorSpawnParameters();
	s.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (auto Item : Buildings)
	{
		auto BuildingInstance = GetWorld()->SpawnActor<AActor>(Item, FVector(0.f, -0.f, -99999.f), FRotator::ZeroRotator, s);
		if (BuildingInstance)
		{
			TArray<UStaticMeshComponent*> comps;
			BuildingInstance->GetComponents(comps);
			if (comps.Num() == 1)
			{
				auto MeshSize = comps[0]->Bounds.BoxExtent;
				BuildingSizes.Add(MeshSize);

				if (MeshSize.Z > MaxBuildingSize)
				{
					MaxBuildingSize = MeshSize.Z;
				}
			}
			else
			{
				UE_LOG(LogTemp, Fatal, TEXT("ACityGenerator::GetBuildingSizes, Mesh sizes misaligned."));
			}
		}
	}

}

void ACityGenerator::SpawnBuilding(int32 Type, FVector Location, FVector Scale, FActorSpawnParameters SpawnParameters)
{
	// Spawn building at location with random Y rotation 

	float RotationY = FMath::RandRange(0, 3) * 90.f;

	AActor* Spawned = GetWorld()->SpawnActor<AActor>(Buildings[Type], Location, FRotator(0.f, RotationY, 0.f), SpawnParameters);
	if (Spawned)
	{
		Spawned->SetActorScale3D(Scale);
		BuildingsOfCity.Add(Spawned);
	}
	
}

int32 ACityGenerator::SpawnRandomBuilding(FVector Location, float OffsetX, float OffsetY, int32 CurrentQuarter)
{
	/* Spawns a random building and returns which type built. 
	// we should offset curve values so that center of city (0 distance) is at the center of the skyline curve.
	// CurrentBlockDistanceToCenter/CityRadius          : between 0-1
	// CurrentBlockDistanceToCenter/CityRadius + 1)     : between 1-2
	// CurrentBlockDistanceToCenter/CityRadius + 1) / 2 : between 0.5-1
	// multiply it with 20 (CurveRange) to normalize in curve range (curve is between 0-20, we use 10-20)
	*/

	int32 SpawnType = FMath::RandRange(0, Buildings.Num() - 1);
	
	float CurveRange = 20.f;
	float CurveTimeX = (CurrentBlockDistanceToCenterX / (CityRadius / 2) + 1) / 2 * CurveRange;
	float CurveTimeY = (CurrentBlockDistanceToCenterY / (CityRadius / 2) + 1) / 2 * CurveRange;
	float DesiredBuildingHeight = 0.f;
	float CurveValueCombined = 0.f;

	if (CurrentQuarter == 2)
	{
		CurveTimeX -= 10.f;
		CurveTimeY -= 10.f; // between 0-10 but center is 10, so we subtract it from 10 in next line.	
		CurveValueCombined = FMath::Min<float>(SkylineCurveX->GetFloatValue(10 - CurveTimeX), SkylineCurveY->GetFloatValue(10 - CurveTimeY));
	}
	else if (CurrentQuarter == 3)
	{
		CurveTimeX -= 10.f;
		CurveValueCombined = FMath::Min<float>(SkylineCurveX->GetFloatValue(10 - CurveTimeX), SkylineCurveY->GetFloatValue(CurveTimeY));
	}
	else if (CurrentQuarter == 0)
	{
		CurveTimeY -= 10.f;
		CurveValueCombined = FMath::Min<float>(SkylineCurveX->GetFloatValue(CurveTimeX), SkylineCurveY->GetFloatValue(10 - CurveTimeY));
	}
	else if (CurrentQuarter == 1)
	{
		CurveValueCombined = FMath::Min<float>(SkylineCurveX->GetFloatValue(CurveTimeX), SkylineCurveY->GetFloatValue(CurveTimeY));
	}
	DesiredBuildingHeight = CurveValueCombined * MaxBuildingSize;

	// Only check curve with percentage
	if (FMath::FRandRange(0.f, 1.f) <= CurveApplyRatio)
	{
		if (BuildingSizes[SpawnType].Z > DesiredBuildingHeight)
		{
			// THIS RECURSION MAY BE WHY SOMETIMES GAME FREEZE
			return SpawnRandomBuilding(Location, OffsetX, OffsetY, CurrentQuarter);
		}
	}

	FVector SpawnLocation = Location;
	float ScaleZ = FMath::RandRange(80, 120) / 100.f;
	SpawnLocation.X += OffsetX;
	SpawnLocation.Y += OffsetY;

	FActorSpawnParameters s = FActorSpawnParameters();
	s.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	FVector SpawnScale = FVector(1.f, 1.f, ScaleZ);
	SpawnScale = SpawnScale * 1.f;

	SpawnBuilding(SpawnType, SpawnLocation, SpawnScale, s);

	return SpawnType;
}

void ACityGenerator::SpawnBlock(FVector Start, FVector End, float Intensity, int32 CurrentQuarter)
{
	float CurrentX = 0.f;
	float CurrentY = 0.f;
	float RangeX = FMath::Abs(Start.X - End.X);
	float RangeY = FMath::Abs(Start.Y - End.Y);

	int32 SpawnType = 0;
	
	// ===================== 1 ===================== +X
	int32 Iteration = 0;
	while (CurrentX < RangeX)
	{
		Iteration++;
		if (Iteration > 100)
		{
			break;
		}
		// Adjust intensity of block
		if (FMath::FRandRange(0.f, 1.f) < Intensity)
		{
			SpawnType = SpawnRandomBuilding(Start, CurrentX, CurrentY, CurrentQuarter);
		}
		
		CurrentX += BuildingSizes[SpawnType].X + DistanceBetweenBuildings;
	}
	CurrentY += BuildingSizes[SpawnType].Y + DistanceBetweenBuildings;
	Iteration = 0;

	// ===================== 2 ===================== +Y
	while (CurrentY < RangeY)
	{
		Iteration++;
		if (Iteration > 100)
		{
			break;
		}

		// Adjust intensity of block
		if (FMath::FRandRange(0.f, 1.f) < Intensity)
		{
			SpawnType = SpawnRandomBuilding(Start, CurrentX, CurrentY, CurrentQuarter);
		}

		CurrentY += BuildingSizes[SpawnType].Y + DistanceBetweenBuildings;
	}	
	CurrentX -= BuildingSizes[SpawnType].X + DistanceBetweenBuildings;
	Iteration = 0;

	// ===================== 3 ===================== -X
	while (CurrentX > 0.f)
	{
		Iteration++;
		if (Iteration > 100)
		{
			break;
		}

		// Adjust intensity of block
		if (FMath::FRandRange(0.f, 1.f) < Intensity)
		{
			SpawnType = SpawnRandomBuilding(Start, CurrentX, CurrentY, CurrentQuarter);
		}
		CurrentX -= BuildingSizes[SpawnType].X + DistanceBetweenBuildings;
	}
	CurrentY -= 0.f;
	Iteration = 0;

	// ===================== 4 ===================== -Y
	while (CurrentY > 0.f)
	{
		Iteration++;
		if (Iteration > 100)
		{
			break;
		}

		// Adjust intensity of block
		if (FMath::FRandRange(0.f, 1.f) < Intensity)
		{
			SpawnType = SpawnRandomBuilding(Start, CurrentX, CurrentY, CurrentQuarter);
		}
		CurrentY -= BuildingSizes[SpawnType].Y + DistanceBetweenBuildings;
	}
	CurrentX -= 0.f;
	Iteration = 0;
}

void ACityGenerator::SpawnCity(FVector Start, float CityRadius_)
{
	// TODO: Marking roads on quarters automatically. Currently quarters 1-3 selected.
	AGrid* CityGrid = NewObject<AGrid>();
	CityGrid->InitializeGrid(CityRadius_, 500.f, DistanceBetweenBlocks, CityCenter, EllipseXRadiusDivident, EllipseYRadiusDivident);

	// Rotations to achieve ellipse
	float CurrentQuarter = 0;
	SpawnCityQuarter(FRotator(0.f, 0.f, 0.f), CityGrid, CurrentQuarter);

	auto CityGrid2 = NewObject<AGrid>();
	CityGrid2->InitializeGrid(CityRadius_, 500.f, DistanceBetweenBlocks, CityCenter, EllipseXRadiusDivident, EllipseYRadiusDivident);
	CityGrid2->MarkRoadArea(RoadStart, RoadEnd, RoadDirection);
	CurrentQuarter = 1;
	SpawnCityQuarter(FRotator(0.f, 0.f, 180.f), CityGrid2, CurrentQuarter);

	auto CityGrid3 = NewObject<AGrid>();
	CityGrid3->InitializeGrid(CityRadius_, 500.f, DistanceBetweenBlocks, CityCenter, EllipseXRadiusDivident, EllipseYRadiusDivident);
	CurrentQuarter = 2;
	SpawnCityQuarter(FRotator(0.f, 180.f, 180.f), CityGrid3, CurrentQuarter);

	auto CityGrid4 = NewObject<AGrid>();
	CityGrid4->InitializeGrid(CityRadius_, 500.f, DistanceBetweenBlocks, CityCenter, EllipseXRadiusDivident, EllipseYRadiusDivident);
	CityGrid4->MarkRoadArea(RoadStart, RoadEnd, RoadDirection);
	CurrentQuarter = 3;
	SpawnCityQuarter(FRotator(0.f, 180.f, 0.f), CityGrid4, CurrentQuarter);

	// SPLINE CONNECTION POINT
	ASplineConnectionPoint* SplineConnection1 = NewObject<ASplineConnectionPoint>();
	SplineConnection1->Location = CityCenter;
	SplineConnection1->Location.Z += 500.f;
	SplineConnection1->PointRange = CityConnectionPointRange; // 600000.f
	if (EnvironmentGenerator)
	{
		EnvironmentGenerator->RegisterSplineConnectionPoint_(SplineConnection1);
	}
	

	Grids.Empty();
	Grids = { CityGrid, CityGrid2, CityGrid3, CityGrid4 };

	GetWorldTimerManager().SetTimer(BuildingSpawnTimer, this, &ACityGenerator::SpawnCitySlowly, BuildingSpawnFrequency, true, 0.f);
}

void ACityGenerator::SpawnCityQuarter(FRotator Rotation, AGrid* CityGrid, int32 Quarter)
{
	(new FAutoDeleteAsyncTask<CityGenerateTask>(CityGrid, this, Rotation, Quarter, IsMenuCity))->StartBackgroundTask();
}

void ACityGenerator::SpawnCitySlowly()
{
	if (BlockLocationsQueue.IsEmpty() && BlockDistancesQueue.IsEmpty())
	{
		GetWorldTimerManager().ClearTimer(BuildingSpawnTimer);
	}
	else
	{
		TTuple<float, float> DistanceXY;
		TTuple<FVector, FVector> BlockLocationStartEnd;

		BlockDistancesQueue.Dequeue(DistanceXY);
		BlockLocationsQueue.Dequeue(BlockLocationStartEnd);

		CurrentBlockDistanceToCenterX = DistanceXY.Key;
		CurrentBlockDistanceToCenterY = DistanceXY.Value;

		int32 CurrentQuarter;
		BelongingQuarterQueue.Dequeue(CurrentQuarter);
		SpawnBlock(BlockLocationStartEnd.Key + LastOriginOffset, BlockLocationStartEnd.Value + LastOriginOffset, BuildingIntensity, CurrentQuarter);
	}
}

void ACityGenerator::UpdateCityCenter(FVector Difference)
{
	// If origin offset changes more than once during city generation, buildings will be at wrong locations (city generation is fast, should never happen).
	LastOriginOffset = Difference;
	CityCenter += Difference;
	
	for (auto Grid : Grids)
	{
		Grid->GridCenter += Difference;
	}
}



// ============================================================ Another Thread ============================================================
CityGenerateTask::CityGenerateTask(AGrid* CityGrid_, ACityGenerator* CityGenerator_, FRotator Rotation_, int32 Quarter_, bool IsMenuCity_)
{
	this->CityGrid = CityGrid_;
	this->CityGenerator = CityGenerator_;
	this->Rotation = Rotation_;
	this->Quarter = Quarter_;
	this->IsMenuCity = IsMenuCity_;
}

CityGenerateTask::~CityGenerateTask()
{
	//UE_LOG(LogTemp, Warning, TEXT("CityGenerateTask::~CityGenerateTask Task Finished."));
}


// automatically called, name important
void CityGenerateTask::DoWork()
{	
	CanSpawn = false;

	FVector BlockStart;
	FVector BlockEnd;

	GeneratedBlockCount = 0;

	
	// endless loop here sometimes?
	while (true)
	{
		if (CityGenerator == nullptr){ return; }
		if (!IsMenuCity)
		{
			if (CityGenerator->EnvironmentGenerator == nullptr) 
			{ 
				return; 
			}
		}

		
		if (CityGrid == nullptr) { return; }

		// !! This may limit city radius - generated block amount. 
		CurrentIteration++;
		if (CurrentIteration > 1000)
		{
			break;
		}


		CanSpawn = CityGrid->FindSuitableLocationForBlock(2500.f, 2500.f, BlockStart, BlockEnd);
		if (CanSpawn)
		{
			FVector A = BlockStart + CityGenerator->CityCenter;
			FVector B = CityGenerator->CityCenter;

			FVector A_ = BlockEnd + CityGenerator->CityCenter;
			FVector B_ = CityGenerator->CityCenter;
	
			FVector Distance = BlockEnd;
			Distance.X *= CityGenerator->EllipseXRadiusDivident;
			Distance.Y *= CityGenerator->EllipseYRadiusDivident;

			float CurrentBlockDistanceToCenterX = BlockEnd.X * CityGenerator->EllipseXRadiusDivident;
			float CurrentBlockDistanceToCenterY = BlockEnd.Y * CityGenerator->EllipseYRadiusDivident;

			FVector Start = B + Rotation.RotateVector(B - A);
			FVector End = B_ + Rotation.RotateVector(B_ - A_);

			if (CityGenerator->EnvironmentGenerator != nullptr)
			{
				Start.Z = -CityGenerator->EnvironmentGenerator->WorldOriginTotalOffset.Z;
				End.Z = -CityGenerator->EnvironmentGenerator->WorldOriginTotalOffset.Z;
			}
			

			CityGenerator->BlockLocationsQueue.Enqueue(TTuple<FVector, FVector>(Start, End));
			CityGenerator->BlockDistancesQueue.Enqueue(TTuple<float, float>(CurrentBlockDistanceToCenterX, CurrentBlockDistanceToCenterY));
			CityGenerator->BelongingQuarterQueue.Enqueue(Quarter);
			GeneratedBlockCount++;

		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("CityGenerateTask::DoWork GeneratedBlockCount: %d"), GeneratedBlockCount);
			break;
		}
	}

}

