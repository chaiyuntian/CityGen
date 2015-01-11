/**
* Original Source: https://github.com/SRombauts/UE4ProceduralMesh/blob/master/Source/ProceduralMesh/ProceduralMeshComponent.cpp
*/

#include "CityGenPch.h"
#include "ProceduralMeshComponent.h"
#include "DynamicMeshBuilder.h"
#include "Runtime/Launch/Resources/Version.h"

/** Vertex Buffer */
class FProceduralMeshVertexBuffer : public FVertexBuffer
{
public:
	TArray<FDynamicMeshVertex> Vertices;

	virtual void InitRHI() override
	{
		FRHIResourceCreateInfo CreateInfo;
		VertexBufferRHI = RHICreateVertexBuffer(Vertices.Num() * sizeof(FDynamicMeshVertex), BUF_Static, CreateInfo);
		// Copy the vertex data into the vertex buffer.
		void* VertexBufferData = RHILockVertexBuffer(VertexBufferRHI, 0, Vertices.Num() * sizeof(FDynamicMeshVertex), RLM_WriteOnly);
		FMemory::Memcpy(VertexBufferData, Vertices.GetData(), Vertices.Num() * sizeof(FDynamicMeshVertex));
		RHIUnlockVertexBuffer(VertexBufferRHI);
	}
};

/** Index Buffer */
class FProceduralMeshIndexBuffer : public FIndexBuffer
{
public:
	TArray<int32> Indices;

	virtual void InitRHI() override
	{
		FRHIResourceCreateInfo CreateInfo;
		IndexBufferRHI = RHICreateIndexBuffer(sizeof(int32), Indices.Num() * sizeof(int32), BUF_Static, CreateInfo);
		// Write the indices to the index buffer.
		void* Buffer = RHILockIndexBuffer(IndexBufferRHI, 0, Indices.Num() * sizeof(int32), RLM_WriteOnly);
		FMemory::Memcpy(Buffer, Indices.GetData(), Indices.Num() * sizeof(int32));
		RHIUnlockIndexBuffer(IndexBufferRHI);
	}
};

/** Vertex Factory */
class FProceduralMeshVertexFactory : public FLocalVertexFactory
{
public:
	FProceduralMeshVertexFactory()
	{
	}

	/** Initialization */
	void Init(const FProceduralMeshVertexBuffer* VertexBuffer)
	{
		// Commented out to enable building light of a level (but no backing is done for the procedural mesh itself)
		//check(!IsInRenderingThread());

		ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
			InitProceduralMeshVertexFactory,
			FProceduralMeshVertexFactory*, VertexFactory, this,
			const FProceduralMeshVertexBuffer*, VertexBuffer, VertexBuffer,
			{
				// Initialize the vertex factory's stream components.
				DataType NewData;
				NewData.PositionComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, Position, VET_Float3);
				NewData.TextureCoordinates.Add(
					FVertexStreamComponent(VertexBuffer, STRUCT_OFFSET(FDynamicMeshVertex, TextureCoordinate), sizeof(FDynamicMeshVertex), VET_Float2)
					);
				NewData.TangentBasisComponents[0] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, TangentX, VET_PackedNormal);
				NewData.TangentBasisComponents[1] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, TangentZ, VET_PackedNormal);
				NewData.ColorComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, Color, VET_Color);
				VertexFactory->SetData(NewData);
			});
	}
};



/** Scene proxy */
class FProceduralMeshSceneProxy : public FPrimitiveSceneProxy
{
public:

