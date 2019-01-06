#include "pch.h"
#include "Coordinate.h"

Coordinate::Coordinate(int row, int col) : row(row), col(col) {}

Coordinate::~Coordinate(){}

float Coordinate::euclideanDistance(const Coordinate& other) const 
{
	return sqrt(pow((row - other.row),2) + pow((col - other.col),2));
}
