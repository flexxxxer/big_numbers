#if !defined TESTING_ENABLED && defined PERFORMANCE_TESTING_ENABLED

#include <iostream>
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
		const function init_number = [](whole_number& number)
		{
			number = static_cast<whole_number>("100000000000000000000000000000000000000000000000000000000000000000000000");
		};

		const function sqrt_benchmark = [](const whole_number& number)
		{
			size_t bit_count = number.sqrt().num_bits();
			bit_count++;
		};

		const performance_test<void, whole_number> sqrt
		(
			sqrt_benchmark,
			init_number,
			"sqrt_test",
			repeat,
			5
		);

		const benchmark_info info = sqrt.perform();

		iostream console(cout.rdbuf());
		sqrt.print_performance_test_info_to_stream(console, info);

		cout << endl;
	}

	static void log_n_test(const int repeat = 10)
	{
		const function init_number_and_base = [](std::pair<whole_number, uint64_t>& values)
		{
			values.first = static_cast<whole_number>("100000000000000000000000000000000000000000000000000000000000000000000000");
			values.second = 10u;
		};

		const function log_n_benchmark = [](const std::pair<whole_number, uint64_t>& values)
		{
			size_t bit_count = values.first.log_n(values.second).num_bits();
			bit_count++;
		};

		const performance_test<void, std::pair<whole_number, uint64_t>> log_n
		(
			log_n_benchmark,
			init_number_and_base,
			"log_n_test",
			repeat,
			5
		);

		const benchmark_info info = log_n.perform();

		iostream console(cout.rdbuf());
		log_n.print_performance_test_info_to_stream(console, info);

		cout << endl;
	}

	static void pow_test(const int repeat = 10)
	{
		const function init_base_and_power = [](std::pair<whole_number, whole_number>& values)
		{
			values.first = 10'000u;
			values.second = 30'000u;
		};

		const function pow_benchmark = [](const std::pair<whole_number, whole_number>& values)
		{
			size_t bit_count = values.first.pow(values.second).num_bits();
			bit_count++;
		};

		const performance_test<void, std::pair<whole_number, whole_number>> factorial
		(
			pow_benchmark,
			init_base_and_power,
			"pow_test",
			repeat,
			5
		);

		const benchmark_info info = factorial.perform();

		iostream console(cout.rdbuf());
		factorial.print_performance_test_info_to_stream(console, info);

		cout << endl;
	}

	static void factorial_test(const int repeat = 10)
	{
		const function init_number = [](whole_number& number)
		{
			number = static_cast<whole_number>("40000");
		};
		
		const function factorial_benchmark = [](const whole_number& number)
		{
			size_t bit_count = number.factorial().num_bits();
			bit_count++;
		};
		
		const performance_test<void, whole_number> factorial
		(
			factorial_benchmark,
			init_number,
			"factorial_test", 
			repeat, 
			5
		);

		const benchmark_info info = factorial.perform();

		iostream console(cout.rdbuf());
		factorial.print_performance_test_info_to_stream(console, info);
		
		cout << endl;
	}

	static void factorial_parallel_test(const int repeat = 10)
	{
		const function init_number = [](whole_number& number)
		{
			number = static_cast<whole_number>("40000");
		};

		const function factorial_benchmark = [](const whole_number& number)
		{
			size_t bit_count = number.factorial_parallel().num_bits();
			bit_count++;
		};

		const performance_test<void, whole_number> factorial
		(
			factorial_benchmark,
			init_number,
			"factorial_parallel_test",
			repeat,
			5
		);

		const benchmark_info info = factorial.perform();

		iostream console(cout.rdbuf());
		factorial.print_performance_test_info_to_stream(console, info);
		cout << "add info: " << std::thread::hardware_concurrency() << " threads on machine" << endl;

		cout << endl;
	}
	
	static void gcd_test(const int repeat = 10)
	{
		const function init_first_and_second = [](std::pair<whole_number, whole_number>& values)
		{
			values.first = static_cast<whole_number>("94268994258938412899835384830325634925939491824981248");
			values.second = static_cast<whole_number>("39659214656723773994969923999239997777");
		};

		const function gcd_benchmark = [](const std::pair<whole_number, whole_number>& values)
		{
			size_t bit_count = whole_number::extensions::gcd(values.first, values.second).num_bits();
			bit_count++;
		};

		const performance_test<void, std::pair<whole_number, whole_number>> gcd
		(
			gcd_benchmark,
			init_first_and_second,
			"gcd_test",
			repeat,
			5
		);

		const benchmark_info info = gcd.perform();

		iostream console(cout.rdbuf());
		gcd.print_performance_test_info_to_stream(console, info);

		cout << endl;
	}

	static void lcm_test(const int repeat = 10)
	{
		const function init_first_and_second = [](std::pair<whole_number, whole_number>& values)
		{
			values.first = static_cast<whole_number>("94268994258938412899835384830325634925939491824981248");
			values.second = static_cast<whole_number>("39659214656723773994969923999239997777");
		};

		const function lcm_benchmark = [](const std::pair<whole_number, whole_number>& values)
		{
			size_t bit_count = whole_number::extensions::lcm(values.first, values.second).num_bits();
			bit_count++;
		};

		const performance_test<void, std::pair<whole_number, whole_number>> lcm
		(
			lcm_benchmark,
			init_first_and_second,
			"lcm_test",
			repeat,
			5
		);

		const benchmark_info info = lcm.perform();

		iostream console(cout.rdbuf());
		lcm.print_performance_test_info_to_stream(console, info);

		cout << endl;
	}

	static void perform_all_tests()
	{
		// whole_number_tests::sqrt_test();
		// whole_number_tests::log_n_test();
		// whole_number_tests::pow_test();
	    whole_number_tests::factorial_test();
		whole_number_tests::factorial_parallel_test();
		// whole_number_tests::gcd_test();
		// whole_number_tests::lcm_test();
	}
};

int main()
{
	whole_number_tests::perform_all_tests();

	cin.get();	
	
	return 0;
}

#endif