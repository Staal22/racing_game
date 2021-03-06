// Fill out your copyright notice in the Description page of Project Settings.


#include "AICar.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Coin.h"
#include "Shotgun.h"
#include "HealthPack.h"
#include "Enemy.h"
#include "EnemyC.h"
#include "Mine.h"
#include "Containers/UnrealString.h"
#include "RacingGameInstance.h"
#include "racing_gameGameModeBase.h"
#include <Kismet/GameplayStatics.h>
#include "Bullet.h"


// Sets default values
AAICar::AAICar()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SetRootComponent(Collision);

	AICarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AICarMesh"));
	AICarMesh->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void AAICar::BeginPlay()
{
	RacingGameMode = Cast<Aracing_gameGameModeBase>(GetWorld()->GetAuthGameMode());
	RacingGameInstance = Cast<URacingGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	Super::BeginPlay();

	SmoothRot = GetActorRotation();

	randInt2 = FMath::FRandRange(25, 28);

	ForceStrength = 1000.f * randInt2;
	ForceStrength2 = 50000.0f;
}

// Called every frame 
void AAICar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LineTrace();
	Time += DeltaTime;
	Turn += DeltaTime;
	BoostTime += DeltaTime;
	RotationCheck += DeltaTime;
	PUTime += DeltaTime;
	
	if (Time > 3.f)
	{
		Count = false;
	}
	/*
	FVector NewLocation = GetActorLocation();
	NewLocation += Collision->GetForwardVector() * Speed;
	SetActorLocation(NewLocation);
	*/
	if (Count == false)
	{
		if (MHit == true)
		{
			AICarMesh->SetRelativeRotation(FRotator(Turn * 180.f, 0.f, 0.f));
			if (Turn > 2.f)
			{
				MHit = false;
			}
			Collision->SetRelativeRotation(LastRotation);
		}
		if (MHit == false)
		{
			if (PUAct == true)
			{

				if (PUTime > randInt)
				{
					if (MisPU == true)
					{
						MissilePU();
					}
					if (MiPU == true)
					{
						MinePU();
					}
				}
			}
			Forward = Collision->GetForwardVector();

			AICarMesh->AddForce(Forward * FMath::FInterpTo(0.0f, ForceStrength, Time, InterpSpeed2) * AICarMesh->GetMass());

			MyRotator4 = MyRotator + MyRotator2 + MyRotator3;

			Collision->SetWorldRotation(FMath::RInterpTo(Collision->GetComponentRotation(), MyRotator4, Time, InterpSpeed));

			if (BoostTime > 3.f)
			{
				ForceStrength = 1000.f * randInt2;
			}
		}
	}
}

