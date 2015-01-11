#include "CityGenPch.h"
#include "CityGenerator.h"
#include "DrawDebugHelpers.h"
#include "ProceduralBuilding.h"

CityGenerator::CityGenerator()
{

}

void CityGenerator::Generate(int32 BuildingsX = 10, int32 BuildingsY = 10)
{
	if (bIsGenerated)
	{
		Clean();
	}

	bIsGenerated = true;
}

void CityGenerator::Clean()
{

}