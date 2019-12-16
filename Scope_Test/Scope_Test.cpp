#include <iostream>

int main()
{
	bool x = false;
	int new_var = 0;

	if (x)
	{
		int new_var = 5;
	}
	else
	{
		int new_var = 10;
	}

	std::cout << new_var;
}

