// Fill out your copyright notice in the Description page of Project Settings.


#include "DigEmpireCharacterPawnBase.h"


// Sets default values
ADigEmpireCharacterPawnBase::ADigEmpireCharacterPawnBase()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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

