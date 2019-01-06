#pragma once

#include<iostream>
#include<stdio.h>
#include <string>
#include "HoleFillingLib.h"

class CliUtility
{
public:
	CliUtility();
	~CliUtility();
	void executeCli();

private:
	void printWelcomeMessage();
	void parseInputLine(std::string& inputLine, std::string& inputImagePath, std::string& inputMaskPath, int& z, float& epsilon, HoleFillingLib::connectivityType& connectivity);
	void constructHoledImage(const std::string& inputImagePath, const std::string& inputMaskPath, std::string& holedImagePath);
};

