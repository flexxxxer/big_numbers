#pragma once

// is big-int implementation from https://github.com/TwilightSaw/big-int

#include <string>
#include <algorithm>
#include <vector>

class unsigned_big_integer
{
public:
	unsigned_big_integer() = default;
	unsigned_big_integer(const std::string& number)
	{
		if (number.empty())
			throw std::exception("incorrect number");

		for (const char symbol : number)
		{
			if (unsigned_big_integer::dec_digits.find(symbol) == std::string::npos)
				throw std::exception("incorrect number");
		}

		this->_value = number;
	}
	unsigned_big_integer(const uint32_t number) : unsigned_big_integer(std::to_string(number)) {}
	unsigned_big_integer(const uint64_t number) : unsigned_big_integer(std::to_string(number)) {}

	[[nodiscard]] unsigned_big_integer sum(const unsigned_big_integer& n) const
	{
		std::string first_number_copy = this->_value;
		std::string second_number_copy = n._value;

		if (first_number_copy.size() < second_number_copy.size())
			first_number_copy.swap(second_number_copy);

		const size_t max_size = first_number_copy.size();
		const size_t min_size = second_number_copy.size();

		std::string sum = first_number_copy;
		uint8_t carry = 0;

		for (size_t i = 0; i < min_size; i++)
		{
			const char first_symbol = first_number_copy[max_size - i - 1];
			const char second_symbol = second_number_copy[min_size - i - 1];

			const uint8_t first_symbol_value = first_symbol - '0';
			const uint8_t second_symbol_value = second_symbol - '0';

			const uint8_t symbols_value_sum = static_cast<uint8_t>(first_symbol_value + second_symbol_value + carry);

			carry = symbols_value_sum > 9;

			const char result_symbol = static_cast<char>('0' + symbols_value_sum % 10);

			sum[max_size - i - 1] = result_symbol;
		}

		for (size_t i = min_size; i < max_size && carry != 0; i++)
		{
			const char first_symbol = first_number_copy[max_size - 1 - i];

			const uint8_t first_symbol_value = first_symbol - '0';

			const uint8_t symbol_value_sum = static_cast<uint8_t>(first_symbol_value + carry);

			carry = symbol_value_sum > 9;

			const char result_symbol = static_cast<char>('0' + symbol_value_sum % 10);

			sum[max_size - 1 - i] = result_symbol;
		}

		if (carry != 0)
			sum.insert(0, 1, carry + '0');

		return sum;
	}
	[[nodiscard]] unsigned_big_integer sub(const unsigned_big_integer& n) const
	{
		struct sub_util
		{
			static size_t steal_grade(std::string& string_number, size_t from_index)
			{
				for (size_t i = string_number.size() - from_index - 2; i != std::numeric_limits<size_t>::max(); i--)
				{
					const uint8_t symbol_value = string_number[i] - '0';

					if(symbol_value != 0)
					{
						--string_number[i];
						
						for(size_t j = i + 1; j < string_number.size() - from_index - 1; j++)
							string_number[j] = '9';

						return 1;
					}
				}

				return 0;
			}
		};
		
		std::string first_number_copy = this->_value;
		std::string second_number_copy = n._value;

		if (first_number_copy.size() < second_number_copy.size()
			|| (first_number_copy.size() == second_number_copy.size() &&
				first_number_copy.front() < second_number_copy.front()))
			throw std::exception();

		if (first_number_copy.size() < second_number_copy.size())
			first_number_copy.swap(second_number_copy);

		const size_t max_size = first_number_copy.size();
		const size_t min_size = second_number_copy.size();
		uint8_t carry = 0;

		for (size_t i = 0; i < min_size; i++)
		{
			const char first_symbol = first_number_copy[max_size - min_size + i];
			const char second_symbol = second_number_copy[i];

			const uint8_t first_symbol_value = first_symbol - '0';
			const uint8_t second_symbol_value = second_symbol - '0';

			const bool first_is_low_then_second = first_symbol_value < second_symbol_value;

			if (first_is_low_then_second)
			{
				if(first_number_copy[max_size - min_size + i - 1] == '0')
				{
					if (sub_util::steal_grade(first_number_copy, i) == 0)
						throw std::invalid_argument("argument greater then this number");
				}
				else
					--first_number_copy[max_size - min_size + i - 1];
			}

			first_number_copy[max_size - min_size + i] = '0' + first_symbol_value - second_symbol_value + 10 * first_is_low_then_second;
		}
		
		align::left(first_number_copy);

		return first_number_copy;
	}
	[[nodiscard]] unsigned_big_integer mul(const unsigned_big_integer& n) const
	{
		if (this->is_zero() || n.is_zero())
			return unsigned_big_integer::zero;
		
		size_t len1 = this->_value.size();
		size_t len2 = n._value.size();

		// will keep the result number in vector 
		// in reverse order 
		std::vector<size_t> result(len1 + len2, 0);

		// Below two indexes are used to find positions 
		// in result.  
		size_t i_n1 = 0;
		size_t i_n2 = 0;

		// Go from right to left in num1 
		for (int64_t i = len1 - 1; i >= 0; i--)
		{
			int64_t carry = 0;
			int64_t n1 = this->_value[i] - '0';

			// To shift position to left after every 
			// multiplication of a digit in num2 
			i_n2 = 0;

			// Go from right to left in num2              
			for (int64_t j = len2 - 1; j >= 0; j--)
			{
				// Take current digit of second number 
				int64_t n2 = n._value[j] - '0';

				// Multiply with current digit of first number 
				// and add_classic result to previously stored result 
				// at current position.  
				int64_t sum = n1 * n2 + result[i_n1 + i_n2] + carry;

				// Carry for next iteration 
				carry = sum / 10;

				// Store result 
				result[i_n1 + i_n2] = sum % 10;

				i_n2++;
			}

			// store carry in next cell 
			if (carry > 0)
				result[i_n1 + i_n2] += carry;

			// To shift position to left after every 
			// multiplication of a digit in num1. 
			i_n1++;
		}

		// ignore '0's from the right 
		int64_t i = result.size() - 1;
		while (i >= 0 && result[i] == 0)
			i--;

		// If all were '0's - means either both or 
		// one of num1 or num2 were '0' 
		if (i == -1)
			return unsigned_big_integer::zero;

		// generate the result string 
		std::stringstream s;

		while (i >= 0)
			s << std::to_string(result[i--]);

		return s.str();
	}
	[[nodiscard]] unsigned_big_integer div(size_t n) const
	{
		if (n == 0)
			throw std::exception();
		if (n == 1)
			return *this;

		std::string ans, number = this->_value;
		uint32_t idx = 0;
		uint32_t temp = number[idx] - '0';

		while (temp < n)
			temp = temp * 10 + (number[++idx] - '0');

		while (number.size() > idx)
		{
			ans.push_back(temp / n + '0');

			temp = (temp % n) * 10 + number[++idx] - '0';
		}

		if (ans.length() == 0)
			return static_cast<std::string>("0");

		return ans;
	}
	[[nodiscard]] unsigned_big_integer div_rem(size_t n, unsigned_big_integer& reminder) const
	{
		std::vector<int> vec(this->_value.size());
		size_t mod = 0;

		for (int i = 0; i < this->_value.size(); i++) 
		{
			const int digit = this->_value[i] - '0';
			mod = mod * 10 + digit;

			const int quo = mod / n;
			vec[i] = quo;

			mod = mod % n;
		}

		reminder = mod;

		size_t index = 0;
		for(index = 0; index < vec.size(); index++)
			if(vec[index] != 0)
				break;

		std::string q; q.resize(vec.size() - index);
		for (size_t i = 0; i < q.size(); i++)
			q[i] = vec[i + index] + '0';

		return q.empty() ? "0" : q;
	}
	[[nodiscard]] unsigned_big_integer div16_rem(uint16_t& reminder) const
	{
		constexpr uint16_t n = 16;
		std::vector<int> vec(this->_value.size());
		uint16_t mod = 0;

		for (int i = 0; i < this->_value.size(); i++)
		{
			const int digit = this->_value[i] - '0';
			mod = mod * 10 + digit;

			const int quo = mod / n;
			vec[i] = quo;

			mod = mod % n;
		}

		reminder = mod;

		size_t index = 0;
		for (index = 0; index < vec.size(); index++)
			if (vec[index] != 0)
				break;

		std::string q; q.resize(vec.size() - index);
		for (size_t i = 0; i < q.size(); i++)
			q[i] = vec[i + index] + '0';

		return q.empty() ? "0" : q;
	}