void AAICar::LineTrace()
{
	//Re-initialize hit info
	//call GetWorld() from within an actor extending class
	//down
	FHitResult* Hit = new FHitResult();
	FVector Start = GetActorLocation();
	FVector End = Start + FVector(0, 0, -HoverHeight);
	GetWorld()->LineTraceSingleByChannel(*Hit, Start, End, ECC_Visibility); //ECC_Pawn
	Ruler = Hit->Location - Start;
	//if (Hit)
	//{
	//	//UE_LOG(LogTemp, Warning, TEXT("123linetrace"))
	//	UKismetSystemLibrary::DrawDebugLine(GetWorld(), Start, End, FColor(100, 0, 0));
	//	UKismetSystemLibrary::DrawDebugSphere(GetWorld(), Hit->Location, 5, 5, FLinearColor::Red);
	//}
	//else
	//{
	//	//UE_LOG(LogTemp, Warning, TEXT("321linetrace"))
	//	UKismetSystemLibrary::DrawDebugLine(GetWorld(), Start, End, FColor(100, 0, 0));
	//	UKismetSystemLibrary::DrawDebugSphere(GetWorld(), End, 5, 5, FLinearColor::White);
	//}
	/*
	if (Ruler.Size() < HoverHeight-5)
	{
		FVector Up = AICarMesh->GetUpVector();

		AICarMesh->AddForce(Up * ForceStrength * AICarMesh->GetMass());
	}
	else if (Ruler.Size() > HoverHeight+5)
	{
		
		FVector NewLocation = GetActorLocation();
		NewLocation += GetActorUpVector() * Speed * -1;
		SetActorLocation(NewLocation);
		
	
	}
	*/
	
	//turn
	FCollisionQueryParams Params;

	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(Coin);
	Params.AddIgnoredActor(Shotgun);
	Params.AddIgnoredActor(Enemy);
	Params.AddIgnoredActor(EnemyC);
	Params.AddIgnoredActor(HP);

	FHitResult* Hit2 = new FHitResult();
	//FHitResult OutHitA;
	FVector End2 = Start + Collision->GetRightVector()*WallCheck;
	GetWorld()->LineTraceSingleByChannel(*Hit2, Start, End2, ECC_Visibility, Params); //ECC_Pawn
	RulerRight = Hit2->Location - Start;
	//OutHit1 = OutHitA;

	//UE_LOG(LogTemp, Error, TEXT("PhysicsHandle working %s"), *RulerRight.ToString());
	if (Hit2)
	{
		WallImpact = -Hit2->ImpactNormal;
		//UE_LOG(LogTemp, Warning, TEXT("Wallcheck"))
		/*	UKismetSystemLibrary::DrawDebugLine(GetWorld(), Start, End2, FColor(100, 0, 0));
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), Hit2->Location, 5, 5, FLinearColor::Red);*/
	}
	if (RulerRight.Size() < 600.0f)
	{
		FVector Left = Collision->GetRightVector() * -1;

		AICarMesh->AddForce(Left * ForceStrength2 * AICarMesh->GetMass());
	}

	FHitResult* Hit3 = new FHitResult();
	FVector End3 = Start + Collision->GetRightVector() * -1 * WallCheck;
	GetWorld()->LineTraceSingleByChannel(*Hit3, Start, End3, ECC_Visibility, Params); //ECC_Pawn
	RulerLeft = Hit3->Location - Start;
	//OutHit2 = OutHitA;

	if (RulerLeft.Size() < 600.0f)
	{
		FVector Right = Collision->GetRightVector();

		AICarMesh->AddForce(Right * ForceStrength2 * AICarMesh->GetMass());
	}

	if (RulerRight.Size() > 3499.0f)
	{
		WallImpact = Hit3->ImpactNormal;
		//FCollisionQueryParams Params2;
		//Params2.AddIgnoredActor(this);
		//FHitResult* Hit3 = new FHitResult();
		FRotator rot(-30, -30, -30);
		FVector DiagonalTrace = Collision->GetRightVector();
		FVector DV = rot.RotateVector(DiagonalTrace);
		UE_LOG(LogTemp, Error, TEXT("PhysicsHandle working %s"), *DV.ToString());
		//FVector End3 = Start + DV * WallCheck;
		//GetWorld()->LineTraceSingleByChannel(*Hit3, Start, End3, ECC_Visibility, Params2); //ECC_Pawn
		//Collision->SetWorldRotation(LastRotation);
	}
	//UE_LOG(LogTemp, Error, TEXT("Component %s"), *OutHitA.GetComponent()->GetName());
	//impact normal
	/*Hit2->ImpactNormal
		AICarMesh->GetUpVector()*/
	//FRotator MyRotator = FRotationMatrix::MakeFromZY(AICarMesh->GetUpVector(), Hit2->ImpactNormal).Rotator();
	//FRotator NewRot = UKismetMathLibrary::MakeRotFromYZ(-Hit2->ImpactNormal, Collision->GetUpVector());

	//if (OutHitA.GetComponent()->GetName() == Name)
	//{
		//UE_LOG(LogTemp, Error, TEXT("YEEEEEEEEEBOOOOOOOOOOOIIIIIIIIIIIII"));
		if (FMath::IsNearlyEqual(0.f, WallImpact.Z, 0.5f))
		{
			MyRotator = FRotationMatrix::MakeFromYZ(WallImpact, AICarMesh->GetUpVector()).Rotator();
			MyRotator.Pitch = 0;
			MyRotator.Roll = 0;
			MyRotator.Yaw -= 1;
		}

		MyRotator2 = FRotationMatrix::MakeFromZX(Hit->ImpactNormal, AICarMesh->GetForwardVector()).Rotator();
		MyRotator2.Pitch = 0;
		MyRotator2.Roll -= 1;
		MyRotator2.Yaw = 0;

		MyRotator3 = FRotationMatrix::MakeFromZY(Hit->ImpactNormal, AICarMesh->GetRightVector()).Rotator();
		MyRotator3.Pitch -= 1;
		MyRotator3.Roll = 0;
		MyRotator3.Yaw = 0;
	//}

	ROTTOT = AICarMesh->GetRelativeRotation();
	
}

void AAICar::SpeedBoost()
{
	BoostTime = 0.f;
	ForceStrength = 40000.0f;
	AICarMesh->AddImpulse(Forward * 500.f * AICarMesh->GetMass());
	NiagaraBoost();
}

void AAICar::Missile()
{
	LastRotation = Collision->GetRelativeRotation();
	
	ForceStrength = 0.0f;
	Up = Collision->GetUpVector();

	AICarMesh->AddImpulse(Up * 10000.f * AICarMesh->GetMass());
	AICarMesh->AddImpulse(Forward * -1 * 1000.f * AICarMesh->GetMass());

	MHit = true;
	Turn = 0.f;
}

void AAICar::MissilePU()
{
	UWorld* World = GetWorld();

	FVector Location = GetActorLocation();

	ABullet* Missile = World->SpawnActor<ABullet>(MissileSpawn, Location + FVector(0.f, -1000.f, 0.f), GetActorRotation());

	PUAct = false;
	MisPU = false;
}

void AAICar::MinePU()
{
	UWorld* World = GetWorld();

	FVector Location = GetActorLocation();

	AMine* Mine = World->SpawnActor<AMine>(MineSpawn, Location + FVector(0.f, 1000.f, 0.f), GetActorRotation());

	PUAct = false;
	MiPU = false;
	//UE_LOG(LogTemp, Warning, TEXT("MINE SPAWNED"));
}

void AAICar::PUActive()
{
	randInt = FMath::RandRange(3, 7);
	PUTime = 0.f;
	PUAct = true;
}

void AAICar::MissileFlip()
{
	MisPU = true;
}

void AAICar::MineFlip()
{
	MiPU = true;
}
