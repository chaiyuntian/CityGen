#include "CityGenPch.h"
#include "CityGenWindow.h"
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
				SNew(STextBlock)
				.Text(this, &CityGenWindow::GetBoundsActorName)
				.ColorAndOpacity(this, &CityGenWindow::GetBoundsActorColorAndOpacity)
			]

			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			[
				SAssignNew(BoundsActorDropdown, SComboButton)
					.ContentPadding(0)
					.ForegroundColor(this, &CityGenWindow::GetBoundsActorForeground)
					.OnGetMenuContent(this, &CityGenWindow::GetBoundsActorContent)
					.ButtonStyle(FEditorStyle::Get(), "ToggleButton")
					.ButtonContent()
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						[
							SNew(SImage).Image(FEditorStyle::GetBrush("GenericViewButton"))
						]

						+ SHorizontalBox::Slot()
							.AutoWidth()
							.VAlign(VAlign_Center)
							.Padding(2, 0, 0, 0)
							[
								SNew(STextBlock).Text(LOCTEXT("SelectedActorName", "SelectedActor"))
							]
					]
			]
		];

	VertBox->AddSlot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock).Text(LOCTEXT("AlleySpaceName", "Alley Space"))
			]

			+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				[
					SNew(SComboButton)
					.ContentPadding(0)
					.OnGetMenuContent(this, &CityGenWindow::GetAlleySpaceContent)
					.ButtonStyle(FEditorStyle::Get(), "ToggleButton")
					.ButtonContent()
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						[
							SNew(SImage).Image(FEditorStyle::GetBrush("GenericViewButton"))
						]

						+ SHorizontalBox::Slot()
							.AutoWidth()
							.VAlign(VAlign_Center)
							.Padding(2, 0, 0, 0)
							[
								SNew(STextBlock).Text(this, &CityGenWindow::GetAlleySpaceText)
							]
					]
				]
		];

	VertBox->AddSlot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock).Text(LOCTEXT("BuildingsPerXname", "Buildings X"))
			]

			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			[
				SNew(SNumericEntryBox<int32>)
				.AllowSpin(true)
				.MinValue(1).MaxValue(200)
				.MinSliderValue(1).MaxSliderValue(200)
				.Value(this, &CityGenWindow::GetBuildingsPerX)
				.OnValueChanged(this, &CityGenWindow::BuildingsPerXChanged)
			]
		];

	VertBox->AddSlot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock).Text(LOCTEXT("BuildingsPerYname", "Buildings Y"))
			]

			+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				[
					SNew(SNumericEntryBox<int32>)
					.AllowSpin(true)
					.MinValue(1).MaxValue(200)
					.MinSliderValue(1).MaxSliderValue(200)
					.Value(this, &CityGenWindow::GetBuildingsPerY)
					.OnValueChanged(this, &CityGenWindow::BuildingsPerYChanged)
				]
		];

	VertBox->AddSlot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock).Text(LOCTEXT("AlleyWidthName", "Alley Width"))
			]

			+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				[
					SNew(SNumericEntryBox<float>)
					.AllowSpin(true)
					.MinValue(1).MaxValue(200)
					.MinSliderValue(1).MaxSliderValue(200)
					.Value(this, &CityGenWindow::GetAlleyWidth)
					.OnValueChanged(this, &CityGenWindow::AlleyWidthChanged)
				]
		];

	VertBox->AddSlot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock).Text(LOCTEXT("HeightPercentName", "Height Percent"))
			]

			+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				[
					SNew(SNumericEntryBox<float>)
					.AllowSpin(true)
					.MinValue(1).MaxValue(100)
					.MinSliderValue(1).MaxSliderValue(100)
					.Value(this, &CityGenWindow::GetHeightPercent)
					.OnValueChanged(this, &CityGenWindow::HeightPercentChanged)
				]
		];

	VertBox->AddSlot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock).Text(LOCTEXT("MinSizePercentName", "MinSize Percent"))
			]

			+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				[
					SNew(SNumericEntryBox<float>)
					.AllowSpin(true)
					.MinValue(1).MaxValue(100)
					.MinSliderValue(1).MaxSliderValue(100)
					.Value(this, &CityGenWindow::GetMinSizePercent)
					.OnValueChanged(this, &CityGenWindow::MinSizePercentChanged)
				]
		];

	VertBox->AddSlot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock).Text(LOCTEXT("SpacingPercentName", "Spacing"))
			]

			+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				[
					SNew(SNumericEntryBox<float>)
					.AllowSpin(true)
					.MinValue(1).MaxValue(30)
					.MinSliderValue(1).MaxSliderValue(30)
					.Value(this, &CityGenWindow::GetSpacing)
					.OnValueChanged(this, &CityGenWindow::SpacingChanged)
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
	if (!BoundsActor)
		return FReply::Handled();

	FVector BoundsSize = BoundsActor->GetComponentsBoundingBox().GetExtent();
	FVector BoundsPosition = BoundsActor->GetActorLocation();
	FVector BoundsMin = BoundsPosition - BoundsSize;
	BoundsSize *= 2;	//Full size, not half size
	
	float XBuildingsBounds = (BoundsSize.X - (AlleySpace == SAlleySpace::X_ALLEY ? AlleyWidth : 0)) / (float)BuildingsPerX;
	float YBuildingsBounds = (BoundsSize.Y - (AlleySpace == SAlleySpace::Y_ALLEY ? AlleyWidth : 0)) / (float)BuildingsPerY;

	float AlleyPivot = 0;
	if (AlleySpace == SAlleySpace::X_ALLEY)
	{
		AlleyPivot = BuildingsPerX / 2.0f;
	}
	else if (AlleySpace == SAlleySpace::Y_ALLEY)
	{
		AlleyPivot = BuildingsPerY / 2.0f;
	}

	FVector Position, Size;
	float CalculateMinSizePercent = MinSizePercent / 100.0f;
	UWorld* World = GetWorld();
	for (int32 X = 0; X < BuildingsPerX; X++)
	{
		for (int32 Y = 0; Y < BuildingsPerY; Y++)
		{
			Size.X = FMath::FRandRange(XBuildingsBounds * CalculateMinSizePercent, XBuildingsBounds) - Spacing;
			Size.Y = FMath::FRandRange(YBuildingsBounds * CalculateMinSizePercent, YBuildingsBounds) - Spacing;
			Size.Z = FMath::FRandRange(BoundsSize.Z * CalculateMinSizePercent, BoundsSize.Z);

			Position.X = BoundsMin.X + (XBuildingsBounds * X) + Spacing + (Size.X / 2.0f);
			Position.Y = BoundsMin.Y + (YBuildingsBounds * Y) + Spacing + (Size.Y / 2.0f);
			Position.Z = BoundsMin.Z + (Size.Z / 2.0f);

			if (AlleySpace == SAlleySpace::X_ALLEY && X >= AlleyPivot)
			{
				Position.X += AlleyWidth;
			}
			else if(AlleySpace == SAlleySpace::Y_ALLEY && Y >= AlleyPivot)
			{
				Position.Y += AlleyWidth;
			}

			CreateBrush(Position, Size, World);
		}
	}

	return FReply::Handled();
}