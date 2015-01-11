#include "CityGenPch.h"
#include "ProceduralBuilding.h"

AProceduralBuilding::AProceduralBuilding(const FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	Mesh = PCIP.CreateDefaultSubobject<UProceduralMeshComponent>(this, TEXT("ProceduralCube"));

	//// Apply a simple material directly using the VertexColor as its BaseColor input
	//static ConstructorHelpers::FObjectFinder<UMaterialInterface> Material(TEXT("Material'/Game/Materials/BaseColor.BaseColor'"));
	//Mesh->SetMaterial(0, Material.Object);

	// Generate a cube
	//TArray<FProceduralMeshTriangle> Triangles;
	//Generate(100.f, Triangles);
	//Mesh->SetProceduralMeshTriangles(Triangles);

	RootComponent = Mesh;
}

void AProceduralBuilding::Generate(TArray<FVector> SortedPoints, FVector RootPoint, TArray<FProceduralMeshTriangle>& OutTriangles)
{
	FProceduralMeshVertex v0;
	FProceduralMeshVertex v1;
	FProceduralMeshVertex v2;
	FProceduralMeshVertex v3;
	v0.U = 0.f; v0.V = 0.f;
	v1.U = 0.f; v1.V = 1.f;
	v2.U = 1.f; v2.V = 1.f;
	v3.U = 1.f; v3.V = 0.f;

	FProceduralMeshTriangle t1;
	FProceduralMeshTriangle t2;
	FProceduralMeshTriangle roof;

	FVector RoofPoint = RootPoint;
	RoofPoint.Z += 50;

	for (int32 i = 0; i < SortedPoints.Num(); i++)
	{
		FVector BottomRight = SortedPoints[i];
		FVector TopRight = BottomRight;
		TopRight.Z += 50;

		int32 next = i + 1;
		if (next >= SortedPoints.Num())
		{
			next = 0;
		}
		FVector BottomLeft = SortedPoints[next];
		FVector TopLeft = BottomLeft;
		TopLeft.Z += 50;

		// front face
		v0.Position = BottomLeft;
		v1.Position = TopLeft;
		v2.Position = BottomRight;
		v3.Position = TopRight;
		t1.Vertex0 = v2;
		t1.Vertex1 = v1;
		t1.Vertex2 = v0;
		t2.Vertex0 = v2;
		t2.Vertex1 = v3;
		t2.Vertex2 = v1; 
		OutTriangles.Add(t1);
		OutTriangles.Add(t2);

		v0.Position = TopRight;
		v1.Position = TopLeft;
		v2.Position = RoofPoint;
		roof.Vertex0 = v2;
		roof.Vertex1 = v1;
		roof.Vertex2 = v0;
		OutTriangles.Add(roof);
	}

	Mesh->SetProceduralMeshTriangles(OutTriangles);
}