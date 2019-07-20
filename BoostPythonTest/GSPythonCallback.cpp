
#define BOOST_PYTHON_STATIC_LIB

#include <iostream>
#include <sstream>
#include "GSPythonCallback.h"
#include <boost/python.hpp>
#include <Python.h>

namespace py = boost::python;

py::object ignore;
std::stringstream ss;
std::string py_call;

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

// Calculate the spline-interpolated values (non-GSL) for given x and y input vectors of data
//-----------------------------------------------------------------------------------------------
extern "C" void __declspec(dllexport) Add(int methodID, int* status, double* inargs, double* outargs)
//extern "C" void Add(int methodID, int* status, double* inargs, double* outargs)
{
	*status = XF_SUCCESS;
	double a = 0.0, b = 0.0;

	py::object main_module;
	py::object main_namespace;
	py::object sum;

	switch (methodID)
	{
		// Initialize (called at beginning of each realization)
		// For example, allocate memory or open files
		// No arguments are passed on this call
	case  XF_INITIALIZE:
		// Initialize global variables
		Py_Initialize();
		main_module = py::import("__main__");
		main_namespace = main_module.attr("__dict__");
		try
		{
			ignore = py::exec(
				"hello = open('hello.txt', 'w')\n"
				"hello.write('Hello world!')\n"
				"hello.close()", main_namespace);
		}
		catch (py::error_already_set const&)
		{
			// Parse and output the exception
			std::string perror_str = parse_python_exception();
			std::cout << "Error in Python: " << perror_str << std::endl;
		}
		
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
		// Two values expected from GoldSim
		outargs[0] = 2.0;

		// One value to be returned to GoldSim
		outargs[1] = 1.0;
		break;

		// Normal calculation.
		// Results are returned as outarg[0], outarg[1], etc. depending on number of outputs
	case  XF_CALCULATE:
		// Get input arguments
		a = inargs[0];
		b = inargs[1];

		ss << a << " + " << b << std::endl;
		py_call = ss.str();

		// Return the sum of the two values
		try
		{
			sum = py::eval(py_call.c_str(), main_namespace);
			outargs[0] = py::extract<double>(sum);
		}
		catch (py::error_already_set const&)
		{
			// Parse and output the exception
			std::string perror_str = parse_python_exception();
			std::cout << "Error in Python: " << perror_str << std::endl;
		}

		break;

		// Close any open files
		// Optionally release any memory that's been allocated
		// No arguments are passed on this call.
	case  XF_CLEANUP:
		// Free allocated memory
		break;

		// Error if this point is reached
		// This means the switch statement did not provide the cases that GoldSim expected.
		// The external function must have cases 0, 1, 2, 3 and 99
	default:
		*status = XF_FAILURE;
		break;
	}
}

// Parses the value of the active python exception (NOTE SHOULD NOT BE CALLED IF NO EXCEPTION)
std::string parse_python_exception() {
	PyObject* type_ptr = NULL, * value_ptr = NULL, * traceback_ptr = NULL;

	// Fetch the exception info from the Python C API
	PyErr_Fetch(&type_ptr, &value_ptr, &traceback_ptr);

	// Fallback error
	std::string ret("Unfetchable Python error");

	// If the fetch got a type pointer, parse the type into the exception string
	if (type_ptr != NULL) {
		py::handle<> h_type(type_ptr);
		py::str type_pstr(h_type);

		// Extract the string from the boost::python object
		py::extract<std::string> e_type_pstr(type_pstr);

		// If a valid string extraction is available, use it otherwise use fallback
		if (e_type_pstr.check())
			ret = e_type_pstr();
		else
			ret = "Unknown exception type";
	}

	// Do the same for the exception value (the stringification of the exception)
	if (value_ptr != NULL) {
		py::handle<> h_val(value_ptr);
		py::str a(h_val);
		py::extract<std::string> returned(a);
		if (returned.check())
			ret += ": " + returned();
		else
			ret += std::string(": Unparseable Python error: ");
	}

	// Parse lines from the traceback using the Python traceback module
	if (traceback_ptr != NULL) {
		py::handle<> h_tb(traceback_ptr);

		// Load the traceback module and the format_tb function
		py::object tb(py::import("traceback"));
		py::object fmt_tb(tb.attr("format_tb"));

		// Call format_tb to get a list of traceback strings
		py::object tb_list(fmt_tb(h_tb));

		// Join the traceback strings into a single string
		py::object tb_str(py::str("\n").join(tb_list));

		// Extract the string, check the extraction, and fallback in necessary
		py::extract<std::string> returned(tb_str);
		if (returned.check())
			ret += ": " + returned();
		else
			ret += std::string(": Unparseable Python traceback");
	}
	return ret;
}
