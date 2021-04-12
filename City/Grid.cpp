// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid.h"

// TODO: this class is highly inefficient, fix it.

// Sets default values
AGrid::AGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGrid::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGrid::InitializeGrid(float GridRadius_, float SquareSize_, float DistanceBetweenBlocks_, FVector GridCenter_, float EllipseXRadiusDivident_, float EllipseYRadiusDivident_)
{
	this->GridRadius = GridRadius_;
	this->SquareSize = SquareSize_;
	this->DistanceBetweenBlocks = DistanceBetweenBlocks_;
	this->GridCenter = GridCenter_;
	this->EllipseXRadiusDivident = EllipseXRadiusDivident_;
	this->EllipseYRadiusDivident = EllipseYRadiusDivident_;

	// Ensure that its multiple of 2
	int32 GridAmountX = FMath::FloorToInt(FMath::FloorToInt(GridRadius / SquareSize) / 2) * 2;
	int32 GridAmountY = FMath::FloorToInt(FMath::FloorToInt(GridRadius / SquareSize) / 2) * 2;

	OffsetX = GridAmountX / 2;
	OffsetY = GridAmountY / 2;

	// Bad way of initializing 2D array
	for (int32 i = 0; i < GridAmountX; i++)
	{
		TArray<bool> Temp;
		for (int32 j = 0; j < GridAmountY; j++)
		{
			Temp.Add(true);
		}

		Grid.Add(Temp);
	}
}

//void AGrid::ResetGrid()
//{
//	// Mark everywhere as true
//	for (int32 i = -OffsetX ; i <= OffsetX; i++)
//	{
//		for (int32 j = -OffsetY; j <= OffsetY; j++)
//		{
//			SetGridElement(i, j, true);
//		}
//	}
//
//	PermanentIteration = 0;
//
//	InitializeGrid(GridRadius, SquareSize, DistanceBetweenBlocks, GridCenter, EllipseXRadiusDivident, EllipseYRadiusDivident);
//}

bool AGrid::IsInBounds(int32 X, int32 Y)
{
	return X < OffsetX-1 && Y < OffsetY-1;
}

bool AGrid::GetGridElement(int32 X, int32 Y)
{
	// (0,0) is center (top right corner of square is at center) of Grid. So we will offset x and y when using array to get location in grid.
	// (0,0) coordinate (center) in Grid means (X + OffsetX, Y + OffsetY) in Array.

	if (!IsInBounds(X, Y)) return false;  //if (!IsInBounds(X + OffsetX, Y + OffsetY)) return false; //


	// CRASHES BECAUSE OF HERE ACCORDING TO LOGS
	//UE_LOG(LogTemp, Warning, TEXT("Grid.Num: %d, X + OffsetX: %d"), Grid.Num(), X + OffsetX);
	return Grid[X + OffsetX].Inner[Y + OffsetY];
}

void AGrid::SetGridElement(int32 X, int32 Y, bool Value)
{
	if (!IsInBounds(X, Y)) return;

	Grid[X + OffsetX].Inner[Y + OffsetY] = Value;
}

FVector AGrid::GetSquareCornerCoords(int32 X, int32 Y, int32 Corner)
{
	// Returns coords of corner of square at (X, Y) in Grid (not in array).
	// 0: Top Right, 1: Top Left, 2: Bottom Right, 3: Bottom Left (clockwise)
	
	if (Corner == 0)
	{
		return FVector(X * SquareSize, (Y + 1) * SquareSize, GridCenter.Z);
	}
	else if (Corner == 1)
	{
		return FVector((X + 1) * SquareSize, (Y + 1) * SquareSize, GridCenter.Z);
	}
	else if (Corner == 2)
	{
		return FVector((X + 1) * SquareSize, Y * SquareSize, GridCenter.Z);
	}
	else if (Corner == 3)
	{
		return FVector(X * SquareSize, Y * SquareSize, GridCenter.Z);
	}
	else
	{
		// Should never be here.
		return FVector();
	}

}

