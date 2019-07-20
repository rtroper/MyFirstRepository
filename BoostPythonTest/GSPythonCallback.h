#pragma once

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

// Declare global variables that persist between calls from GoldSim to the DLL


//  XFStatusID (below) identifies the return codes for external functions. 
//      
//    XF_SUCCESS          – Call completed successfully. 
//    XF_CLEANUP_NOW      - Call was successful, but GoldSim should clean up and unload the DLL immediately. 
//    XF_FAILURE          - Failure (no error information returned).  
//    XF_FAILURE_WITH_MSG – Failure, with DLL-supplied error message available. Address of error message is returned in the first element of the output arguments array. 
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

// Function to handle Python errors
std::string parse_python_exception();

// Signature for function to be called by GoldSim
extern "C" void __declspec(dllexport) Add(int methodID, int* status, double* inargs, double* outargs);
