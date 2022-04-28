// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy.generated.h"

//setting up delegate for casting to EnemySpawner when the enemy is destroyed
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPUOnEnemyDestruction);

UCLASS()
class RACING_GAME_API AEnemy : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemy();
    FPUOnEnemyDestruction OnEnemyDestruction;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    //called when hit, destroys actor
	void IsHit();
    

    FVector MoveDirection = FVector(1.f, 0.f, 0.f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        class USkeletalMeshComponent* SkeletalMesh {nullptr};

private:
    //Speed of enemy
    UPROPERTY(EditAnywhere, category="EnemyVariables")
        float Speed{ 5.f };

    //mnaximun turndelay (towards player)
    UPROPERTY(EditAnywhere, category = "EnemyVariables")
        float TurnDelayMax{ 5.f };

   //minimum turndelay (towards player)
    UPROPERTY(EditAnywhere, category = "EnemyVariables")
        float TurnDelayMin{ 1.f };

    UPROPERTY(EditAnywhere, category = "EnemyVariables")
    float CurrentTurnDelay{ 0.f };

    //collider component
    UPROPERTY(EditAnywhere)
        class UShapeComponent* Root {nullptr};


    //DeathFX, init through blueprint
    UPROPERTY(EditAnywhere, Category = "EnemyFX")
        UParticleSystem* ExplosionUponDeath = nullptr;
    
    UPROPERTY(EditAnywhere, Category = "EnemyFX")
        USoundBase* DeathSound = nullptr;
    UPROPERTY(EditAnywhere, Category = "EnemyAIController")
        class AAIController* AIController = nullptr;
   
};
