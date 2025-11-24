// Fill out your copyright notice in the Description page of Project Settings.


#include "DigEmpireCharacterPawnBase.h"

#include "DigEmpire/Character/DigComponent.h"
#include "DigEmpire/Character/DwarfLightComponent.h"


// Sets default values
ADigEmpireCharacterPawnBase::ADigEmpireCharacterPawnBase()
{
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    DigComponent = CreateDefaultSubobject<UDigComponent>(TEXT("DigComponent"));
    DwarfLightComponent = CreateDefaultSubobject<UDwarfLightComponent>(TEXT("DwarfLightComponent"));
}

// Called when the game starts or when spawned
void ADigEmpireCharacterPawnBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADigEmpireCharacterPawnBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ADigEmpireCharacterPawnBase::SetupPlayerInputComponent(UInputComponent* IC)
{
    Super::SetupPlayerInputComponent(IC);

    IC->BindAxis("MoveForward", this, &ADigEmpireCharacterPawnBase::InputForward);
    IC->BindAxis("MoveRight",   this, &ADigEmpireCharacterPawnBase::InputRight);

    // Dig actions (define in Project Settings or DefaultInput.ini): Arrow keys
    IC->BindAction("DigUp",    IE_Pressed,  this, &ADigEmpireCharacterPawnBase::OnDigUpPressed);
    IC->BindAction("DigUp",    IE_Released, this, &ADigEmpireCharacterPawnBase::OnDigUpReleased);
    IC->BindAction("DigDown",  IE_Pressed,  this, &ADigEmpireCharacterPawnBase::OnDigDownPressed);
    IC->BindAction("DigDown",  IE_Released, this, &ADigEmpireCharacterPawnBase::OnDigDownReleased);
    IC->BindAction("DigLeft",  IE_Pressed,  this, &ADigEmpireCharacterPawnBase::OnDigLeftPressed);
    IC->BindAction("DigLeft",  IE_Released, this, &ADigEmpireCharacterPawnBase::OnDigLeftReleased);
    IC->BindAction("DigRight", IE_Pressed,  this, &ADigEmpireCharacterPawnBase::OnDigRightPressed);
    IC->BindAction("DigRight", IE_Released, this, &ADigEmpireCharacterPawnBase::OnDigRightReleased);
}

void ADigEmpireCharacterPawnBase::InputForward(float Value)
{
	if (Value == 0.f) return;
	AddMovementInput(FVector::RightVector, Value); // +Y
}

void ADigEmpireCharacterPawnBase::InputRight(float Value)
{
    if (Value == 0.f) return;
    AddMovementInput(FVector::ForwardVector, Value);   // +X
}

void ADigEmpireCharacterPawnBase::OnDigUpPressed()    { if (DigComponent) DigComponent->StartDigUp(); }
void ADigEmpireCharacterPawnBase::OnDigUpReleased()   { if (DigComponent) DigComponent->StopDig(); }
void ADigEmpireCharacterPawnBase::OnDigDownPressed()  { if (DigComponent) DigComponent->StartDigDown(); }
void ADigEmpireCharacterPawnBase::OnDigDownReleased() { if (DigComponent) DigComponent->StopDig(); }
void ADigEmpireCharacterPawnBase::OnDigLeftPressed()  { if (DigComponent) DigComponent->StartDigLeft(); }
void ADigEmpireCharacterPawnBase::OnDigLeftReleased() { if (DigComponent) DigComponent->StopDig(); }
void ADigEmpireCharacterPawnBase::OnDigRightPressed() { if (DigComponent) DigComponent->StartDigRight(); }
void ADigEmpireCharacterPawnBase::OnDigRightReleased(){ if (DigComponent) DigComponent->StopDig(); }

