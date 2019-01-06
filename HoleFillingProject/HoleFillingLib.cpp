#include "pch.h"
#include "HoleFillingLib.h"
#include <cmath>

HoleFillingLib::HoleFillingLib()
{
	//Set default values
	wf = nullptr;
	z = 2;
	epsilon = pow(10, -10);
	connectivity = connectivityType::FOUR_CONNECTED;
}

HoleFillingLib::~HoleFillingLib()
{
}

void HoleFillingLib::fillHole(std::string imagePath) {
	//Note: input image is assumed to be a valid input
	inputImgMat = cv::imread(imagePath, -1); //read the image file as-is
	std::vector<Coordinate> B;
	std::vector<Coordinate> H;
	
	//Find boundary and hole pixels. For simplicity - we assume that the image has only one hole.
	HoleFillingLib::setBoundaryAndHoleVectors(B, H);

	//Fill hole pixels according to given hole filling algorithm
	for (Coordinate& u : H) {
		float numeratorSum = 0;
		float denominatorSum = 0;
		float uvWeight;
		for (Coordinate v : B) {
			uvWeight = getWeight(u, v);
			numeratorSum += uvWeight * inputImgMat.at<float>(v.row, v.col);
			denominatorSum += uvWeight;
		}
		inputImgMat.at<float>(u.row, u.col) = float(numeratorSum / denominatorSum);
	}

	//Save the new image to the same input image file
	imwrite(imagePath, inputImgMat);
}

float HoleFillingLib::defaultWeightingFunction(const Coordinate& u, const Coordinate& v)
{
	float denominator = pow(u.euclideanDistance(v), z) + epsilon;
	return float(1/denominator);
}

float HoleFillingLib::getWeight(const Coordinate& u, const Coordinate& v)
{
	return (wf == nullptr ? HoleFillingLib::defaultWeightingFunction(u, v) : wf(u, v));
}

void HoleFillingLib::setBoundaryAndHoleVectors(std::vector<Coordinate> &B, std::vector<Coordinate> &H)
{
	B.clear();
	H.clear();

	//Iterate over input image matrix to build H and B(use pointer instead of cv::Mat .at() function for efficiency)
	float* inputImgPtr;
	int inputImgMatRows = inputImgMat.rows; //if (matrix_number_of_channels != 1) need to add: *=number_of_channels
	int inputImgMatCols = inputImgMat.cols; //if (matrix_number_of_channels != 1) need to add: *=number_of_channels
	for (int i = 0; i < inputImgMatRows; i++)
	{
		inputImgPtr = inputImgMat.ptr<float>(i);
		for (int j = 0; j < inputImgMatCols; j++)
		{
			Coordinate currCoordinate = Coordinate(i, j);
			if (inputImgPtr[j] == -1){
				H.push_back(currCoordinate);
			}
			else if (HoleFillingLib::isBoundaryPixel(currCoordinate)) {
				B.push_back(currCoordinate);
			}
		}
	}
}

bool HoleFillingLib::isBoundaryPixel(const Coordinate& coor)
{
	if (connectivity == connectivityType::FOUR_CONNECTED) {
		return HoleFillingLib::isBoundaryPixel4Connected(coor);
	}
	else if (connectivity == connectivityType::EIGHT_CONNECTED) {
		return HoleFillingLib::isBoundaryPixel8Connected(coor);
	}
	return false;
}

bool HoleFillingLib::isBoundaryPixel4Connected(const Coordinate& coor)
{
	if (((coor.row - 1 >= 0) && (inputImgMat.at<float>(coor.row - 1, coor.col) == -1)) ||
		((coor.row + 1 < inputImgMat.rows) && (inputImgMat.at<float>(coor.row + 1, coor.col) == -1)) ||
		((coor.col - 1 >= 0) && (inputImgMat.at<float>(coor.row, coor.col - 1) == -1)) ||
		((coor.col + 1 < inputImgMat.cols) && (inputImgMat.at<float>(coor.row, coor.col + 1) == -1))){
		return true;
	}
	return false;
}

bool HoleFillingLib::isBoundaryPixel8Connected(const Coordinate& coor)
{
	if ((isBoundaryPixel4Connected(coor)) || 
		((coor.row - 1 >= 0) && (coor.col - 1 >= 0) && (inputImgMat.at<float>(coor.row - 1, coor.col - 1) == -1)) ||
		((coor.row - 1 >= 0) && (coor.col + 1 < inputImgMat.cols) && (inputImgMat.at<float>(coor.row - 1, coor.col + 1) == -1)) ||
		((coor.row + 1 < inputImgMat.rows) && (coor.col - 1 >= 0) && (inputImgMat.at<float>(coor.row + 1, coor.col - 1) == -1)) ||
		((coor.row + 1 < inputImgMat.rows) && (coor.col + 1 < inputImgMat.cols) && (inputImgMat.at<float>(coor.row + 1, coor.col + 1) == -1))) {
		return true;
	}
	return false;
}

