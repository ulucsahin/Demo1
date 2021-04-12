// Fill out your copyright notice in the Description page of Project Settings.


#include "GameOverManager.h"
#include "Kismet/GameplayStatics.h"
#include "../ImpossibleGameInstance.h"
#include "../Cars/CustomCarUluc/CustomCarUluc.h"
#include "../GameModes/SubGameMode.h"
#include "../Profile/PlayerProfile.h"

// Sets default values for this component's properties
UGameOverManager::UGameOverManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGameOverManager::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UGameOverManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UGameOverManager::ShowGameOverMenu()
{
	UImpossibleGameInstance* GameInstance_ = Cast<UImpossibleGameInstance>(UGameplayStatics::GetGameInstance(World_));
	ACustomCarUluc* Player_ = Cast<ACustomCarUluc>(UGameplayStatics::GetPlayerPawn(World_, 0));
	auto PC = Cast<APlayerController>(Player_->GetController());

	PC->SetTickableWhenPaused(true);
	Player_->ShouldMuteEngine = true;
	UGameplayStatics::SetGamePaused(World_, true);

	// create and show game over menu
	FString MenuReference = "WidgetBlueprint'/Game/Uluc/Menus/GameOverMenu/GameOverMenu.GameOverMenu_C'"; // GameInstance_->GameMode->HUDReference;
	FStringClassReference MyWidgetClassRef(MenuReference);
	UClass* MyWidgetClass = MyWidgetClassRef.TryLoadClass<UUserWidget>();
	if (PC)
	{
		UUserWidget* HUD = CreateWidget<UUserWidget>(PC, MyWidgetClass);
		if (HUD)
		{
			HUD->AddToViewport();
		}

	}

	// Set Input Mode
	//PC->SetInputMode(FInputModeUIOnly());
	PC->bShowMouseCursor = true;

	// bring mouse to middle of screen
	FVector2D ViewportSize = FVector2D(1, 1);

	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FViewport* Viewport = CastChecked<ULocalPlayer>(PC->Player)->ViewportClient->Viewport;
	if (Viewport)
	{
		auto Resolution = Viewport->GetSizeXY();
		Viewport->SetMouse(int(Resolution.X / 2), int(Resolution.Y / 2));
	}
}

void UGameOverManager::CompleteRun()
{
	UE_LOG(LogTemp, Warning, TEXT("UGameOverManager::CompleteRun"));

	UImpossibleGameInstance* GameInstance_ = Cast<UImpossibleGameInstance>(UGameplayStatics::GetGameInstance(World_));
	ACustomCarUluc* Player_ = Cast<ACustomCarUluc>(UGameplayStatics::GetPlayerPawn(World_, 0));

	// Handle Game Mode 
	GameInstance_->GameMode->OnRunCompleted();

	// Save profile for latest changes
	FString ProfileName = GameInstance_->PlayerProfile->Name;
	GameInstance_->PlayerProfile->AdjustTotalDistance(); // to keep track of total distance driven lifetime
	GameInstance_->PlayerProfile->AddMoneyGainedOnRun();
	GameInstance_->PlayerProfile->UpdateMissionCompletionCount();
	GameInstance_->PlayerProfile->AddExp();
	GameInstance_->SaveGame(ProfileName);

	// Unpause game
	UGameplayStatics::SetGamePaused(World_, false);

	// Open Main Menu
	UGameplayStatics::OpenLevel(World_, "/Game/Uluc/Maps/MainMenu");

}