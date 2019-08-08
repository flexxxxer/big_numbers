#include "src/big_numbers/whole_number.hpp"

#include <iostream>
#include <ctime>

using namespace std;
using namespace numbers;

void test1()
{
	const uint64_t time = clock();
	const whole_number a = 40'000u;
	whole_number c = a.factorial();
	cout << "test1 calculated, time: ";
	cout << clock() - time << endl;
}

int main()
{
	whole_number a = 4096u;

	cout << a.is_power_of_two() << endl;

	return 0;
}