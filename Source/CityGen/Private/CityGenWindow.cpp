#include "CityGenPch.h"
#include "CityGenWindow.h"
#include "CityGenerator.h"
#include "DrawDebugHelpers.h"
#include "CityGenerator.h"

#include "Engine/TriggerBox.h"
#include "ProceduralBuilding.h"

#include "Voronoi.h"
#include "VPoint.h"
#include <map>
#include <vector.h>
#include <algorithm>

#define LOCTEXT_NAMESPACE "CityGenWindow"

CityGenWindow::CityGenWindow() :
	BuildingsPerX(1),
	BuildingsPerY(1),
	AlleyWidth(10),
	HeightPercent(50),
	MinSizePercent(50),
	Spacing(10),
	BoundsActor(nullptr),
	AlleySpace(SAlleySpace::NO_ALLEY)
{
	float X = 0;
	float Y = 0;
	int32 j = 0;
	ReferencesPoints.Init(100);
	for (int32 i = 0; i < ReferencesPoints.Num(); i++)
	{
		ReferencesPoints[i].X = X;
		ReferencesPoints[i].Y = Y;
		X += 100;

		if (j == 10)
		{
			j = 0;
			Y += 100;
			X = 0;
		}
		j++;
		
	}
}

void CityGenWindow::Construct(const FArguments& Args)
{
	TSharedRef<SVerticalBox> VertBox = SNew(SVerticalBox);

	ChildSlot
		[
			VertBox
		];

	VertBox->AddSlot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock).Text(LOCTEXT("Radius", "Radius"))
			]

			+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				[
					SNew(SNumericEntryBox<uint16>)
					.AllowSpin(true)
					.MinValue(1).MaxValue(200)
					.MinSliderValue(1).MaxSliderValue(200)
					.Value(this, &CityGenWindow::GetNumberPoints)
					.OnValueChanged(this, &CityGenWindow::NumberPointsChanged)
				]
		];

	VertBox->AddSlot()
		.AutoHeight()
		[
			SNew(SButton)
			.Text(FString(TEXT("Generate City")))
			.OnClicked(this, &CityGenWindow::GenerateCity)
		];
}

UWorld* CityGenWindow::GetWorld()
{
	UWorld* World = NULL;
	const TIndirectArray<FWorldContext>& Contexts = GEngine->GetWorldContexts();
	for (int32 i = 0; i < Contexts.Num(); i++)
	{
		if (Contexts[i].WorldType == EWorldType::PIE)
		{
			return Contexts[i].World();
		}

		if (Contexts[i].WorldType == EWorldType::Editor)
		{
			World = Contexts[i].World();
		}
	}

	return World;
}

void CityGenWindow::BuildingsPerXChanged(const int32 Value)
{
	BuildingsPerX = Value;
}

TOptional<int32> CityGenWindow::GetBuildingsPerX() const
{
	return BuildingsPerX;
}

void CityGenWindow::BuildingsPerYChanged(const int32 Value)
{
	BuildingsPerY = Value;
}

TOptional<int32> CityGenWindow::GetBuildingsPerY() const
{
	return BuildingsPerY;
}

void CityGenWindow::AlleyWidthChanged(const float Value)
{
	AlleyWidth = Value;
}

TOptional<float> CityGenWindow::GetAlleyWidth() const
{
	return AlleyWidth;
}

void CityGenWindow::HeightPercentChanged(const float Value)
{
	HeightPercent = Value;
}

TOptional<float> CityGenWindow::GetHeightPercent() const
{
	return HeightPercent;
}

void CityGenWindow::MinSizePercentChanged(const float Value)
{
	MinSizePercent = Value;
}

TOptional<float> CityGenWindow::GetMinSizePercent() const
{
	return MinSizePercent;
}

void CityGenWindow::SpacingChanged(const float Value)
{
	Spacing = Value;
}

TOptional<float> CityGenWindow::GetSpacing() const
{
	return Spacing;
}

FString CityGenWindow::GetBoundsActorName() const
{
	if (!BoundsActor) 
		return FString::Printf(TEXT("No Actor Selected"));

	return BoundsActor->GetName();
}

FSlateColor CityGenWindow::GetBoundsActorColorAndOpacity() const
{
	if (!BoundsActor) 
		return FLinearColor(1, 0, 0);
	
	return FLinearColor(1, 1, 1);
}

void CityGenWindow::NumberPointsChanged(const uint16 Value)
{
	NumberPoints = Value;
}

TOptional<uint16> CityGenWindow::GetNumberPoints() const
{
	return NumberPoints;
}

FSlateColor CityGenWindow::GetBoundsActorForeground() const
{
	if (BoundsActorDropdown->IsHovered())
		return FEditorStyle::GetSlateColor("InvertedForeground");

	return FEditorStyle::GetSlateColor("DefaultForeground");
}

