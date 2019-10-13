#if defined TESTING_ENABLED && !defined PERFORMANCE_TESTING_ENABLED

#include <iostream>
#include <gtest/gtest.h>
#include <include/fast_big_integer.h>

using namespace std;
using namespace numbers;

TEST(Arithmetic, Sum)
{
	const fast_big_integer a = "94561245";
	const fast_big_integer b = "4925794534123";

	const fast_big_integer sum = a + b;
	
	ASSERT_EQ(sum.to_string(), "4925889095368");
}

TEST(Arithmetic, Sub)
{
	const fast_big_integer a = "492579453412394583";
	const fast_big_integer b = "94561245";

	const fast_big_integer sum = a - b;

	ASSERT_EQ(sum.to_string(), "492579453317833338");
}

TEST(Arithmetic, Multiplication)
{
	const fast_big_integer a = "492579453412394583";
	const fast_big_integer b = "712394561245";

	const fast_big_integer mul = a * b;

	ASSERT_EQ(mul.to_string(), "350910923592024757001099735835");
}

TEST(Arithmetic, Division)
{
	const fast_big_integer a = "492579453412394583";
	const fast_big_integer b = "712394561245";

	const fast_big_integer sum = a / b;

	ASSERT_EQ(sum.to_string(), "691441");
}

#endif