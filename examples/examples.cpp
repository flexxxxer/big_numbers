#if !defined TESTING_ENABLED && !defined PERFORMANCE_TESTING_ENABLED

#include <iostream>
#include <big_numbers/whole_number.hpp>

using namespace std;
using namespace numbers;

uint64_t mul(uint64_t n, uint64_t m)
{
	uint64_t ans = 0u, count = 0u;

	while(m != 0)
	{
		if (m % 2 == 1)
			ans += n << count;

		count++;
		m /= 2;
	}

	return ans;
}

int main()
{
	whole_number a = 27'977u;

	cout << (a.factorial().to_string_hex() == a.factorial_parallel().to_string_hex()) << endl;
	return 0;
}

#endif