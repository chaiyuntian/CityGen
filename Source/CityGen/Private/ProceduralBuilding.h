#pragma once

#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProceduralBuilding.generated.h"

UCLASS()
class AProceduralBuilding : public AActor
{
	GENERATED_UCLASS_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, Category = Materials)
	TSubobjectPtr<UProceduralMeshComponent> Mesh;

	void Generate(TArray<FVector> SortedPoints, FVector RootPoint, TArray<FProceduralMeshTriangle>& OutTriangles);
};