	FProceduralMeshSceneProxy(UProceduralMeshComponent* Component)
		: FPrimitiveSceneProxy(Component) 
#if ENGINE_MAJOR_VERSION >= 4 && ENGINE_MINOR_VERSION >= 5
		, MaterialRelevance(Component->GetMaterialRelevance(GetScene().GetFeatureLevel()))
#else
		, MaterialRelevance(Component->GetMaterialRelevance())
#endif
	{
		// Add each triangle to the vertex/index buffer
		for (int TriIdx = 0; TriIdx<Component->ProceduralMeshTriangles.Num(); TriIdx++)
		{
			FProceduralMeshTriangle& Tri = Component->ProceduralMeshTriangles[TriIdx];

			const FVector Edge01 = (Tri.Vertex1.Position - Tri.Vertex0.Position);
			const FVector Edge02 = (Tri.Vertex2.Position - Tri.Vertex0.Position);

			const FVector TangentX = Edge01.SafeNormal();
			const FVector TangentZ = (Edge02 ^ Edge01).SafeNormal();
			const FVector TangentY = (TangentX ^ TangentZ).SafeNormal();

			FDynamicMeshVertex Vert0;
			Vert0.Position = Tri.Vertex0.Position;
			Vert0.Color = Tri.Vertex0.Color;
			Vert0.SetTangents(TangentX, TangentY, TangentZ);
			Vert0.TextureCoordinate.Set(Tri.Vertex0.U, Tri.Vertex0.V);
			int32 VIndex = VertexBuffer.Vertices.Add(Vert0);
			IndexBuffer.Indices.Add(VIndex);

			FDynamicMeshVertex Vert1;
			Vert1.Position = Tri.Vertex1.Position;
			Vert1.Color = Tri.Vertex1.Color;
			Vert1.SetTangents(TangentX, TangentY, TangentZ);
			Vert1.TextureCoordinate.Set(Tri.Vertex1.U, Tri.Vertex1.V);
			VIndex = VertexBuffer.Vertices.Add(Vert1);
			IndexBuffer.Indices.Add(VIndex);

			FDynamicMeshVertex Vert2;
			Vert2.Position = Tri.Vertex2.Position;
			Vert2.Color = Tri.Vertex2.Color;
			Vert2.SetTangents(TangentX, TangentY, TangentZ);
			Vert2.TextureCoordinate.Set(Tri.Vertex2.U, Tri.Vertex2.V);
			VIndex = VertexBuffer.Vertices.Add(Vert2);
			IndexBuffer.Indices.Add(VIndex);
		}

		// Init vertex factory
		VertexFactory.Init(&VertexBuffer);

		// Enqueue initialization of render resource
		BeginInitResource(&VertexBuffer);
		BeginInitResource(&IndexBuffer);
		BeginInitResource(&VertexFactory);

		// Grab material
		Material = Component->GetMaterial(0);
		if (Material == NULL)
		{
			Material = UMaterial::GetDefaultMaterial(MD_Surface);
		}
	}

	virtual ~FProceduralMeshSceneProxy()
	{
		VertexBuffer.ReleaseResource();
		IndexBuffer.ReleaseResource();
		VertexFactory.ReleaseResource();
	}

#if ENGINE_MAJOR_VERSION >= 4 && ENGINE_MINOR_VERSION >= 5
	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_ProceduralMeshSceneProxy_GetDynamicMeshElements);

		const bool bWireframe = AllowDebugViewmodes() && ViewFamily.EngineShowFlags.Wireframe;

		auto WireframeMaterialInstance = new FColoredMaterialRenderProxy(
			GEngine->WireframeMaterial ? GEngine->WireframeMaterial->GetRenderProxy(IsSelected()) : NULL,
			FLinearColor(0, 0.5f, 1.f)
			);

		Collector.RegisterOneFrameMaterialProxy(WireframeMaterialInstance);

		FMaterialRenderProxy* MaterialProxy = NULL;
		if (bWireframe)
		{
			MaterialProxy = WireframeMaterialInstance;
		}
		else
		{
			MaterialProxy = Material->GetRenderProxy(IsSelected());
		}

		for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
		{
			if (VisibilityMap & (1 << ViewIndex))
			{
				const FSceneView* View = Views[ViewIndex];
				// Draw the mesh.
				FMeshBatch& Mesh = Collector.AllocateMesh();
				FMeshBatchElement& BatchElement = Mesh.Elements[0];
				BatchElement.IndexBuffer = &IndexBuffer;
				Mesh.bWireframe = bWireframe;
				Mesh.VertexFactory = &VertexFactory;
				Mesh.MaterialRenderProxy = MaterialProxy;
				BatchElement.PrimitiveUniformBuffer = CreatePrimitiveUniformBufferImmediate(GetLocalToWorld(), GetBounds(), GetLocalBounds(), true, UseEditorDepthTest());
				BatchElement.FirstIndex = 0;
				BatchElement.NumPrimitives = IndexBuffer.Indices.Num() / 3;
				BatchElement.MinVertexIndex = 0;
				BatchElement.MaxVertexIndex = VertexBuffer.Vertices.Num() - 1;
				Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
				Mesh.Type = PT_TriangleList;
				Mesh.DepthPriorityGroup = SDPG_World;
				Mesh.bCanApplyViewModeOverrides = false;
				Collector.AddMesh(ViewIndex, Mesh);
			}
		}
	}
