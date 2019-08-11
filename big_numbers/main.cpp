#include <iostream>
#include <ctime>
#include <random>

#include "src/big_numbers/whole_number.hpp"

using namespace std;
using namespace numbers;

void print_vector(const std::vector<sbyte>& v)
{
	for (char i : v)
		cout << static_cast<int>(i) << " ";

	cout << endl;
}

void long_to_bytes()
{
	int64_t var = numeric_limits<int64_t>::min();

	numbers::sbyte* bts = reinterpret_cast<numbers::sbyte*>(&var);

	vector<numbers::sbyte> v;
	v.assign(bts, bts + 8);

	print_vector(v);
}

void sqrt_test(const int repeat = 10)
{
	for(int i = 1; i <= repeat; i++)
	{
		const whole_number n = "100000000000000000000000000000000000000000000000000000000000000000000000";
		const uint64_t timer = clock();
		whole_number result = n.sqrt();
		cout << "sqrt_test, iteration: " << i << " , time: " << clock() - timer << "ms, result = " << result.to_string() << endl;
	}
	cout << endl;
}

void log_n_test(const int repeat = 10, const uint64_t n = 10)
{
	for (int i = 1; i <= repeat; i++)
	{
		const whole_number number = "100000000000000000000000000000000000000000000000000000000000000000000000";
		const uint64_t timer = clock();
		whole_number result = number.log_n(n);
		cout << "log_n_test, iteration: " << i << " , time: " << clock() - timer << "ms, result = " << result.to_string() << endl;
	}
	cout << endl;
}

void factorial_test(const int repeat = 10)
{
	for (int i = 1; i <= repeat; i++)
	{
		const whole_number n = 40'000u;
		const uint64_t timer = clock();
		const whole_number result = n.factorial();
		cout << "factorial_test, iteration: " << i << " , time: " << clock() - timer 
			<< "ms, result = " << "*many symbols*, " << result.num_bits() << "bits" << endl;
	}
	cout << endl;
}

void gcd_lcm_test()
{

	const whole_number a = 256u;
	const whole_number b = 144u;

	cout << whole_number::extensions::gcd(a, b).to_string() << endl;
	cout << whole_number::extensions::lcm(a, b).to_string() << endl;
}

void perform_tests()
{
	sqrt_test();
	log_n_test();
	factorial_test();
}

int main()
{
	// perform_tests();

	whole_number a = 4096ull;
	a = a.pow(a);
	cout << a.is_power_of_two() << endl;

	return 0;
}