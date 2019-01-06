#pragma once

#include <string>
#include<opencv2/opencv.hpp>
#include "Coordinate.h"

//Definition for weightingFunction type.
//In order to use library's "setWeightingFunction" API, one should pass it 
//a poiter to a function that: receives 2 const by reference coordinates, and returns a float.
typedef float(*weightingFunction)(const Coordinate& u, const Coordinate& v);

class HoleFillingLib
{
public:
	enum class connectivityType
	{
		FOUR_CONNECTED = 4,
		EIGHT_CONNECTED = 8
	};

//Private Class Members
private:
	cv::Mat inputImgMat;
	weightingFunction wf;
	int z;
	float epsilon;
	connectivityType connectivity;

//Public Class Functions
public:
	HoleFillingLib();
	~HoleFillingLib();

	void fillHole(std::string imagePath);

	//Setters & Getters
	int getZ() const { return z; }
	void setZ(int value) { z = std::move(value); }
	float getEpsilon() const { return epsilon; }
	void setEpsilon(float value) { epsilon = std::move(value); }
	connectivityType getConnectivityType() const { return connectivity; }
	void setConnectivityType(connectivityType value) { connectivity = std::move(value); }
	void setWeightingFunction(weightingFunction value) { wf = value; } //In order to use default weighting function after it was changed ->
																	   //-> use setWeightingFunction(nullprt)

//Private Class Functions
private:
	float defaultWeightingFunction(const Coordinate& u, const Coordinate& v);
	float getWeight(const Coordinate& u, const Coordinate& v);
	void setBoundaryAndHoleVectors(std::vector<Coordinate> &B, std::vector<Coordinate> &H);
	bool isBoundaryPixel(const Coordinate& coor);
	bool isBoundaryPixel4Connected(const Coordinate& coor);
	bool isBoundaryPixel8Connected(const Coordinate& coor);
};

