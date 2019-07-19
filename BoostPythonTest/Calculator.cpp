//  Copyright Joel de Guzman 2002-2004. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt
//  or copy at http://www.boost.org/LICENSE_1_0.txt)
//  Hello World Example from the tutorial
//  [Joel de Guzman 10/9/2002]

#define BOOST_PYTHON_STATIC_LIB

#include <iostream>
#include <boost/python.hpp>

class Calculator
{
public:
	Calculator() {}
	double add(double a, double b) { return a + b; };
	double subtract(double a, double b) { return a - b; };
	double multiply(double a, double b) { return a * b; };
	double divide(double a, double b) { return a / b; };

	void sayHello() { std::cout << "Hello, Python!\n"; }
};

BOOST_PYTHON_MODULE(BoostPythonTest)
{
	using namespace boost::python;
	
	class_<Calculator>("Calculator", init<>())
		.def("add", &Calculator::add)
		.def("subtract", &Calculator::subtract)
		.def("multiply", &Calculator::multiply)
		.def("divide", &Calculator::divide)
		.def("sayHello", &Calculator::sayHello);
}

