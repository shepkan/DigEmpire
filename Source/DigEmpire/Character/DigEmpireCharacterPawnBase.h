// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DigEmpireCharacterPawnBase.generated.h"

class UDigComponent;

UCLASS()
class DIGEMPIRE_API ADigEmpireCharacterPawnBase : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ADigEmpireCharacterPawnBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
    void InputForward(float Value);
    void InputRight(float Value);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UDigComponent> DigComponent;

    // Dig input handlers
    void OnDigUpPressed();
    void OnDigUpReleased();
    void OnDigDownPressed();
    void OnDigDownReleased();
    void OnDigLeftPressed();
    void OnDigLeftReleased();
    void OnDigRightPressed();
    void OnDigRightReleased();
};