#endif

	virtual void DrawDynamicElements(FPrimitiveDrawInterface* PDI, const FSceneView* View)
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_ProceduralMeshSceneProxy_DrawDynamicElements);

		const bool bWireframe = AllowDebugViewmodes() && View->Family->EngineShowFlags.Wireframe;

		FColoredMaterialRenderProxy WireframeMaterialInstance(
			GEngine->WireframeMaterial ? GEngine->WireframeMaterial->GetRenderProxy(IsSelected()) : NULL,
			FLinearColor(0, 0.5f, 1.f)
			);

		FMaterialRenderProxy* MaterialProxy = NULL;
		if (bWireframe)
		{
			MaterialProxy = &WireframeMaterialInstance;
		}
		else
		{
			MaterialProxy = Material->GetRenderProxy(IsSelected());
		}

		// Draw the mesh.
		FMeshBatch Mesh;
		FMeshBatchElement& BatchElement = Mesh.Elements[0];
		BatchElement.IndexBuffer = &IndexBuffer;
		Mesh.bWireframe = bWireframe;
		Mesh.VertexFactory = &VertexFactory;
		Mesh.MaterialRenderProxy = MaterialProxy;

#if ENGINE_MAJOR_VERSION >= 4 && ENGINE_MINOR_VERSION >= 5
		BatchElement.PrimitiveUniformBuffer = CreatePrimitiveUniformBufferImmediate(GetLocalToWorld(), GetBounds(), GetLocalBounds(), true, UseEditorDepthTest());
#else
		BatchElement.PrimitiveUniformBuffer = CreatePrimitiveUniformBufferImmediate(GetLocalToWorld(), GetBounds(), GetLocalBounds(), true);
#endif

		BatchElement.FirstIndex = 0;
		BatchElement.NumPrimitives = IndexBuffer.Indices.Num() / 3;
		BatchElement.MinVertexIndex = 0;
		BatchElement.MaxVertexIndex = VertexBuffer.Vertices.Num() - 1;
		Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
		Mesh.Type = PT_TriangleList;
		Mesh.DepthPriorityGroup = SDPG_World;
		PDI->DrawMesh(Mesh);
	}

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View)
	{
		FPrimitiveViewRelevance Result;
		Result.bDrawRelevance = IsShown(View);
		Result.bShadowRelevance = IsShadowCast(View);
		Result.bDynamicRelevance = true;
		MaterialRelevance.SetPrimitiveViewRelevance(Result);
		return Result;
	}

	virtual bool CanBeOccluded() const override
	{
		return !MaterialRelevance.bDisableDepthTest;
	}

	virtual uint32 GetMemoryFootprint(void) const
	{
		return(sizeof(*this) + GetAllocatedSize());
	}

	uint32 GetAllocatedSize(void) const
	{
		return(FPrimitiveSceneProxy::GetAllocatedSize());
	}

private:

	UMaterialInterface* Material;
	FProceduralMeshVertexBuffer VertexBuffer;
	FProceduralMeshIndexBuffer IndexBuffer;
	FProceduralMeshVertexFactory VertexFactory;

	FMaterialRelevance MaterialRelevance;
};


//////////////////////////////////////////////////////////////////////////

UProceduralMeshComponent::UProceduralMeshComponent(const FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	PrimaryComponentTick.bCanEverTick = false;

	SetCollisionProfileName(UCollisionProfile::BlockAllDynamic_ProfileName);
}

bool UProceduralMeshComponent::SetProceduralMeshTriangles(const TArray<FProceduralMeshTriangle>& Triangles)
{
	ProceduralMeshTriangles = Triangles;

	// Need to recreate scene proxy to send it over
	MarkRenderStateDirty();

	return true;
}


FPrimitiveSceneProxy* UProceduralMeshComponent::CreateSceneProxy()
{
	FPrimitiveSceneProxy* Proxy = NULL;
	// Only if have enough triangles
	if (ProceduralMeshTriangles.Num() > 0)
	{
		Proxy = new FProceduralMeshSceneProxy(this); //TODO
	}
	return Proxy;
}