bool CityGenWindow::IsActorSelected(ATriggerBox* const Actor)
{
	return Actor == BoundsActor;
}

void CityGenWindow::SelectBoundsActor(ATriggerBox* Actor)
{
	BoundsActor = Actor;
}

TSharedRef<SWidget> CityGenWindow::GetBoundsActorContent()
{
	UWorld* World = GetWorld();
	FMenuBuilder Menu(true, NULL);

	for (TActorIterator<ATriggerBox> It(World); It; ++It)
	{
		Menu.AddMenuEntry(
			FText::FromString(It->GetName()),
			FText::FromString(It->GetName()),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateSP(this, &CityGenWindow::SelectBoundsActor, *It),
				FCanExecuteAction(),
				FIsActionChecked::CreateSP(this, &CityGenWindow::IsActorSelected, *It)
				),
			NAME_None,
			EUserInterfaceActionType::ToggleButton
			);
	}

	return Menu.MakeWidget();
}

FString CityGenWindow::GetAlleySpaceText() const
{
	switch (AlleySpace)
	{
	case SAlleySpace::NO_ALLEY:
		return FString::Printf(TEXT("No Alley"));
	case SAlleySpace::X_ALLEY:
		return FString::Printf(TEXT("X Alley"));
	case SAlleySpace::Y_ALLEY:
		return FString::Printf(TEXT("Y Alley"));
	}

	return FString::Printf(TEXT("ERROR"));
}

void CityGenWindow::SelectAlleySpace(const SAlleySpace::E Value)
{
	AlleySpace = Value;
}

bool CityGenWindow::IsAlleySpaceSelected(const SAlleySpace::E Value)
{
	return AlleySpace == Value;
}

TSharedRef<SWidget> CityGenWindow::GetAlleySpaceContent()
{
	FMenuBuilder Menu(true, NULL);

	Menu.AddMenuEntry(
		FText::FromString(TEXT("No Alley")),
		FText::FromString(TEXT("No Alley")),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateSP(this, &CityGenWindow::SelectAlleySpace, SAlleySpace::NO_ALLEY),
			FCanExecuteAction(),
			FIsActionChecked::CreateSP(this, &CityGenWindow::IsAlleySpaceSelected ,SAlleySpace::NO_ALLEY)),
		NAME_None,
		EUserInterfaceActionType::ToggleButton);

	Menu.AddMenuEntry(
		FText::FromString(TEXT("X Alley")),
		FText::FromString(TEXT("X Alley")),
		FSlateIcon(),
		FUIAction(
		FExecuteAction::CreateSP(this, &CityGenWindow::SelectAlleySpace, SAlleySpace::X_ALLEY),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(this, &CityGenWindow::IsAlleySpaceSelected, SAlleySpace::X_ALLEY)),
		NAME_None,
		EUserInterfaceActionType::ToggleButton);

	Menu.AddMenuEntry(
		FText::FromString(TEXT("Y Alley")),
		FText::FromString(TEXT("Y Alley")),
		FSlateIcon(),
		FUIAction(
		FExecuteAction::CreateSP(this, &CityGenWindow::SelectAlleySpace, SAlleySpace::Y_ALLEY),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(this, &CityGenWindow::IsAlleySpaceSelected, SAlleySpace::Y_ALLEY)),
		NAME_None,
		EUserInterfaceActionType::ToggleButton);

	return Menu.MakeWidget();
}

void CityGenWindow::CreateBrush(FVector Position, FVector Size, UWorld* World)
{
	UCubeBuilder* CubeBuilder = Cast<UCubeBuilder>(GEditor->FindBrushBuilder(UCubeBuilder::StaticClass()));
	CubeBuilder->X = Size.X;
	CubeBuilder->Y = Size.Y;
	CubeBuilder->Z = Size.Z;
	CubeBuilder->Build(World);

	World->GetBrush()->SetActorLocation(Position);
	GEditor->RedrawLevelEditingViewports();
	GEditor->Exec(World, TEXT("BRUSH ADD"));
}

