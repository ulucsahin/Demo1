// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeOfDayManager.h"
#include "Components/PostProcessComponent.h"
#include "Engine/PostProcessVolume.h"
#include "../ImpossibleGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Atmosphere/AtmosphericFog.h"
#include "EngineUtils.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Components/SkyLightComponent.h"
#include "Engine/ExponentialHeightFog.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Math/Color.h"
#include "../MapSettings/CustomSkySphere.h"
#include "Materials/MaterialInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "../Cars/CustomCarUluc/CustomCarUluc.h"
#include "Components/SpotLightComponent.h"
#include "Atmosphere/AtmosphericFogComponent.h"

// TODO: make hardcoded values data driven

// Sets default values for this component's properties
ATimeOfDayManager::ATimeOfDayManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryActorTick.bCanEverTick = true;

	
	//static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("/Game/Tropical_Jungle_Pack/Materials/Instances/Sky/MI_Sky_Day.MI_Sky_Day_C"));

	//if (Material.Object != NULL)
	//{
	//	SkyMaterialNight = (UMaterial*)Material.Object;
	//}
}


// Called when the game starts
void ATimeOfDayManager::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<UImpossibleGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	if (GameInstance != nullptr)
	{
		EnableTimeOfDay(GameInstance->TimeOfDay);
	}

	AdjustSunLocation();
	AdjustDirectionalLight();
	AdjustSkylight();
	AdjustExponentialHeightFog();
	AdjustHorizonColor();
	AdjustWorldObjects();
}


// Called every frame
void ATimeOfDayManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ...
}


void ATimeOfDayManager::EnableTimeOfDay(ETimeOfDay TimeOfDay)
{
	PostProcessVolumeMorning->bEnabled = false;
	PostProcessVolumeNoon->bEnabled = false;
	PostProcessVolumeEvening->bEnabled = false;
	PostProcessVolumeNight->bEnabled = false;

	if (TimeOfDay == ETimeOfDay::Morning)
	{
		PostProcessVolumeMorning->bEnabled = true;
	}
	else if (TimeOfDay == ETimeOfDay::Noon)
	{
		PostProcessVolumeNoon->bEnabled = true;
	}
	else if (TimeOfDay == ETimeOfDay::Evening)
	{
		PostProcessVolumeEvening->bEnabled = true;
	}
	else if (TimeOfDay == ETimeOfDay::Night)
	{
		PostProcessVolumeNight->bEnabled = true;
	}
	
}

void ATimeOfDayManager::AdjustSunLocation()
{
	// sets sun location and colors by adjusting atmospheric fog parameters

	auto TimeOfDay = GameInstance->TimeOfDay;

	// Get Atmospheric Fog (controls sun location)
	AAtmosphericFog* AFog = nullptr;
	for (TActorIterator<AAtmosphericFog> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AFog = Cast<AAtmosphericFog>(*ActorItr);
		if (AFog)
		{
			break;
		}
	}

	if (AFog != nullptr)
	{	

		// Adjust colors
		if (DefaultLightColors.Contains(TimeOfDay))
		{
			auto LightColor = DefaultLightColors[TimeOfDay];
			AFog->GetAtmosphericFogComponent()->SetDefaultLightColor(LightColor);
		}
		
		// Adjust Sun Location
		if (SunRotations.Contains(TimeOfDay))
		{
			auto SunRotation = SunRotations[TimeOfDay];
			AFog->SetActorRotation(SunRotation);
		}





		
	}
	
}

void ATimeOfDayManager::AdjustDirectionalLight()
{
	auto TimeOfDay = GameInstance->TimeOfDay;

	// Get Directional Light
	ADirectionalLight* DirectionalLight = nullptr;
	for (TActorIterator<ADirectionalLight> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		DirectionalLight = Cast<ADirectionalLight>(*ActorItr);
		if (DirectionalLight)
		{
			break;
		}
	}

	if (DirectionalLight != nullptr)
	{

		if (LightRotations.Contains(TimeOfDay))
		{
			auto LightRotation = LightRotations[TimeOfDay];
			DirectionalLight->GetLightComponent()->SetWorldRotation(LightRotation);
			
		}

	}

	if (TimeOfDay == ETimeOfDay::Noon)
	{
		DirectionalLight->GetLightComponent()->Intensity = 25.f;
		
	}
	if (TimeOfDay == ETimeOfDay::Evening)
	{
		DirectionalLight->GetLightComponent()->Intensity = 0.75f;
	}

	DirectionalLight->GetLightComponent()->UpdateColorAndBrightness();
}


