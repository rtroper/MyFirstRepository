// JTurnerTutorial.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define BOOST_PYTHON_STATIC_LIB

#include <iostream>
#include <boost/python.hpp>
#include <Python.h>

namespace py = boost::python;

std::string parse_python_exception();

int main()
{
    std::cout << "Hello World! (from C++)\n";

	Py_Initialize();
	py::object main_module = py::import("__main__");
	py::object main_namespace = main_module.attr("__dict__");

	py::object ignore;

	try
	{
		ignore = py::exec("print('Hello, world!! (from Python)')", main_namespace);
		ignore = py::exec("print('Hello, world'[3:5])", main_namespace);
		ignore = py::exec("print('.'.join(['1','2','3']))", main_namespace);
	}
	catch (py::error_already_set const&)
	{
		// Parse and output the exception
		std::string perror_str = parse_python_exception();
		std::cout << "Error in Python: " << perror_str << std::endl;
	}
	
	ignore = py::exec("import random", main_namespace);
	py::object rand = py::eval("random.random()", main_namespace);
	std::cout << py::extract<double>(rand) << std::endl;

	py::object rand_mod = py::import("random");
	py::object rand_func = rand_mod.attr("random");
	py::object rand2 = rand_func();
	std::cout << py::extract<double>(rand2) << std::endl;

	try {
		//Py_Initialize();
		py::object rand_mod = py::import("fake_module");
	}
	catch (py::error_already_set const&) {
		// Parse and output the exception
		std::string perror_str = parse_python_exception();
		std::cout << "Error in Python: " << perror_str << std::endl;
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

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