FBoxSphereBounds UProceduralMeshComponent::CalcBounds(const FTransform & LocalToWorld) const
{
	// Only if have enough triangles
	if (ProceduralMeshTriangles.Num() > 0)
	{
		// Minimum Vector: It's set to the first vertex's position initially (NULL == FVector::ZeroVector might be required and a known vertex vector has intrinsically valid values)
		FVector VecMin = ProceduralMeshTriangles[0].Vertex0.Position;

		// Maximum Vector: It's set to the first vertex's position initially (NULL == FVector::ZeroVector might be required and a known vertex vector has intrinsically valid values)
		FVector VecMax = ProceduralMeshTriangles[0].Vertex0.Position;

		// Get maximum and minimum X, Y and Z positions of vectors
		for (int32 TriangleIndex = 0; TriangleIndex < ProceduralMeshTriangles.Num(); TriangleIndex++)
		{
			VecMin.X = (VecMin.X > ProceduralMeshTriangles[TriangleIndex].Vertex0.Position.X) ? ProceduralMeshTriangles[TriangleIndex].Vertex0.Position.X : VecMin.X;
			VecMin.X = (VecMin.X > ProceduralMeshTriangles[TriangleIndex].Vertex1.Position.X) ? ProceduralMeshTriangles[TriangleIndex].Vertex1.Position.X : VecMin.X;
			VecMin.X = (VecMin.X > ProceduralMeshTriangles[TriangleIndex].Vertex2.Position.X) ? ProceduralMeshTriangles[TriangleIndex].Vertex2.Position.X : VecMin.X;

			VecMin.Y = (VecMin.Y > ProceduralMeshTriangles[TriangleIndex].Vertex0.Position.Y) ? ProceduralMeshTriangles[TriangleIndex].Vertex0.Position.Y : VecMin.Y;
			VecMin.Y = (VecMin.Y > ProceduralMeshTriangles[TriangleIndex].Vertex1.Position.Y) ? ProceduralMeshTriangles[TriangleIndex].Vertex1.Position.Y : VecMin.Y;
			VecMin.Y = (VecMin.Y > ProceduralMeshTriangles[TriangleIndex].Vertex2.Position.Y) ? ProceduralMeshTriangles[TriangleIndex].Vertex2.Position.Y : VecMin.Y;

			VecMin.Z = (VecMin.Z > ProceduralMeshTriangles[TriangleIndex].Vertex0.Position.Z) ? ProceduralMeshTriangles[TriangleIndex].Vertex0.Position.Z : VecMin.Z;
			VecMin.Z = (VecMin.Z > ProceduralMeshTriangles[TriangleIndex].Vertex1.Position.Z) ? ProceduralMeshTriangles[TriangleIndex].Vertex1.Position.Z : VecMin.Z;
			VecMin.Z = (VecMin.Z > ProceduralMeshTriangles[TriangleIndex].Vertex2.Position.Z) ? ProceduralMeshTriangles[TriangleIndex].Vertex2.Position.Z : VecMin.Z;

			VecMax.X = (VecMax.X < ProceduralMeshTriangles[TriangleIndex].Vertex0.Position.X) ? ProceduralMeshTriangles[TriangleIndex].Vertex0.Position.X : VecMax.X;
			VecMax.X = (VecMax.X < ProceduralMeshTriangles[TriangleIndex].Vertex1.Position.X) ? ProceduralMeshTriangles[TriangleIndex].Vertex1.Position.X : VecMax.X;
			VecMax.X = (VecMax.X < ProceduralMeshTriangles[TriangleIndex].Vertex2.Position.X) ? ProceduralMeshTriangles[TriangleIndex].Vertex2.Position.X : VecMax.X;

			VecMax.Y = (VecMax.Y < ProceduralMeshTriangles[TriangleIndex].Vertex0.Position.Y) ? ProceduralMeshTriangles[TriangleIndex].Vertex0.Position.Y : VecMax.Y;
			VecMax.Y = (VecMax.Y < ProceduralMeshTriangles[TriangleIndex].Vertex1.Position.Y) ? ProceduralMeshTriangles[TriangleIndex].Vertex1.Position.Y : VecMax.Y;
			VecMax.Y = (VecMax.Y < ProceduralMeshTriangles[TriangleIndex].Vertex2.Position.Y) ? ProceduralMeshTriangles[TriangleIndex].Vertex2.Position.Y : VecMax.Y;

			VecMax.Z = (VecMax.Z < ProceduralMeshTriangles[TriangleIndex].Vertex0.Position.Z) ? ProceduralMeshTriangles[TriangleIndex].Vertex0.Position.Z : VecMax.Z;
			VecMax.Z = (VecMax.Z < ProceduralMeshTriangles[TriangleIndex].Vertex1.Position.Z) ? ProceduralMeshTriangles[TriangleIndex].Vertex1.Position.Z : VecMax.Z;
			VecMax.Z = (VecMax.Z < ProceduralMeshTriangles[TriangleIndex].Vertex2.Position.Z) ? ProceduralMeshTriangles[TriangleIndex].Vertex2.Position.Z : VecMax.Z;
		}

		FVector vecOrigin = ((VecMax - VecMin) / 2) + VecMin;	/* Origin = ((Max Vertex's Vector - Min Vertex's Vector) / 2 ) + Min Vertex's Vector */
		FVector BoxPoint = VecMax - VecMin;			/* The difference between the "Maximum Vertex" and the "Minimum Vertex" is our actual Bounds Box */
		return FBoxSphereBounds(vecOrigin, BoxPoint, BoxPoint.Size()).TransformBy(LocalToWorld);
	}
	else
	{
		return FBoxSphereBounds();
	}
}
