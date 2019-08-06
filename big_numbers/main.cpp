#include "src/big_numbers/whole_number.hpp"

#include <iostream>
#include <ctime>

using namespace std;
using namespace numbers;

int main()
{
	whole_number a = 500u;
	whole_number c = a.factorial_fast();

	cout << c.to_string() << endl;

	return 0;
}
