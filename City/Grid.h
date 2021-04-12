// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grid.generated.h"

USTRUCT()
struct FBoolArrayStruct
{
	GENERATED_BODY()

	FBoolArrayStruct() : Inner({}) {}

	// constructor that takes 1 parameters
	FBoolArrayStruct(const TArray<bool>& InInner) : Inner(InInner) { }

	TArray<bool> Inner;
};

UCLASS()
class THEIMPOSSIBLE_API AGrid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrid();
	TArray<FBoolArrayStruct> Grid;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	float SquareSize;
	float GridRadius; 
	float DistanceBetweenBlocks;
	FVector GridCenter;
	int32 OffsetX = 0;
	int32 OffsetY = 0;
	int32 PermanentIteration = 0;
	float EllipseYRadiusDivident = 1.f;
	float EllipseXRadiusDivident = 1.f;

	void InitializeGrid(float GridRadius_, float SquareSize_, float DistanceBetweenBlocks_, FVector GridCenter_, float EllipseXRadiusDivident_, float EllipseYRadiusDivident_);
	//void ResetGrid();
	bool IsInBounds(int32 X, int32 Y);
	bool GetGridElement(int32 X, int32 Y);
	void SetGridElement(int32 X, int32 Y, bool Value);
	
	FVector GetSquareCornerCoords(int32 X, int32 Y, int32 Corner);
	bool FindSuitableLocationForBlock(float RangeX, float RangeY, FVector& ResultStart, FVector& ResultEnd);
	bool CheckIfSquaresInRangeEmpty(int32 XStart, int32 XEnd, int32 YStart, int32 YEnd);
	void MarkSquaresInRangeFalse(int32 XStart, int32 XEnd, int32 YStart, int32 YEnd);
	void MarkRoadArea(int32 Start, int32 End, int32 Direction);

};
