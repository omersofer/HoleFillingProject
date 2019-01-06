#pragma once
#include <cmath>

class Coordinate
{
public:
	int row;
	int col;
	Coordinate(int row, int col);
	~Coordinate();

	float euclideanDistance(const Coordinate& other) const;
};