void ATimeOfDayManager::AdjustSkylight()
{
	TMap<ETimeOfDay, float> SkyLightIntensities = {
		{ETimeOfDay::Morning, 0.5f},
		{ETimeOfDay::Noon, 1.0f},
		{ETimeOfDay::Evening, 0.5f},
		{ETimeOfDay::Night, 0.5f}
	};
	auto TimeOfDay = GameInstance->TimeOfDay;

	ASkyLight* SkyLight = nullptr;
	for (TActorIterator<ASkyLight> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		SkyLight = Cast<ASkyLight>(*ActorItr);
		if (SkyLight)
		{
			break;
		}
	}

	if (SkyLight != nullptr)
	{
		if (SkyLightIntensities.Contains(TimeOfDay))
		{
			SkyLight->GetLightComponent()->Intensity = SkyLightIntensities[TimeOfDay];
		}
		
	}

}

void ATimeOfDayManager::AdjustExponentialHeightFog()
{
	// AExponentialHeightFog 
	TMap<ETimeOfDay, FColor> FogColors = {
		{ETimeOfDay::Morning, FColor(1.f, 0.702251f, 0.652327f, 1.f)},
		{ETimeOfDay::Noon, FColor(1.f, 0.702251f, 0.652327f, 1.f)},
		{ETimeOfDay::Evening, FColor(1.f, 0.702251f, 0.652327f, 1.f)},
		{ETimeOfDay::Night, FColor(1.f, 0.702251f, 0.652327f, 1.f)}
	};

	auto TimeOfDay = GameInstance->TimeOfDay;

	AExponentialHeightFog* Fog = nullptr;
	for (TActorIterator<AExponentialHeightFog> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		Fog = Cast<AExponentialHeightFog>(*ActorItr);
		if (Fog)
		{
			break;
		}
	}

	if (Fog != nullptr)
	{
		UExponentialHeightFogComponent* FogComponent = Fog->GetComponent();
		if (FogColors.Contains(TimeOfDay))
		{
			FogComponent->SetFogInscatteringColor(FLinearColor(FogColors[TimeOfDay]));
		}
	}


}

void ATimeOfDayManager::AdjustHorizonColor()
{
	TMap<ETimeOfDay, UMaterialInstance*> FogColors = {
		{ETimeOfDay::Morning, SkyMaterialMorning},
		{ETimeOfDay::Noon, SkyMaterialNoon},
		{ETimeOfDay::Evening, SkyMaterialEvening},
		{ETimeOfDay::Night, SkyMaterialNight}
	};

	auto TimeOfDay = GameInstance->TimeOfDay;

	ACustomSkySphere* SkySphere = nullptr;
	for (TActorIterator<ACustomSkySphere> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		SkySphere = Cast<ACustomSkySphere>(*ActorItr);
		if (SkySphere)
		{
			break;
		}
	}

	if (SkySphere != nullptr) 
	{
		if (FogColors.Contains(TimeOfDay))
		{
			SkySphere->GetStaticMeshComponent()->SetMaterialByName(FName("MI_Sky_Cloud_Dislowland"), FogColors[TimeOfDay]);
		}
				
	}

	if (TimeOfDay == ETimeOfDay::Night)
	{
		SkySphere->SetHidden(true);
	}

}


void ATimeOfDayManager::AdjustWorldObjects()
{
	// Adjust player car lights
	auto Player_ = Cast<ACustomCarUluc>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	auto TimeOfDay = GameInstance->TimeOfDay;

	// Lights off on daytime
	if (TimeOfDay == ETimeOfDay::Morning || TimeOfDay == ETimeOfDay::Noon)
	{
		Player_->HeadLightLeft->SetIntensity(0.f);
		Player_->HeadLightLeft->SetAttenuationRadius(0.f);
		Player_->HeadLightRight->SetIntensity(0.f);
		Player_->HeadLightRight->SetAttenuationRadius(0.f);
	}
	else if (TimeOfDay == ETimeOfDay::Evening)
	{
		Player_->HeadLightLeft->SetIntensity(10.f);
		Player_->HeadLightLeft->SetAttenuationRadius(5000.f);
		Player_->HeadLightRight->SetIntensity(10.f);
		Player_->HeadLightRight->SetAttenuationRadius(5000.f);
	}
	else if (TimeOfDay == ETimeOfDay::Night)
	{
		Player_->HeadLightLeft->SetIntensity(50.f);
		Player_->HeadLightLeft->SetAttenuationRadius(20000.f);
		Player_->HeadLightRight->SetIntensity(50.f);
		Player_->HeadLightRight->SetAttenuationRadius(20000.f);
	}

	//Player_->HeadLightLeft->SetLightColor(FColor(0.991102f, 0.846873f, 0.152926f));
	//Player_->HeadLightRight->SetLightColor(FColor(0.991102f, 0.846873f, 0.152926f));

}