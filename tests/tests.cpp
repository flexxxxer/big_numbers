#if defined TESTING_ENABLED && !defined PERFORMANCE_TESTING_ENABLED

#include <iostream>
#include <gtest/gtest.h>
#include <big_numbers/whole_number.hpp>

using namespace std;
using namespace numbers;

TEST(Arithmetic, Sum)
{
	const whole_number a = "94561245";
	const whole_number b = "4925794534123";

	const whole_number sum = a + b;
	
	ASSERT_EQ(sum.to_string(), "4925889095368");
}

TEST(Arithmetic, Sub)
{
	const whole_number a = "492579453412394583";
	const whole_number b = "94561245";

	const whole_number sum = a - b;

	ASSERT_EQ(sum.to_string(), "492579453317833338");
}

TEST(Arithmetic, Multiplication)
{
	const whole_number a = "492579453412394583";
	const whole_number b = "712394561245";

	const whole_number mul = a * b;

	ASSERT_EQ(mul.to_string(), "350910923592024757001099735835");
}

TEST(Arithmetic, Division)
{
	const whole_number a = "492579453412394583";
	const whole_number b = "712394561245";

	const whole_number sum = a / b;

	ASSERT_EQ(sum.to_string(), "691441");
}

#endif