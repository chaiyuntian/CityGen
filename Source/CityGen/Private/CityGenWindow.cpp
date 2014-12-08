#include "CityGenPch.h"
#include "CityGenWindow.h"
#include "CityGenerator.h"
#include "DrawDebugHelpers.h"
#include "CityGenerator.h"

#include "Engine/TriggerBox.h"

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
	float xValues[count];
	float yValues[count];
	UWorld* World = GetWorld();

	for (int32 i = 0; i < count; i++)
	{
		float x = ReferencesPoints[i].X + FMath::RandRange(-NumberPoints, NumberPoints);
		float y = ReferencesPoints[i].Y + FMath::RandRange(-NumberPoints, NumberPoints);

		DrawDebugPoint(World, FVector(ReferencesPoints[i].X, ReferencesPoints[i].Y, 100), 5, FColor(0, 0, 255, 255), false, 10);
		DrawDebugPoint(World, FVector(x, y, 100), 5, FColor(255, 0, 0, 255), false, 10);

		xValues[i] = x;
		yValues[i] = y;
	}

	VoronoiDiagramGenerator vdg;
	vdg.generateVoronoi(xValues, yValues, count, 0, 1000, 0, 1000, 0);
	vdg.resetIterator();
	
	float x1, y1, x2, y2;
	while (vdg.getNext(x1, y1, x2, y2))
	{
		UE_LOG(CityGen, Log, TEXT("GOT Line (%f,%f)->(%f,%f)"), x1, y1, x2, y2);
		DrawDebugLine(World, FVector(x1, y1, 100), FVector(x2, y2, 100), FColor(255, 0, 0, 255), false, 10);
	}

	return FReply::Handled();
}