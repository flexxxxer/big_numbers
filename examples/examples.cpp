#if !defined TESTING_ENABLED && !defined PERFORMANCE_TESTING_ENABLED

#include <iostream>
#include "../src/include/fast_big_integer.h"

using namespace std;
using namespace numbers;

int main()
{
	// rand number with max 40 bytes count
	fast_big_integer rand = fast_big_integer::extensions::random(40).plus_one();

	// sum
	fast_big_integer a = rand + rand;

	// sub
	fast_big_integer b = a - rand;

	// mul
	fast_big_integer c = a * b;

	// div
	fast_big_integer d = c / rand;

	// power
	fast_big_integer e = c.pow(100);

	// factorial parallel
	fast_big_integer f = static_cast<fast_big_integer>(100).factorial_parallel();

	// logic gates        xor and or
	fast_big_integer g = a ^ b & c | d;

	// sqrt, logarithm
	fast_big_integer h = a.sqrt().log2().log_n(10);
	
	return 0;
}

#endif