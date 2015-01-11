#pragma once

#include "Core.h"

class CityGenerator
{
	CityGenerator(){};
	void Generate(int32 PointsX = 10, int32 PointsY = 10);
	void GenerateReferencedPoints();
	void Clean();

private:
	bool bIsGenerated;
	int32 PointsX;
	int32 PointsY;
};