	[[nodiscard]] unsigned_big_integer pow(size_t power) const
	{
		if (power == 0)
			return unsigned_big_integer::one;
		if (power == 1)
			return *this;
		if (this->is_zero() || this->is_one())
			return *this;

		unsigned_big_integer exponent = power;
		unsigned_big_integer result = unsigned_big_integer::one;
		unsigned_big_integer x = *this;

		while (exponent.is_not_zero())
		{
			if(exponent.is_even())
			{
				x = x.mul(x);
				exponent = exponent.div(2);
			}
			else
			{
				result = result.mul(x);
				x = x.mul(x);
				exponent = exponent.sub(unsigned_big_integer::one).div(2);
			}
		}

		return result;
	}

	[[nodiscard]] std::string to_string() const
	{
		return  this->_value;
	}

	[[nodiscard]] bool is_zero() const
	{
		return this->_value == "0";
	}
	[[nodiscard]] bool is_one() const
	{
		return this->_value == "1";
	}
	[[nodiscard]] bool is_not_zero() const
	{
		return !this->is_zero();
	}
	[[nodiscard]] bool is_odd() const
	{
		return this->is_even() == false;
	}
	[[nodiscard]] bool is_even() const
	{
		return this->is_zero() || this->_value.back() % 2 == 0;
	}
	
private:
	std::string _value = "0";
	inline static const std::string dec_digits = "1234567890";
	inline static const std::string zero = { '0' };
	inline static const std::string one = { '1' };

	struct align
	{
		static void right(std::string& number)
		{
			for (size_t i = number.size() - 1; i != 0; i--)
			{
				if (number[i] != '0')
				{
					number.resize(i + 1);
					return;
				}
			}

			number.resize(1);
		}

		static void left(std::string& number)
		{
			for (size_t i = 0; i < number.size(); i++)
			{
				if (number[i] != '0')
				{
					number.erase(0, i);
					return;
				}
			}

			number.resize(1);
		}
	};
};
