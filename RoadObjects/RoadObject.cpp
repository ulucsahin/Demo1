// Fill out your copyright notice in the Description page of Project Settings.

#include "RoadObject.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"

// Sets default values
ARoadObject::ARoadObject(const FObjectInitializer& ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	Mesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Mesh"));
	Mesh->SetupAttachment(SceneComponent);
	RootComponent = SceneComponent;
}

// Called when the game starts or when spawned
void ARoadObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARoadObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

