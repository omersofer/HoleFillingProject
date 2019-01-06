#include "pch.h"
#include "CliUtility.h"

CliUtility::CliUtility()
{
}

CliUtility::~CliUtility()
{
}

void CliUtility::executeCli()
{
	//Help Variables
	HoleFillingLib holeFilling;
	std::string inputImagePath;
	std::string inputMaskPath;
	std::string holedImagePath;
	int z;
	float epsilon;
	HoleFillingLib::connectivityType connectivity;
	std::string inputLine = "";

	CliUtility::printWelcomeMessage();

	while (inputLine.compare("exit") != 0) {
		std::cout << ">> ";
		//Get input line and check for errors
		if (!std::getline(std::cin, inputLine)) {
			std::cout << "Something went wrong with I/O operations, exiting..." << std::endl;
			return;
		}
		if ((!inputLine.empty()) && (inputLine.compare("exit") != 0)) {
			//Parse input line and set parameters
			parseInputLine(inputLine, inputImagePath, inputMaskPath, z, epsilon, connectivity);

			//Build holed image
			constructHoledImage(inputImagePath, inputMaskPath, holedImagePath);

			//Set HoleFillingLib parameters
			holeFilling.setEpsilon(epsilon);
			holeFilling.setZ(z);
			holeFilling.setConnectivityType(connectivity);
			//Fill the hole in the holed image
			holeFilling.fillHole(holedImagePath);

			//Save the hole-filled image in the original format
			//Read the hole-filled image file as-is
			cv::Mat holeFilledImage = cv::imread(holedImagePath, -1);
			//Scale [0,1] range back to [0,255] range
			holeFilledImage.convertTo(holeFilledImage, CV_8UC1, 255);
			//Construct hole-filled image file name
			std::string origImageFormat = inputImagePath.substr(inputImagePath.find_last_of("."));
			std::string finalImagePath = holedImagePath.substr(0, holedImagePath.find_last_of(".") - 1) + "Filled" + origImageFormat;
			//Save output image in original format
			cv::imwrite(finalImagePath, holeFilledImage);
			//Remove holed image file
			remove(holedImagePath.c_str());
		}
	}
}

void CliUtility::printWelcomeMessage() {
	std::cout << "**********************************************************************************************************************" << std::endl;
	std::cout << "Welcome to Hole Filling CLI" << std::endl;
	std::cout << std::endl;
	std::cout << "In order to execute Hole Filling Algorithm - enter input parameters in the '>>' prompt below, in the following format:" << std::endl;
	std::cout << "<inputImagePath> <inputMaskPath> <z> <epsilon> <connectivityType>" << std::endl;
	std::cout << "Where:" << std::endl;
	std::cout << "	<inputImagePath> - Full path to input RGB image of any type" << std::endl;
	std::cout << "	<inputMaskPath> - Full path to mask RGB image of any type" << std::endl;
	std::cout << "			  Hole pixels are all black pixels" << std::endl;
	std::cout << "			  Mask image size is assumed to be equal to input image size" << std::endl;	
	std::cout << "	<z> - z parameter for default weighting function" << std::endl;
	std::cout << "	<epsilon> - epsilon parameter for default weighting function" << std::endl;
	std::cout << "	<connectivityType> - connectivity type for hole filling library, supported values are: '4' and '8'" << std::endl;
	std::cout << std::endl;
	std::cout << "In order to exit - enter 'exit'" << std::endl;
	std::cout << "**********************************************************************************************************************" << std::endl;
	std::cout << std::endl;
}

void CliUtility::parseInputLine(std::string& inputLine, std::string& inputImagePath, std::string& inputMaskPath, int& z, float& epsilon, HoleFillingLib::connectivityType& connectivity) {
	//Parse given input line into given parameter references
	//Note: assuming valid input according to the instructions printed to console by CliUtility::printWelcomeMessage()
	std::istringstream iss(inputLine);
	std::vector<std::string> tokens(std::istream_iterator<std::string>{iss},
		std::istream_iterator<std::string>());

	inputImagePath = tokens[0];
	inputMaskPath = tokens[1];
	z = std::stoi(tokens[2]);
	epsilon = float(std::atof(tokens[3].c_str()));
	connectivity = HoleFillingLib::connectivityType(std::stoi(tokens[4]));
}

void CliUtility::constructHoledImage(const std::string& inputImagePath, const std::string& inputMaskPath, std::string& holedImagePath){
	//Create Image with hole of .tiff format, each non-hole pixel will be a float in the range [0,1], hole pixels will be marked by -1 value
	cv::Mat inputImgMat = cv::imread(inputImagePath, 0);
	cv::Mat inputMaskMat = cv::imread(inputMaskPath, 0);
	cv::Mat holedImageMat;
	//Set holed image pixels to be floats in the range [0,1] according to inputImgMat pixels values
	inputImgMat.convertTo(holedImageMat, CV_32FC1); //set holedImageMat as float matrix with inputImgMat pixels values
	holedImageMat.convertTo(holedImageMat, CV_32FC1, 1.f / 255); //scale holedImageMat pixels values from [0,255] to [0,1]

	//Construct holed image matrix (use pointers instead of cv::Mat .at() function for efficiency)
	uchar* maskPtr;
	float* holedImagePtr;
	int holedImageMatRows = holedImageMat.rows; //if (matrix_number_of_channels != 1) need to add: *=number_of_channels
	int holedImageMatCols = holedImageMat.cols; //if (matrix_number_of_channels != 1) need to add: *=number_of_channels
	for (int i = 0; i < holedImageMatRows; i++)
	{
		maskPtr = inputMaskMat.ptr<uchar>(i);
		holedImagePtr = holedImageMat.ptr<float>(i);
		for (int j = 0; j < holedImageMatCols; j++)
		{
			if (maskPtr[j] == 0) {
				holedImagePtr[j] = -1;
			}
		}
	}

	//Construct holed .tiff image file name
	std::size_t lastDotPos = inputImagePath.find_last_of(".");
	holedImagePath = inputImagePath.substr(0, lastDotPos) + "_Holed.tiff";
	//Save the holed image to tiff output image file
	imwrite(holedImagePath, holedImageMat);
}