bool AGrid::FindSuitableLocationForBlock(float RangeX, float RangeY, FVector& ResultStart, FVector& ResultEnd)
{	
	// BlockStart and BlockEnd is used for getting block size. Result vectors specify location of the block.
	// returns true if succesfully finds a location and sets Results vectors.

	int32 SquareAmountX = FMath::Abs(FMath::CeilToInt(RangeX / SquareSize));
	int32 SquareAmountY = FMath::Abs(FMath::CeilToInt(RangeY / SquareSize));

	bool IsSuitable = false;
	
	int32 CurrentX = 0;
	int32 CurrentY = 0;
	int32 Iteration = PermanentIteration;
	int32 OriginalCurrentX = CurrentX;
	int32 OriginalCurrentY = CurrentY;
	int32 MaxIterations = (FMath::Abs(OffsetX - SquareAmountX)) * (FMath::Abs(OffsetY - SquareAmountY));

	while (!IsSuitable)
	{
		IsSuitable = CheckIfSquaresInRangeEmpty(CurrentX, CurrentX + SquareAmountX, CurrentY, CurrentY + SquareAmountY);
		
		CurrentX = OriginalCurrentX + (Iteration % OffsetX);
		CurrentY = OriginalCurrentY + FMath::FloorToInt(Iteration / OffsetX); // there may be some optimizations here by skipping y lines
		Iteration++;

		if (Iteration > MaxIterations)
		{
			PermanentIteration = OffsetX * CurrentY;
			return false;
		}

	}
	
	// for empty space between blocks
	int32 SquaresBetweenBlocks = FMath::FloorToInt(DistanceBetweenBlocks / SquareSize);
	MarkSquaresInRangeFalse(CurrentX, CurrentX + SquareAmountX + SquaresBetweenBlocks, CurrentY, CurrentY + SquareAmountY + SquaresBetweenBlocks);

	ResultStart = FVector(CurrentX * SquareSize, CurrentY  * SquareSize, 0.f);
	ResultEnd = FVector((CurrentX + SquareAmountX) * SquareSize, (CurrentY + SquareAmountY) * SquareSize, 0.f);

	return true;
}

bool AGrid::CheckIfSquaresInRangeEmpty(int32 XStart, int32 XEnd, int32 YStart, int32 YEnd)
{
	// Check if start-end in Grid(array)* bounds
	bool IsInBounds = FMath::Abs(XEnd - XStart) < OffsetX && FMath::Abs(YEnd - YStart) < OffsetY;
	if (!IsInBounds) return false;

	// Check if in radius (to achieve circular or ellipse city)
	// City is limited mainly by offset, if we form an ellipse by strecthing the circle we will be limited by offset and get a square instead. So we make an axis of circle smaller to achieve ellipse.
	int32 Distance = FMath::Sqrt(FMath::Square(FMath::Abs(XEnd * EllipseXRadiusDivident)) + FMath::Square(FMath::Abs(YEnd * EllipseYRadiusDivident)));
	IsInBounds = Distance < OffsetX;
	if (!IsInBounds) return false;

	bool IsAllEmpty = true;
	for (int32 i = XStart; i <= XEnd; i++)
	{
		for (int32 j = YStart; j <= YEnd; j++)
		{
			if (GetGridElement(i, j) == false)
			{
				IsAllEmpty = false;
				break;
			}
		}
		if (IsAllEmpty == false)
		{
			break;
		}
	}

	return IsAllEmpty;
}

void AGrid::MarkSquaresInRangeFalse(int32 XStart, int32 XEnd, int32 YStart, int32 YEnd)
{
	for (int32 i = XStart; i <= XEnd; i++)
	{
		for (int32 j = YStart; j <= YEnd; j++)
		{
			SetGridElement(i, j, false);
		}
	}

}


void AGrid::MarkRoadArea(int32 Start, int32 End, int32 Direction)
{
	/* Marks road area as false so there won't be any buildings on roads. 
	 Direction: 0 or 1
	*/
	if (Direction == 0)
	{
		MarkSquaresInRangeFalse(-OffsetX, OffsetX, Start, End);
	}
	if (Direction == 1)
	{
		MarkSquaresInRangeFalse(Start, End, -OffsetY, OffsetY);
	}
	

}