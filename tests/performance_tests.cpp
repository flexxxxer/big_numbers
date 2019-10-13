#if !defined TESTING_ENABLED && defined PERFORMANCE_TESTING_ENABLED

#include <iostream>
#include <random>

#include  "performance_test.h"
#include <include/fast_big_integer.h>

using namespace std;
using namespace numbers;

class whole_number_tests
{
public:
	static void sqrt_test(const int repeat = 10)
	{
		const function init_number = [](fast_big_integer& number)
		{
			number = static_cast<fast_big_integer>("100000000000000000000000000000000000000000000000000000000000000000000000");
		};

		const function sqrt_benchmark = [](const fast_big_integer& number)
		{
			size_t bit_count = number.sqrt().num_bits();
			bit_count++;
		};

		const performance_test<void, fast_big_integer> sqrt
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
		const function init_number_and_base = [](std::pair<fast_big_integer, uint64_t>& values)
		{
			values.first = static_cast<fast_big_integer>("100000000000000000000000000000000000000000000000000000000000000000000000");
			values.second = 25u;
		};

		const function log_n_benchmark = [](const std::pair<fast_big_integer, uint64_t>& values)
		{
			size_t bit_count = values.first.log_n(values.second).num_bits();
			bit_count++;
		};

		const performance_test<void, std::pair<fast_big_integer, uint64_t>> log_n
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
		const function init_base_and_power = [](std::pair<fast_big_integer, fast_big_integer>& values)
		{
			values.first = 10'000;
			values.second = 30'000;
		};

		const function pow_benchmark = [](const std::pair<fast_big_integer, fast_big_integer>& values)
		{
			size_t bit_count = values.first.pow(values.second).num_bits();
			bit_count++;
		};

		const performance_test<void, std::pair<fast_big_integer, fast_big_integer>> factorial
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
		const function init_number = [](fast_big_integer& number)
		{
			number = static_cast<fast_big_integer>("40000");
		};
		
		const function factorial_benchmark = [](const fast_big_integer& number)
		{
			size_t bit_count = number.factorial().num_bits();
			bit_count++;
		};
		
		const performance_test<void, fast_big_integer> factorial
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
		const function init_number = [](fast_big_integer& number)
		{
			number = static_cast<fast_big_integer>("40000");
		};

		const function factorial_benchmark = [](const fast_big_integer& number)
		{
			size_t bit_count = number.factorial_parallel().num_bits();
			bit_count++;
		};

		const performance_test<void, fast_big_integer> factorial
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
		const function init_first_and_second = [](std::pair<fast_big_integer, fast_big_integer>& values)
		{
			values.first = static_cast<fast_big_integer>("94268994258938412899835384830325634925939491824981248");
			values.second = static_cast<fast_big_integer>("39659214656723773994969923999239997777");
		};

		const function gcd_benchmark = [](const std::pair<fast_big_integer, fast_big_integer>& values)
		{
			size_t bit_count = fast_big_integer::extensions::gcd(values.first, values.second).num_bits();
			bit_count++;
		};

		const performance_test<void, std::pair<fast_big_integer, fast_big_integer>> gcd
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
		const function init_first_and_second = [](std::pair<fast_big_integer, fast_big_integer>& values)
		{
			values.first = static_cast<fast_big_integer>("94268994258938412899835384830325634925939491824981248");
			values.second = static_cast<fast_big_integer>("39659214656723773994969923999239997777");
		};

		const function lcm_benchmark = [](const std::pair<fast_big_integer, fast_big_integer>& values)
		{
			size_t bit_count = fast_big_integer::extensions::lcm(values.first, values.second).num_bits();
			bit_count++;
		};

		const performance_test<void, std::pair<fast_big_integer, fast_big_integer>> lcm
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
		whole_number_tests::sqrt_test();
		whole_number_tests::log_n_test();
		whole_number_tests::pow_test();
	    whole_number_tests::factorial_test();
		whole_number_tests::factorial_parallel_test();
		whole_number_tests::gcd_test();
		whole_number_tests::lcm_test();

		cout << "tests was ended" << endl;
	}
};

int main()
{
	whole_number_tests::perform_all_tests();

	cin.get();	
	
	return 0;
}

#endif