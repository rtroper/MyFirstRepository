
#include "pch.h"

//  Global enumerations, useful for C-style implementations 

//  Every external function is called by GoldSim with specific requests including:
//    - initialization
//    - performing calculations
//    - obtaining the version number
//    - obtaining the number of input and output arguments
//    - "cleaning up" after the simulation (e.g. releasing memory and closing files)
//
//  XFMethodID (below) identifies the method types used to identify the phase of the simulation that is currently in progress.
//
//    XF_INITIALIZE    - Called after DLL is loaded and before each realization (no arguments are passed on this call). 
//    XF_CALCULATE     - Called during the simulation, each time the inputs change. 
//    XF_REP_VERSION   - Called after DLL load to report the external fcn version number. 
//    XF_REP_ARGUMENTS - Called after DLL load to report the number of input and output arguments. 
//    XF_CLEANUP       - Called before the DLL is unloaded. 

enum XFMethodID
{
	XF_INITIALIZE = 0,
	XF_CALCULATE = 1,
	XF_REP_VERSION = 2,
	XF_REP_ARGUMENTS = 3,
	XF_CLEANUP = 99
};

enum TSDefinitionConstants
{
	TS_START_IDX = 1,
	TS_SIZE_IDX = 8,
	TS_DATA_START_IDX = 9
};


// Declare global variables that persist between calls from GoldSim to the DLL


//  XFStatusID (below) identifies the return codes for external functions. 
//      
//    XF_SUCCESS          � Call completed successfully. 
//    XF_CLEANUP_NOW      - Call was successful, but GoldSim should clean up and unload the DLL immediately. 
//    XF_FAILURE          - Failure (no error information returned).  
//    XF_FAILURE_WITH_MSG � Failure, with DLL-supplied error message available. Address of error message is returned in the first element of the output arguments array. 
//    XF_INCREASE_MEMORY  - Failed because the memory allocated for output arguments is too small.  GoldSim will increase the size of the output argument array and try again. 

// Return codes:
//   0 indicates OK, continue GoldSim
//   >0 and <99 indicates to terminate GoldSim
//   99 indicates OK, unload the DLL
//
// The following codes can only be used for XF_CALCULATE:
//   -1 indicates fatal error and an error message pointer is returned
//   -2 indicates more result memory is required; total amount in doubles is returned in outargs[0]
enum XFStatusID
{
	XF_SUCCESS = 0,
	XF_FAILURE = 1,
	XF_CLEANUP_NOW = 99,
	XF_FAILURE_WITH_MSG = -1,
	XF_INCREASE_MEMORY = -2
};

//  When calling methods from a DLL, GoldSim always expects the following C/C++ function signature:
//     extern "C" void __declspec(dllexport) MyExternalFcn(int XFMethod, int* XFState, double* inargs, double* outargs)
//
//  Notes:
//    - The function name and argument names can be different from those shown above
//    - "C" specifies C language style linkage between GoldSim and the DLL
//    - __declspec(dllexport) makes the function visible outside the DLL
//    - XFMethod: specifies the action that the external function must perform
//    - XFState: a code indicating the status of the external function
//    - inargs: array of input arguments
//    - outargs: this array returns different information for different XFMethod values

// Import a hard-coded value into GoldSim
//-----------------------------------------------------------------------------------------------
extern "C" void __declspec(dllexport) BasicInitialConditionsImport(int methodID, int* status, double* inargs, double* outargs)
{
	*status = XF_SUCCESS;
	std::ofstream output_file;

	switch (methodID)
	{
	// Initialize (called at beginning of each realization); For example, allocate memory or open files
	// No arguments are passed on this calls
	case  XF_INITIALIZE:
		// Initialize global variables
		break;	// no initialization in this example

	// The external function reports its version
	// No input arguments are passed on this call.
	// outargs[0] is set equal to the external fcn version
	case  XF_REP_VERSION:
		outargs[0] = 1.01;
		break;

	// External fcn reports the number of input and output arguments
	// outargs[0] is set equal to the # of inputs arguments
	// outargs[1] is set equal to the # of output arguments
	case  XF_REP_ARGUMENTS:
		// See what GoldSim sends to the DLL when linked to a Time Series definition
		outargs[0] = 2.0;

		// Must import at least one value into GoldSim
		outargs[1] = 1.0;
		break;

	// Normal calculation.
	// Results are returned as outarg[0], outarg[1], etc. depending on number of outputs
	case  XF_CALCULATE:
		// A necessary (but not sufficient) condition for this to be a time series definition
		// is that the first value is equal to 20.
		if (int(inargs[TS_START_IDX]) == 20)
		{
			// Assume that this is a time series definition and get the number of data points
			int number_of_data_points = int(inargs[TS_SIZE_IDX]);

			// Load time and data values
			std::vector<double> times;
			std::vector<double> values;
			for (int i = 0; i < number_of_data_points; i++)
			{
				times.push_back(inargs[TS_DATA_START_IDX + i]);
				values.push_back(inargs[TS_DATA_START_IDX + number_of_data_points + i]);
			}

			// Write data to output file
			output_file.open("test_output_" + std::to_string(int(inargs[0])) + ".txt");
			//output_file.open("test_output.txt");
			for (size_t i = 0; i < times.size(); i++)
			{
				//output_file << std::to_string(inargs[i]) << std::endl;
				output_file << std::to_string(times[i]) << '\t' << std::to_string(values[i]) << std::endl;
			}
			output_file.close();
		}

		// Must import at least one value into GoldSim
		outargs[0] = 10.0;

		break;

	// Close any open files; Optionally release any memory that's been allocated
	// No arguments are passed on this call.
	case  XF_CLEANUP:
		
		break;

	// Error if this point is reached; This means the switch statement did not provide the cases that GoldSim expected.
	// The external function must have cases 0, 1, 2, 3 and 99
	default:
		*status = XF_FAILURE;
		break;
	}
}
