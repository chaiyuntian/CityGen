#include "CityGenPch.h"
#include "CityGenerator.h"
#include "DrawDebugHelpers.h"
#include "ProceduralBuilding.h"

void CityGenerator::Generate(int32 PointsX, int32 PointsY)
{
	if (bIsGenerated)
	{
		Clean();
	}

	if (PointsX != this->PointsX || PointsY != this->PointsY)
	{
		this->PointsX = PointsX;
		this->PointsY = PointsY;
		GenerateReferencedPoints();
	}

	
	
	bIsGenerated = true;
}

void CityGenerator::GenerateReferencedPoints()
{

}

void CityGenerator::Clean()
{

}