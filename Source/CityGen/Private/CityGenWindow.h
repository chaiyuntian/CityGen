#pragma once

class ATriggerBox;

struct SAlleySpace
{
	enum E
	{
		NO_ALLEY,
		X_ALLEY,
		Y_ALLEY
	};
};

class CityGenWindow : public SCompoundWidget
{
public:
	CityGenWindow();

	SLATE_BEGIN_ARGS(CityGenWindow) {}
	SLATE_END_ARGS()

	/**
	* Constructs the initial window GUI.
	* @param[in] Args	-	The arguments to create the widget
	*/
	void Construct(const FArguments& Args);

	UWorld* GetWorld();

private:

	void BuildingsPerXChanged(const int32 Value);
	TOptional<int32> GetBuildingsPerX() const;

	void BuildingsPerYChanged(const int32 Value);
	TOptional<int32> GetBuildingsPerY() const;

	void AlleyWidthChanged(const float Value);
	TOptional<float> GetAlleyWidth() const;

	void HeightPercentChanged(const float Value);
	TOptional<float> GetHeightPercent() const;

	void MinSizePercentChanged(const float Value);
	TOptional<float> GetMinSizePercent() const;

	void SpacingChanged(const float Value);
	TOptional<float> GetSpacing() const;

	//Parameters
	int32 BuildingsPerX;
	int32 BuildingsPerY;
	float AlleyWidth;
	float HeightPercent;
	float MinSizePercent;
	float Spacing;
	
	//BoundActor
	FString GetBoundsActorName() const;
	FSlateColor GetBoundsActorColorAndOpacity() const;
	FSlateColor GetBoundsActorForeground() const;
	bool IsActorSelected(ATriggerBox* const Actor);
	void SelectBoundsActor(ATriggerBox* Actor);
	TSharedRef<SWidget> GetBoundsActorContent();
	TSharedPtr<SComboButton> BoundsActorDropdown;
	ATriggerBox* BoundsActor; //Boundaries of the city

	//AlleySpace
	SAlleySpace::E AlleySpace;
	FString GetAlleySpaceText() const;
	void SelectAlleySpace(const SAlleySpace::E Value);
	bool IsAlleySpaceSelected(const SAlleySpace::E Value);
	TSharedRef<SWidget> GetAlleySpaceContent();

	//Creating
	void CreateBrush(FVector Position, FVector Size, UWorld* World);
	FReply GenerateCity();
	
	//The New stuff
	void NumberPointsChanged(const uint16 Value);
	TOptional<uint16> GetNumberPoints() const;
	uint16 NumberPoints;

	TArray<FVector> ReferencesPoints;
};