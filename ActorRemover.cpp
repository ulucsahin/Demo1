// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorRemover.h"
#include "Components/StaticMeshComponent.h"

// TODO: Proper way of skipping objects if we don't want to delete (in overlapbegin).

// Sets default values
AActorRemover::AActorRemover(const FObjectInitializer& ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	auto CollisionAreaMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));

	CollisionArea = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("SettlementArea")); // MeshComp from UsableActor parent class
	CollisionArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionArea->SetCollisionProfileName("OverlapAll");
	CollisionArea->SetupAttachment(SceneComponent);
	CollisionArea->SetStaticMesh(CollisionAreaMesh);
	CollisionArea->SetVisibility(true);
	
	RootComponent = SceneComponent;

}

// Called when the game starts or when spawned
void AActorRemover::BeginPlay()
{
	Super::BeginPlay();

	CollisionArea->OnComponentBeginOverlap.AddDynamic(this, &AActorRemover::OnOverlapBegin);
	CollisionArea->OnComponentEndOverlap.AddDynamic(this, &AActorRemover::OnOverlapEnd);

}

// Called every frame
void AActorRemover::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void AActorRemover::RemoveActor(AActor* ActorToRemove)
{
	ActorToRemove->Destroy();
}


void AActorRemover::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto OtherActorClassName = OtherActor->GetClass()->GetFName().ToString();
	
	if (OtherActor != this)
	{
		if (OtherActor != nullptr)
		{
			
			if (OtherActorClassName.Contains("Building") || OtherActorClassName.Contains("Skyscraper") || OtherActorClassName.Contains("SkyScraper"))
			{
				OtherActor->Destroy();
			}
		
		}
	}

}


void AActorRemover::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// nothing for now
}