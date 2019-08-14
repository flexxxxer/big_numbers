﻿#ifndef WHOLE_NUMBER_H
#define WHOLE_NUMBER_H

#include <vector> // use std::vector
#include <string> // use std::string, std::wstring

#include "../cpp/base_converter.cpp"

using namespace converters;

namespace numbers
{
	/**
	 * \brief это один байт, восемь бит, принимает значения от 0 до 255
	 * \brief to jeden bajt, osiem bitów, przyjmuje wartości od 0 do 255
	 * \brief is one byte, eight bits, takes values ​​from 0 to 255
	 */
	typedef unsigned char byte;

	
	/**
	 * \brief это один байт, восемь бит, принимает значения от -128 до 127
	 * \brief to jeden bajt, osiem bitów, przyjmuje wartości od -128 do 127
	 * \brief is one byte, eight bits, takes values ​​from -128 to 127
	 */
	typedef char sbyte;

	
	/**
	 * \brief представляет собой большое положительное целое число
	 * \brief reprezentuje dużą dodatnią liczbę całkowitą
	 * \brief is a large positive integer
	 */
	class whole_number   // NOLINT(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)
	{
		
		/**
		 * \brief это набор байт, из которого состоит число
		 * \brief jest to zbiór bajtów, z których składa się liczba
		 * \brief this is the set of bytes that make up the number
		 */
		std::vector<byte> bytes_;

		static const converters::base_converter hex2dec;
		static const converters::base_converter dec2hex;

		/**
		 * \brief convert uint32_t to bytes
		 * \param number convertible value 
		 * \return vector of bytes
		 */
		static std::vector<byte> uint_to_bytes(const uint32_t number);
		/**
		 * \brief convert uint64_t to bytes
		 * \param number convertible value
		 * \return vector of bytes
		 */
		static std::vector<byte> ulong_to_bytes(const uint64_t number);

		/**
		 * \brief compare two numbers
		 * \param a first number
		 * \param b second number
		 * \return 0: if eq; 1: if a > b; -1: if a < b
		 */
		static sbyte compare(const whole_number& a, const whole_number& b);
		static sbyte compare_optimized(const whole_number& a, const whole_number& b);

		/**
		 * \brief sum two numbers by logic gates
		 * \param destination first number for sum (store the result)
		 * \param source second number for sum
		 */
		static void add_logic_gate(whole_number& destination, const whole_number& source);
		/**
		 * \brief sum two numbers
		 * \param destination first number for sum (store the result)
		 * \param source second number for sum
		 */
		static void add_classic(whole_number& destination, const whole_number& source);

		/**
		 * \brief sub source from destination by logic gates
		 * \param destination first number (store the result)
		 * \param source second number
		 */
		static void sub_logic_gate(whole_number& destination, const whole_number& source);
		/**
		 * \brief sub source from destination
		 * \param destination first number (store the result)
		 * \param source second number
		 */
		static void sub_classic(whole_number& destination, const whole_number& source);

		/**
		 * \brief perform division by Newton-Raphson algorithm (https://en.wikipedia.org/wiki/Newton%27s_method)
		 * \param dividend number to be divided
		 * \param divisor is divider (NOT EQUAL TO ZERO)
		 * \param quotient store the result of division
		 * \param remainder store the remainder
		 */
		static void div(const whole_number& dividend, const whole_number& divisor, whole_number& quotient, whole_number& remainder);

		/**
		 * \brief remove all back bytes, which equal to zero
		 * \param bytes to clear
		 */
		static void clear_zero_bytes(std::vector<byte>& bytes);
		/**
		 * \brief remove all back bytes, which equal to zero
		 * \param number to clear
		 */
		static void clear_zero_bytes(whole_number& number);

	public:

		/**
		 * \brief contains all extensions methods to working
		 */
		class extensions
		{
			extensions();
		public:

			/**
			 * \param a first number
			 * \param b second number
			 * \return largest of the numbers
			 */
			static whole_number max(const whole_number& a, const whole_number& b);
			/**
			 * \param a first number
			 * \param b second number
			 * \return smallest of numbers
			 */
			static whole_number min(const whole_number& a, const whole_number& b);

			/**
			 * \param a fist number
			 * \param b second number
			 * \return greatest common divisor number
			 */
			static whole_number gcd(const whole_number& a, const whole_number& b);
			/**
			 * \param a fist number
			 * \param b second number
			 * \return least common multiple number
			 */
			static whole_number lcm(const whole_number& a, const whole_number& b);
			
			/**
			 * \param max_byte_count max random byte count
			 * \return random whole number
			 */
			static whole_number random(uint32_t max_byte_count = 1000u);
		};

		/**
		 * \brief init number to default value (is zero)
		 */
		whole_number();
		/**
		 * \brief init number from string
		 * \param str number in hex or dec format
		 */
		whole_number(std::string str);
		/**
		 * \brief init number from byte vector
		 * \param bytes future number bytes
		 */
		whole_number(std::vector<byte> bytes);
		/**
		 * \brief init number from byte array
		 * \param bytes future number bytes
		 */
		whole_number(std::initializer_list<byte> bytes);
		/**
		 * \brief init number from uint32_t value
		 * \param number any number
		 */
		whole_number(uint64_t number);
		/**
		 * \brief init number from uint32_t value
		 * \param number any number
		 */
		whole_number(uint32_t number);

		~whole_number();

		/**
		 * \return number with value "0"
		 */
		static whole_number zero();
		/**
		 * \return number with value "1"
		 */
		static whole_number one();
		/**
		 * \return number with value "2"
		 */
		static whole_number two();

		/**
		 * \return dec string view of number
		 */
		std::string to_string() const;
		/**
		 * \return hex string view of number
		 */
		std::string to_string_hex() const;
		/**
		 * \brief try cast whole number to uint64_t value
		 * \return uint64_t value
		 */
		uint64_t to_uint64_t() const;
		/**
		 * \return byte vector
		 */
		std::vector<byte> to_bytes() const;

		/**
		 * \brief perform AND operation
		 * \param number operand
		 * \return AND operation result between this and number
		 */
		whole_number and (const whole_number& number) const;
		/**
		 * \brief perform OR operation
		 * \param number operand
		 * \return OR operation result between this and number
		 */
		whole_number or (const whole_number & number) const;
		/**
		 * \brief perform XOR operation
		 * \param number operand
		 * \return XOR operation result between this and number
		 */
		whole_number xor (const whole_number& number) const;
		/**
		 * \return NOT operation result (bit inversion)
		 */
		whole_number not () const;

		/**
		 * \brief perform shift bits to right
		 * \param shift_count bits shift count
		 */
		void shr(size_t shift_count);
		/**
		 * \brief perform shift bits to left
		 * \param shift_count bits shift count
		 */
		void shl(size_t shift_count);

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

		size_t num_bits() const;
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
		whole_number operator % (const whole_number& number) const;

		whole_number operator << (size_t shift_count) const;
		void operator <<= (size_t shift_count);
		whole_number operator >> (size_t shift_count) const;
		void operator >>= (size_t shift_count);

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