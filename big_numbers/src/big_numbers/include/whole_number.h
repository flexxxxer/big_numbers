#ifndef WHOLE_NUMBER_H
#define WHOLE_NUMBER_H

#include <vector> // use std::vector
#include <string> // use std::string, std::wstring

#include "../cpp/base_converter.cpp"

using namespace converters;

namespace numbers
{
	// is one byte, eight bits, takes values ​​from 0 to 255
	// это один байт, восемь бит, принимает значения от 0 до 255
	// to jeden bajt, osiem bitów, przyjmuje wartości od 0 do 255
	typedef unsigned char byte;

	// is one byte, eight bits, takes values ​​from -128 to 127
	// это один байт, восемь бит, принимает значения от -128 до 127
	// to jeden bajt, osiem bitów, przyjmuje wartości od -128 do 127
	typedef char sbyte;

	// is a large positive integer
	// представляет собой большое положительное целое число
	// reprezentuje dużą dodatnią liczbę całkowitą
	class whole_number
	{
		// this is the set of bytes that make up the number
		// это набор байт, из которого состоит число
		// jest to zbiór bajtów, z których składa się liczba
		std::vector<byte> bytes;

		static const converters::base_converter hex2dec;
		static const converters::base_converter dec2hex;

		//static std::string to_hex_str(const std::vector<byte>& bytes);
		static std::vector<byte> uint_to_bytes(const uint32_t number);
		static std::vector<byte> ulong_to_bytes(const uint64_t number);

		// 0 if eq
		// 1 if a > b
		// -1 if a < b
		static sbyte compare(const whole_number& a, const whole_number& b);
		static sbyte compare_optimized(const whole_number& a, const whole_number& b);

		static void add_logic_gate(whole_number& destination, const whole_number& source);
		static void add_classic(whole_number& destination, const whole_number& source);

		static void sub_logic_gate(whole_number& destination, const whole_number& source);
		static void sub_classic(whole_number& destination, const whole_number& source);

		static void div(const whole_number& dividend, const whole_number& divisor, whole_number& quotient, whole_number& remainder);

		static void clear_zero_bytes(std::vector<byte>& bytes);
		static void clear_zero_bytes(whole_number& number);

	public:

		whole_number();
		whole_number(std::string str);
		whole_number(std::vector<byte> bytes);
		whole_number(std::initializer_list<byte> bytes);
		whole_number(uint64_t number);
		whole_number(uint32_t number);
		~whole_number();

		static whole_number zero();
		static whole_number one();
		static whole_number two();

		std::string to_string() const;
		std::string to_string_hex() const;
		uint64_t to_uint64_t() const;
		std::vector<byte> to_bytes() const;

		whole_number and (const whole_number& number) const;
		whole_number or (const whole_number & number) const;
		whole_number xor (const whole_number& number) const;
		whole_number not () const;

		void shr(uint64_t shift_count);
		void shl(uint64_t shift_count);

		void add(const whole_number& number);
		whole_number sum(const whole_number& number) const;

		void sub(const whole_number& number);
		whole_number difference(const whole_number& number) const;

		void mul(const whole_number& number);
		void fast_mul(const whole_number& number);
		whole_number product(const whole_number& multiplier) const;

		whole_number division(const whole_number& divisor) const;
		
		bool is_zero() const;
		bool is_not_zero() const;

		bool is_one() const;
		bool is_two() const;
		bool is_power_of_two() const;

		uint64_t num_bits() const;
		bool is_odd() const;
		bool is_even() const;

		void set_zero();

		// prefix
		whole_number& operator ++ (); // inc
		whole_number& operator-- (); // dec

		// postfix
		whole_number& operator ++ (int); // inc
		whole_number& operator-- (int); // dec

		whole_number operator + (const whole_number& number) const;
		whole_number operator - (const whole_number& number) const;
		whole_number operator * (const whole_number& number) const;
		whole_number operator / (const whole_number& number) const;

		whole_number operator << (uint64_t shift_count) const;
		void operator <<= (uint64_t shift_count);
		whole_number operator >> (uint64_t shift_count) const;
		void operator >>= (uint64_t shift_count);

		whole_number operator & (const whole_number& number) const;
		whole_number operator | (const whole_number& number) const;
		whole_number operator ^ (const whole_number& number) const;

		whole_number pow(const whole_number& exponent) const;
		whole_number factorial() const;
		whole_number sqrt() const;
		whole_number log_n(uint64_t n) const;
		whole_number log2() const;
	};
}

#endif