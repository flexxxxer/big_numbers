#include "src/big_numbers/whole_number.hpp"

#include <iostream>
#include <ctime>

using namespace std;
using namespace numbers;

void sqrt_test(int repeat = 10)
{
	for(int i = 1; i <= 10; i++)
	{
		const whole_number n = "100000000000000000000000000000000000000000000000000000000000000000000000";
		const uint64_t timer = clock();
		whole_number result = n.sqrt();
		cout << "sqrt_test, iteration: " << i << " , time: " << clock() - timer << "ms, result = " << result.to_string() << endl << endl;
	}
	cout << endl;
}

void log_n_test(int repeat = 10, uint64_t nnn = 10)
{
	for (int i = 1; i <= 10; i++)
	{
		const whole_number n = "100000000000000000000000000000000000000000000000000000000000000000000000";
		const uint64_t timer = clock();
		whole_number result = n.log_n(nnn);
		cout << "log_n_test, iteration: " << i << " , time: " << clock() - timer << "ms, result = " << result.to_string() << endl << endl;
	}
	cout << endl;
}

void factorial_test(int repeat = 10)
{
	for (int i = 1; i <= 10; i++)
	{
		const whole_number n = 40'000u;
		const uint64_t timer = clock();
		whole_number result = n.factorial();
		cout << "factorial_test, iteration: " << i << " , time: " << clock() - timer 
			<< "ms, result = " << "*many symbols*, " << result.num_bits() << "bits" << endl << endl;
	}
	cout << endl;
}

void perform_tests()
{
	sqrt_test();
	log_n_test();
	factorial_test();
}

int main()
{
	perform_tests();
		
	return 0;
}