FReply CityGenWindow::GenerateCity()
{
	const int32 count = 100;
	UWorld* World = GetWorld();
	
	double w = 1;
	int32 MaxPointDist = 1000;
	vor::Voronoi* Voronoi = new vor::Voronoi();
	vor::Vertices* Vertices = new vor::Vertices();
	vor::Edges* Edges = new vor::Edges();

	for (int32 i = 0; i < count; i++)
	{
		float x = ReferencesPoints[i].X + FMath::RandRange(-NumberPoints, NumberPoints);
		float y = ReferencesPoints[i].Y + FMath::RandRange(-NumberPoints, NumberPoints);
		
		Vertices->push_back(new VPoint(x, y));
	}

	Edges = Voronoi->GetEdges(Vertices, w, w);
	
	std::map<FVector2D, std::vector<FVector2D>> Cells;
	for (vor::Vertices::iterator i = Vertices->begin(); i != Vertices->end(); ++i)
	{
		FVector2D Vert = FVector2D((*i)->x, (*i)->y);
		DrawDebugPoint(World, FVector(Vert.X, Vert.Y, 200), 10, FColor(255, 0, 0, 255), false, 100);
		Cells.insert(std::make_pair(Vert, std::vector<FVector2D>()));
	}

	int32 BottomHeight = 200;

	for (vor::Edges::iterator i = Edges->begin(); i != Edges->end(); ++i)
	{
		FVector2D Start = FVector2D((*i)->start->x, (*i)->start->y);
		FVector2D End = FVector2D((*i)->end->x, (*i)->end->y);
		FVector2D LeftSite = FVector2D((*i)->left->x, (*i)->left->y);
		FVector2D RightSite = FVector2D((*i)->right->x, (*i)->right->y);
		
		FVector StartPoint = FVector(Start.X, Start.Y, BottomHeight);
		DrawDebugLine(World, StartPoint, FVector(End.X, End.Y, BottomHeight), FColor(255, 0, 0, 255), false, 100);
		DrawDebugLine(World, StartPoint, FVector(LeftSite.X, LeftSite.Y, BottomHeight), FColor(0, 255, 0, 255), false, 100);
		DrawDebugLine(World, StartPoint, FVector(RightSite.X, RightSite.Y, BottomHeight), FColor(0, 0, 255, 255), false, 100);

		if (FVector2D::Distance(Start, End) <= 200)
		{
			if (Cells.count(LeftSite) && LeftSite != FVector2D::ZeroVector)
			{
				Cells.at(LeftSite).push_back(Start);
				Cells.at(LeftSite).push_back(End);
			}

			if (Cells.count(RightSite) && RightSite != FVector2D::ZeroVector)
			{
				Cells.at(RightSite).push_back(Start);
				Cells.at(RightSite).push_back(End);
			}
		}
	}

	FVector Direction;
	for (auto MapItr = Cells.begin(); MapItr != Cells.end(); MapItr++)
	{
		FVector CoreLocation = FVector(MapItr->first.X, MapItr->first.Y, BottomHeight);

		vector<FVector2D> SortedPoints = MapItr->second;
		//SortedPoints.erase(unique(SortedPoints.begin(), SortedPoints.end()), SortedPoints.end());

		/*SortedPoints.push_back(MapItr->second[0]);
		for (auto PointItr = MapItr->second.begin(); PointItr != MapItr->second.end(); PointItr++)
		{
			for (auto NextPointItr = MapItr->second.begin(); NextPointItr != MapItr->second.end(); PointItr++)
			{
				if ((*NextPointItr))
			}
		}
*/
		TArray<FVector> BottomBuildingPoints;
		for (auto PointItr = SortedPoints.begin(); PointItr != SortedPoints.end(); PointItr++)
		{
			FVector PointLocation = FVector(PointItr->X, PointItr->Y, BottomHeight);
			Direction = CoreLocation - PointLocation;
			Direction.Normalize();

			FVector OffsetPosition = PointLocation + (30 * Direction);
			BottomBuildingPoints.Add(OffsetPosition);
		}

		AProceduralBuilding* Building = World->SpawnActor<AProceduralBuilding>();
		
		TArray<FProceduralMeshTriangle> Triangles;
		Building->Generate(BottomBuildingPoints, CoreLocation, Triangles);
	}

	return FReply::Handled();
}









	/*TArray<FProceduralMeshTriangle> Triangles;
	TArray<FVector> Points;
	Points.Add(FVector(10, 10, 200));
	Points.Add(FVector(20, 20, 200));
	Points.Add(FVector(20, 30, 200));
	Points.Add(FVector(10, 40, 200));
	Points.Add(FVector(0, 50, 200));
	Points.Add(FVector(5, 25, 200));
	AProceduralBuilding* Building = World->SpawnActor<AProceduralBuilding>();
	Building->Generate(Points, FVector(10, 25, 200), Triangles);
*/


//if (Cells.count(leftsite))
//{
//	if (leftsite->x != 0 && leftsite->y != 0 && FVector2D::Distance(LeftSite, Start) <= 200)
//	{
//		Cells.at(leftsite).push_back(*start);
//	}
//	if (leftsite->x != 0 && leftsite->y != 0 && FVector2D::Distance(LeftSite, End) <= 200)
//	{
//		Cells.at(leftsite).push_back(*end);
//	}
//}
//
//if (Cells.count(rightsite))
//{
//	if (start->x != 0 && start->y != 0 && FVector2D::Distance(RightSite, Start) <= 200)
//	{
//		Cells.at(rightsite).push_back(*start);
//	}
//	if (end->x != 0 && end->y != 0 && FVector2D::Distance(RightSite, End) <= 200)
//	{
//		Cells.at(rightsite).push_back(*end);
//	}
//}