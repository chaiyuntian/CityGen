/**
 * Source: https://wiki.unrealengine.com/Procedural_Mesh_Generation
 */
#pragma once

#include "ProceduralMeshComponent.generated.h"

USTRUCT()
struct FProceduralMeshVertex
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, Category = Triangle)
	FVector Position;

	UPROPERTY(EditAnywhere, Category = Triangle)
	FColor Color;

	UPROPERTY(EditAnywhere, Category = Triangle)
	float U;

	UPROPERTY(EditAnywhere, Category = Triangle)
	float V;
};

USTRUCT()
struct FProceduralMeshTriangle
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = Triangle)
	FProceduralMeshVertex Vertex0;

	UPROPERTY(EditAnywhere, Category = Triangle)
	FProceduralMeshVertex Vertex1;

	UPROPERTY(EditAnywhere, Category = Triangle)
	FProceduralMeshVertex Vertex2;
};

UCLASS(editinlinenew, meta = (BlueprintSpawnableComponent), ClassGroup = Rendering)
class UProceduralMeshComponent : public UMeshComponent
{
	GENERATED_UCLASS_BODY()

public:
	/** Set the geometry to use on this triangle mesh */
	UFUNCTION(BlueprintCallable, Category = "Components|ProceduralMesh")
	bool SetProceduralMeshTriangles(const TArray<FProceduralMeshTriangle>& Triangles);

	//UPrimitiveComponent interface.
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	//UMeshComponent interface.
	virtual int32 GetNumMaterials() const override { return 1; };
private:
	//USceneComponent interface.
	virtual FBoxSphereBounds CalcBounds(const FTransform & LocalToWorld) const override;

	TArray<FProceduralMeshTriangle> ProceduralMeshTriangles;

	friend class FProceduralMeshSceneProxy;
};