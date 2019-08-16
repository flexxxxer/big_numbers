#if !defined TESTING_ENABLED && defined PERFORMANCE_TESTING_ENABLED

#include <iostream>
#include <ctime>
#include <random>

#include  "performance_test.h"

#include <big_numbers/whole_number.hpp>

using namespace std;
using namespace numbers;

class whole_number_tests
{
public:
	static void sqrt_test(const int repeat = 10)
	{
		for (int i = 1; i <= repeat; i++)
		{
			const whole_number n = "100000000000000000000000000000000000000000000000000000000000000000000000";
			const uint64_t timer = clock();
			whole_number result = n.sqrt();
			cout << "sqrt_test, iteration: " << i << " , time: " << clock() - timer << "ms, result = " << result.to_string() << endl;
		}
		cout << endl;
	}

	static void log_n_test(const int repeat = 10, const uint64_t n = 10)
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

	static void pow_test(const int repeat = 10)
	{
		for (int i = 1; i <= repeat; i++)
		{
			const whole_number base = 10'000u;
			const whole_number power = 30'000u;
			const uint64_t timer = clock();
			const whole_number result = base.pow(power);
			cout << "pow_test, iteration: " << i << " , time: " << clock() - timer
				<< "ms, result = " << "*many symbols*, " << result.num_bits() << "bits" << endl;
		}
		cout << endl;
	}
	
	static void factorial_test(const int repeat = 10)
	{
		const performance_test<void, whole_number> factorial([](const whole_number& number)
			{
				const size_t bits_count = number.factorial().num_bits();
			}, 40'000ull, "factorial_test", repeat, 5
		);

		const benchmark_info info = factorial.perform();

		iostream console(cout.rdbuf());
		factorial.print_performance_test_info_to_stream(console, info);
		
		cout << endl;
	}

	static void gcd_test(const int repeat = 10)
	{
		for (int i = 1; i <= repeat; i++)
		{
			const whole_number a = 1956929592349429ull;
			const whole_number b = 14418577843ull;
			const uint64_t timer = clock();
			const whole_number result = whole_number::extensions::gcd(a, b);
			
			cout << "gcd_test, iteration: " << i << " , time: " << clock() - timer
				<< "ms, result = " << "*many symbols*, " << result.num_bits() << "bits" << endl;
		}
		cout << endl;
	}

	static void lcm_test(const int repeat = 10)
	{
		for (int i = 1; i <= repeat; i++)
		{
			const whole_number a = 1956929592349429ull;
			const whole_number b = 14418577843ull;
			const uint64_t timer = clock();
			const whole_number result = whole_number::extensions::lcm(a, b);

			cout << "lcm_test, iteration: " << i << " , time: " << clock() - timer
				<< "ms, result = " << "*many symbols*, " << result.num_bits() << "bits" << endl;
		}
		cout << endl;
	}

	static void perform_all_tests()
	{
		// whole_number_tests::sqrt_test();
		// whole_number_tests::log_n_test();
		// whole_number_tests::pow_test();
		whole_number_tests::factorial_test();
		// whole_number_tests::gcd_test();
		// whole_number_tests::lcm_test();
	}
};

int main()
{
	whole_number_tests::perform_all_tests();

	return 0;
}

#endif