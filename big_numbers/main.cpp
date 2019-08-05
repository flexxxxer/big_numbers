#include "src/big_numbers/whole_number.hpp"

#include <iostream>

using namespace std;
using namespace numbers;

int main()
{
	whole_number a = 2u;
	whole_number b = 4u;

	cout << (a.product(b)).to_string() << endl;
}