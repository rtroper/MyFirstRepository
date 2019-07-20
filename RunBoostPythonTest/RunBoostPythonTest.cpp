// RunBoostPythonTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include "GSPythonCallback.h"

int main()
{
    std::cout << "Hello World!\n";

	int status = XFStatusID::XF_SUCCESS;
	std::vector<double> inargs{ 10.0, 20.0 };
	std::vector<double> outargs{ 0.0 };

	Add(XFMethodID::XF_INITIALIZE, &status, inargs.data(), outargs.data());
	Add(XFMethodID::XF_CALCULATE, &status, inargs.data(), outargs.data());

	std::cout << inargs[0] << " + " << inargs[1] << " = " << outargs[0] << std::endl;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
