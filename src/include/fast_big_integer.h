#pragma once

#include <vector> // use std::vector
#include <string> // use std::string
#include <stdexcept> // use exceptions
#include <compare>

#include "modules/converter/bbc.h"

#include <future>
#include "modules/hpc/thread_pool.h"
#include <random>

namespace numbers
{
	/**
	 * \brief is one byte, eight bits, takes values ​​from 0 to 255
	 */
	typedef unsigned char byte;

	/**
	 * \brief is one byte, eight bits, takes values ​​from -128 to 127
	 */
	typedef char sbyte;

	/**
	 * \brief is a large positive integer
	 */
	class fast_big_integer
	{
		/**
		 * \brief this is the set of bytes that make up the number
		 */
		std::vector<byte> bytes_;

		/**
		 * \brief convert int32_t to bytes
		 * \param number convertible value
		 * \return vector of bytes
		 */
		static std::vector<byte> int_to_bytes(const int32_t number)
		{
			if (number < 0) // when number less then zero
				throw std::invalid_argument("number");
			if (number == 0) // when number eq to zero
				return {};

			// init const and bytes
			constexpr uint32_t int_bytes_count = sizeof(int32_t); // is 4
			std::vector<byte> bytes = std::vector<byte>(int_bytes_count);

			// rewrite bytes
			for (uint8_t i = 0; i < int_bytes_count; i++)
				bytes[i] = number >> (i * 8);

			// remove zero bytes
			while (bytes.back() == 0)
				bytes.pop_back();

			return bytes;
		}
		/**
		 * \brief convert int64_t to bytes
		 * \param number convertible value
		 * \return vector of bytes
		 */
		static std::vector<byte> long_to_bytes(const int64_t number)
		{
			if (number < 0) // when number less then zero
				throw std::invalid_argument("number");
			if (number == 0) // when number eq to zero
				return {};

			// init const and bytes
			constexpr uint32_t int_bytes_count = sizeof(int64_t); // is 8
			std::vector<byte> bytes = std::vector<byte>(int_bytes_count);

			// rewrite bytes
			for (uint8_t i = 0; i < int_bytes_count; i++)
				bytes[i] = static_cast<byte>(number >> (i * 8)) & 0xFFFFFFFF;

			// remove zero bytes
			while (bytes.back() == 0)
				bytes.pop_back();

			return bytes;
		}
		/**
		 * \brief convert uint64_t to bytes
		 * \param number convertible value
		 * \return vector of bytes
		 */
		static std::vector<numbers::byte> ulong_to_bytes(const uint64_t number)
		{
			if (number == 0) // when number eq to zero
				return {};

			// init const and bytes
			constexpr uint32_t int_bytes_count = sizeof(int64_t); // is 8
			std::vector<byte> bytes = std::vector<byte>(int_bytes_count);

			// rewrite bytes
			for (uint8_t i = 0; i < int_bytes_count; i++)
				bytes[i] = static_cast<byte>(number >> (i * 8)) & 0xFFFFFFFF;

			// remove zero bytes
			while (bytes.back() == 0)
				bytes.pop_back();

			return bytes;
		}

		static sbyte compare(const fast_big_integer& a, const fast_big_integer& b)
		{
			const size_t a_size = a.bytes_.size();
			const size_t b_size = b.bytes_.size();

			if (a_size < b_size)
				return -1;
			if (a_size > b_size)
				return 1;
			if (a_size == 0)
				return 0;

			for (size_t i = a_size - 1; i != std::numeric_limits<size_t>::max(); i--)
			{
				if (a.bytes_[i] > b.bytes_[i])
					return 0;
				if (a.bytes_[i] < b.bytes_[i])
					return -1;
			}

			return 0;
		}
		static std::strong_ordering compare_optimized(const fast_big_integer& a, const fast_big_integer& b)
		{
			throw std::exception("not implemented");
		}

		/**
		 * \brief sum two numbers
		 * \param destination first number for sum (store the result)
		 * \param source second number for sum
		 */
		static void add_classic(fast_big_integer& destination, const fast_big_integer& source)
		{
			if (destination.is_zero() && source.is_zero())
				return;
			if (source.is_zero())
				return;
			if (destination.is_zero())
			{
				destination = source;
				return;
			}

			if (destination.bytes_.size() < source.bytes_.size())
				destination.bytes_.resize(source.bytes_.size());

			byte carry = 0;

			auto destination_data = destination.bytes_.begin();
			auto source_data = source.bytes_.begin();

			auto counter = source.bytes_.size();
			auto addition_iterations = destination.bytes_.size() - counter;

			while (counter-- != 0)
			{
				const short sum = static_cast<short>(*destination_data + *source_data++ + carry);
				*destination_data++ = static_cast<byte>(sum & 0xFF);
				carry = static_cast<byte>(sum >> 8);
			}

			while (addition_iterations-- != 0)
			{
				const short sum = static_cast<short>(*destination_data + carry);
				*destination_data++ = static_cast<byte>(sum & 0xFF);
				carry = static_cast<byte>(sum >> 8);
			}

			if (carry != 0)
				destination.bytes_.push_back(carry);
		}
		/**
		 * \brief sub source from destination
		 * \param destination first number (store the result)
		 * \param source second number
		 */
		static void sub_classic(fast_big_integer& destination, const fast_big_integer& source)
		{
			for (size_t i = 0; i < source.bytes_.size(); i++)
			{
				if (destination.bytes_[i] < source.bytes_[i])
				{
					auto data = destination.bytes_.begin() + i;

					while (*++data == 0)
						*data = 0xFF;

					*data -= 1;
				}

				destination.bytes_[i] -= source.bytes_[i];
			}

			fast_big_integer::clear_zero_bytes(destination);
		}
		/**
		 * \brief perform division by Newton-Raphson algorithm (https://en.wikipedia.org/wiki/Newton%27s_method)
		 * \param dividend number to be divided
		 * \param divisor is divider (NOT EQUAL TO ZERO)
		 * \param quotient store the result of division
		 * \param remainder store the remainder
		 */
		static void div_classic(const fast_big_integer& dividend, const fast_big_integer& divisor, fast_big_integer& quotient, fast_big_integer& remainder)
		{
			if (divisor.is_zero())
				throw std::invalid_argument("divisor is zero");

			const fast_big_integer _dividend = dividend, _divisor = divisor;
			const size_t k = _dividend.num_bits() + _divisor.num_bits();

			const fast_big_integer pow2 = fast_big_integer::extensions::pow2(k + 1);

			fast_big_integer x = _dividend - _divisor;
			fast_big_integer last_x;

			while(last_x != x)
			{
				last_x = x;
				x = (x * (pow2 - x * divisor)) >> k;
			}

			quotient = _dividend * x >> k;

			if (_dividend - (quotient * _divisor) >= _divisor)
				++quotient;

			remainder = _dividend - (quotient * divisor);
		}

		/**
		 * \brief remove all back bytes, which equal to zero
		 * \param bytes to clear
		 */
		static void clear_zero_bytes(std::vector<byte>& bytes)
		{
			while (!bytes.empty())
			{
				if (bytes.back() != 0)
					break;

				bytes.pop_back();
			}
		}
		/**
		 * \brief remove all back bytes, which equal to zero
		 * \param number to clear
		 */
		static void clear_zero_bytes(fast_big_integer& number)
		{
			fast_big_integer::clear_zero_bytes(number.bytes_);
		}

		static std::vector<byte> create_from_hex_string(const std::string& hex_string)
		{
			struct hex_table {
				long long tab[128];
				constexpr hex_table() : tab{} {
					tab['1'] = 1;
					tab['2'] = 2;
					tab['3'] = 3;
					tab['4'] = 4;
					tab['5'] = 5;
					tab['6'] = 6;
					tab['7'] = 7;
					tab['8'] = 8;
					tab['9'] = 9;
					tab['a'] = 10;
					tab['A'] = 10;
					tab['b'] = 11;
					tab['B'] = 11;
					tab['c'] = 12;
					tab['C'] = 12;
					tab['d'] = 13;
					tab['D'] = 13;
					tab['e'] = 14;
					tab['E'] = 14;
					tab['f'] = 15;
					tab['F'] = 15;
				}
				constexpr int16_t hex_to_int(const char number) const
				{
					return tab[static_cast<int16_t>(number)];
				}
				
			} constexpr hex_table;

			std::vector<byte> bytes; bytes.reserve(hex_string.size() / 2 + hex_string.size() % 2);

			for (size_t i = (hex_string.length() >> 1) - 1; i != std::numeric_limits<size_t>::max(); i--)
			{
				const byte b1 = static_cast<byte>(hex_table.hex_to_int(hex_string[i * 2])) << 4;
				const byte b2 = static_cast<byte>(hex_table.hex_to_int(hex_string[i * 2 + 1]));
				bytes.push_back(b1 + b2);
			}

			return bytes;
		}
		static std::string create_hex_string(const std::vector<byte>& bytes)
		{
			if (bytes.empty())
				return "00";

			std::stringstream hex_view;

			for (size_t i = 0; i < bytes.size(); i++)
			{
				if (bytes[bytes.size() - i - 1] < 16)
					hex_view << '0';

				hex_view << std::uppercase << std::hex << static_cast<uint16_t>(bytes[bytes.size() - 1 - i]);
			}

			return hex_view.str();
		}
		
	public:

		/**
		 * \brief contains all extensions methods to working
		 */
		struct extensions
		{
			/**
			 * \param a first number
			 * \param b second number
			 * \return largest of the numbers
			 */
			static fast_big_integer max(const fast_big_integer& a, const fast_big_integer& b)
			{
				return a > b ? a : b;
			}
			/**
			 * \param a first number
			 * \param b second number
			 * \return smallest of numbers
			 */
			static fast_big_integer min(const fast_big_integer& a, const fast_big_integer& b)
			{
				return a < b ? a : b;
			}

			/**
			 * \param a fist number
			 * \param b second number
			 * \return greatest common divisor number
			 */
			static fast_big_integer gcd(const fast_big_integer& a, const fast_big_integer& b)
			{
				fast_big_integer tmp_a = a, tmp_b = b;

				while (tmp_b.is_not_zero())
				{
					fast_big_integer rem, q;
					fast_big_integer::div_classic(tmp_a, tmp_b, q, rem);

					tmp_a = tmp_b;
					tmp_b = rem;
				}

				return tmp_a;
			}
			/**
			 * \param a fist number
			 * \param b second number
			 * \return least common multiple number
			 */
			static fast_big_integer lcm(const fast_big_integer& a, const fast_big_integer& b)
			{
				return (a * b / fast_big_integer::extensions::gcd(a, b));
			}

			/**
			 * \param max_byte_count max random byte count
			 * \return random whole number
			 */
			static fast_big_integer random(uint32_t max_byte_count = 1000u)
			{
				if (max_byte_count == 0)
					return fast_big_integer::zero();

				std::mt19937 rnd(std::random_device().operator()());
				const std::uniform_int_distribution<std::mt19937::result_type> rand_size(0, max_byte_count);

				const uint32_t size = rand_size(rnd);
				if (size == 0) return fast_big_integer::zero();

				std::vector<byte> random_bytes(size);
				const std::uniform_int_distribution<std::mt19937::result_type> rand_byte(0, 255);

				for (size_t i = 0; i < size; i++)
					random_bytes[i] = static_cast<byte>(rand_byte(rnd));

				fast_big_integer::clear_zero_bytes(random_bytes);

				return random_bytes;
			}

			/**
			 * \param hex_string anything hex string
			 * \return random whole number
			 */
			static fast_big_integer create_from_hex(std::string hex_string)
			{
				if (hex_string.empty())
					return fast_big_integer::zero();
				
				if (hex_string.size() % 2 == 1)
					hex_string.insert(0, 1, '0');
				
				return fast_big_integer::create_from_hex_string(hex_string);
			}

			static fast_big_integer pow(const fast_big_integer& n, const fast_big_integer& power)
			{
				return n.pow(power);
			}
			static fast_big_integer factorial(const fast_big_integer& n)
			{
				return n.factorial();
			}
			static fast_big_integer factorial_parallel(const fast_big_integer& n)
			{
				return n.factorial_parallel();
			}
			static fast_big_integer log_n(const fast_big_integer& x, size_t n)
			{
				return x.log_n(n);
			}
			static fast_big_integer log_2(const fast_big_integer& x)
			{
				return x.log_n(2);
			}
			static fast_big_integer sqrt(const fast_big_integer& n)
			{
				return n.sqrt();
			}
			static fast_big_integer is_power_of_two(const fast_big_integer& n)
			{
				return n.is_power_of_two();
			}
			static fast_big_integer pow2(size_t power)
			{
				if (power == 0)
					return { 1 };
				if (power == 1)
					return { 2 };
				
				constexpr byte eight = 8; // eight bits in byte
				const size_t byte_count = power / eight;
				const size_t bits_shift_count = power - byte_count * eight;

				std::vector<byte> bytes(byte_count + 1);
				bytes.back() = 1 << bits_shift_count;

				return bytes;
			}

		private:
			extensions() = default;
		};

		/**
		 * \brief init number to default value (is zero)
		 */
		fast_big_integer() = default;
		/**
		 * \brief init number from string
		 * \param decimal_string number in dec format
		 */
		fast_big_integer(const std::string& decimal_string)
		{
			const converters::dec2hex_bbc d2h;
			const std::string hex_str = d2h.convert(decimal_string);

			this->bytes_ = fast_big_integer::create_from_hex_string(hex_str);
		}
		/**
		 * \brief init number from byte vector
		 * \param bytes future number bytes
		 */
		fast_big_integer(std::vector<byte> bytes)
		{
			fast_big_integer::clear_zero_bytes(bytes);
			this->bytes_ = bytes;
		}
		/**
		 * \brief init number from byte array
		 * \param bytes future number bytes
		 */
		fast_big_integer(const std::initializer_list<byte> bytes)
		{
			std::vector<byte> bts = bytes;
			fast_big_integer::clear_zero_bytes(bts);
			this->bytes_ = bts;
		}
		/**
		 * \brief init number from int64_t value
		 * \param number any number
		 */
		fast_big_integer(int64_t number) : bytes_(fast_big_integer::long_to_bytes(number)) {}
		/**
		 * \brief init number from uint64_t value
		 * \param number any number
		 */
		fast_big_integer(uint64_t number) : bytes_(fast_big_integer::ulong_to_bytes(number)) {}
		/**
		 * \brief init number from int32_t value
		 * \param number any number
		 */
		fast_big_integer(int32_t number) : bytes_(fast_big_integer::int_to_bytes(number)) {}
		~fast_big_integer()
		{
			this->bytes_.clear();
		}

		/**
		 * \return number with value "0"
		 */
		static fast_big_integer zero()
		{
			static fast_big_integer zero;
			return zero;
		}
		/**
		 * \return number with value "1"
		 */
		static fast_big_integer one()
		{
			static fast_big_integer one = { 1 };
			return one;
		}
		/**
		 * \return number with value "2"
		 */
		static fast_big_integer two()
		{
			static fast_big_integer two = { 2 };
			return two;
		}

		/**
		 * \return dec string view of number
		 */
		[[nodiscard]] std::string to_string() const
		{
			const converters::hex2dec_bbc h2d;

			const std::string hex_str = fast_big_integer::create_hex_string(this->bytes_);
			const std::string dec_string = h2d.convert(hex_str);

			return dec_string;
		}
		/**
		 * \return hex string view of number
		 */
		[[nodiscard]] std::string to_string_hex() const
		{
			return fast_big_integer::create_hex_string(this->bytes_);
		}
		/**
		 * \return byte vector
		 */
		[[nodiscard]] std::vector<byte> to_bytes() const
		{
			return this->bytes_;
		}

		/**
		 * \brief perform AND operation
		 * \param number operand
		 * \return AND operation result between this and number
		 */
		fast_big_integer and (const fast_big_integer& number) const
		{
			const size_t result_size = std::min(this->bytes_.size(), number.bytes_.size());
			std::vector<byte> result_bytes(result_size);

			for (size_t i = 0; i < result_size; i++)
				result_bytes[i] = number.bytes_[i] & this->bytes_[i];

			fast_big_integer::clear_zero_bytes(result_bytes);

			return result_bytes;
		}
		/**
		 * \brief perform OR operation
		 * \param number operand
		 * \return OR operation result between this and number
		 */
		fast_big_integer or (const fast_big_integer & number) const
		{
			const size_t calculated_part_size = std::min(this->bytes_.size(), number.bytes_.size());
			const size_t filled_part_size = std::max(this->bytes_.size(), number.bytes_.size()) - calculated_part_size;
			auto& greater_number_bytes = this->bytes_.size() > number.bytes_.size() ? this->bytes_ : number.bytes_;
			std::vector<byte> result_bytes(calculated_part_size + filled_part_size);

			for (size_t i = 0; i < calculated_part_size; i++)
				result_bytes[i] = number.bytes_[i] | this->bytes_[i];

			for (size_t i = calculated_part_size; i < filled_part_size + calculated_part_size; i++)
				result_bytes[i] = greater_number_bytes[i];

			fast_big_integer::clear_zero_bytes(result_bytes);

			return result_bytes;
		}
		/**
		 * \brief perform XOR operation
		 * \param number operand
		 * \return XOR operation result between this and number
		 */
		fast_big_integer xor (const fast_big_integer& number) const
		{
			const size_t calculated_part_size = std::min(this->bytes_.size(), number.bytes_.size());
			const size_t filled_part_size = std::max(this->bytes_.size(), number.bytes_.size()) - calculated_part_size;
			auto& greater_number_bytes = this->bytes_.size() > number.bytes_.size() ? this->bytes_ : number.bytes_;
			std::vector<byte> result_bytes(calculated_part_size + filled_part_size);

			for (size_t i = 0; i < calculated_part_size; i++)
				result_bytes[i] = number.bytes_[i] ^ this->bytes_[i];

			for (size_t i = calculated_part_size; i < filled_part_size + calculated_part_size; i++)
				result_bytes[i] = greater_number_bytes[i];

			fast_big_integer::clear_zero_bytes(result_bytes);

			return result_bytes;
		}

		/**
		 * \brief perform shift bits to right
		 * \param shift_count bits shift count
		 */
		void fast_shr(const size_t shift_count)
		{
			if(this->is_zero())
				return;

			constexpr byte eight = 8; // 1 byte contains 8 bits
			const size_t byte_shift_count = shift_count / eight;

			if(byte_shift_count >= this->bytes_.size())
			{
				this->bytes_.clear();
				return;
			}
			
			if(byte_shift_count != 0)
				this->bytes_.erase(this->bytes_.begin(), this->bytes_.begin() + byte_shift_count);
			
			size_t counter = shift_count - byte_shift_count * eight; // get bits shift count
			
			for(byte* data = &this->bytes_.front(), i = this->bytes_.size();
				counter != 0; 
				counter--, data = &this->bytes_.front(), i = this->bytes_.size())
			{
				*data >>= 1;
				
				while(data++, --i != 0)
				{
					*(data - 1) ^= *data << 7; // branch-less version
					// eq to:
					//	if (*data & 0x01) // 0x01 = 1
					//		*(data - 1) ^= 0x80; // 0x80 = 128
					*data >>= 1;
				}
			}

			fast_big_integer::clear_zero_bytes(this->bytes_);
		}
		/**
		 * \brief perform shift bits to left
		 * \param shift_count bits shift count
		 */
		void fast_shl(size_t shift_count)
		{
			if (this->is_zero())
				return;

			constexpr byte eight = 8; // 8 bits in 1 byte
			const size_t byte_shift_count = shift_count / eight;
			this->bytes_.insert(this->bytes_.begin(), byte_shift_count, 0); // insert empty bytes_ in begin of vector

			auto counter = shift_count - byte_shift_count * eight;
			if (counter == 0) return; // why shift if you can not shift

			this->bytes_.push_back(0); // insert last byte for shifting bits
			byte* data = nullptr;
			uint64_t i = 0;
			while (i = this->bytes_.size() - byte_shift_count - 1, data = &(this->bytes_.back()) - 1, counter-- != 0)
			{
				*(data + 1) <<= 1;
				*(data + 1) ^= *data >> 7;
				*data <<= 1;

				while (data--, --i != 0)
				{
					*(data + 1) ^= *data >> 7;
					*data <<= 1;
				}
			}

			fast_big_integer::clear_zero_bytes(this->bytes_);
		}

		/**
		 * \brief perform addition to this number
		 * \param number addition number
		 */
		void add(const fast_big_integer& number)
		{
			fast_big_integer::add_classic(*this, number);
		}
		/**
		 * \param number addition number
		 * \return sum of this and parameter numbers
		 */
		[[nodiscard]] fast_big_integer sum(const fast_big_integer& number) const
		{
			fast_big_integer sum_result = *this;
			fast_big_integer::add_classic(sum_result, number);

			return sum_result;
		}

		/**
		 * \brief perform subtraction from this number
		 * \param number subtrahend number
		 */
		void sub(const fast_big_integer& number)
		{
			fast_big_integer::sub_classic(*this, number);
		}
		/**
		 * \param number subtrahend number
		 * \return difference between this and parameter
		 */
		[[nodiscard]] fast_big_integer difference(const fast_big_integer& number) const
		{
			fast_big_integer sub_result = *this;
			fast_big_integer::sub_classic(sub_result, number);

			return sub_result;
		}

		/**
		 * \brief perform multiply between this and parameter
		 * \param number multiplier
		 */
		void mul(const fast_big_integer& number)
		{
			struct fft
			{
				__declspec(noinline) static uint64_t calculate_expr_value(const std::vector<byte>& a, const std::vector<byte>& b,
					const size_t left, const size_t right)
				{
					uint64_t sum = 0;
					size_t loc_left = left - 1, loc_right = right + 1;

					while (++loc_left < --loc_right)
					{
						const uint32_t pairs_value = a[loc_left] * b[loc_right] + a[loc_right] * b[loc_left];
						sum += pairs_value;
					}

					if (loc_left == loc_right)
						sum += static_cast<uint64_t>(a[loc_left])* static_cast<uint64_t>(b[loc_right]);

					return sum;
				}
			};

			if (this->is_zero() || number.is_zero())
			{
				this->bytes_.clear(); // set zero
				return;;
			}

			if (number.is_one())
				return;

			if (this->is_one())
			{
				this->bytes_ = number.bytes_;
				return;
			}

			std::vector<byte> mul_vector_a(this->bytes_);
			std::vector<byte> mul_vector_b(number.bytes_);

			const size_t total_multipliers_size = std::max(this->bytes_.size(), number.bytes_.size());

			mul_vector_a.resize(total_multipliers_size);
			mul_vector_b.resize(total_multipliers_size);

			const size_t _left = 0, _right = total_multipliers_size - 1;
			const uint16_t first_expr_value = mul_vector_a[_left] * mul_vector_b[_left];
			const uint16_t last_expr_value = mul_vector_a[_right] * mul_vector_b[_right];

			if (total_multipliers_size == 1)
			{
				std::vector<byte> result =
				{
					static_cast<byte>(first_expr_value),
					static_cast<byte>(first_expr_value >> 8)
				};

				if (result[1] == 0)
					result.pop_back();

				this->bytes_ = result;

				return;
			}

			uint64_t middle_expr_value = fft::calculate_expr_value(mul_vector_a, mul_vector_b, _left, _right);

			std::vector<std::pair<byte, uint64_t>> fft_calculations; fft_calculations.resize(total_multipliers_size * 2 - 1);
			fft_calculations[0] = std::pair<byte, uint64_t>(static_cast<byte>(first_expr_value), first_expr_value >> 8);

			size_t current_fft_calc_index = 1; // because first is inserted

			for (size_t i = 1; i < _right; i++)
			{
				uint64_t value = fft::calculate_expr_value(mul_vector_a, mul_vector_b, _left, i);
				value += fft_calculations[current_fft_calc_index - 1].second;

				fft_calculations[current_fft_calc_index++] = std::pair<byte, uint64_t>(static_cast<byte>(value), value >> 8);
			}

			middle_expr_value += fft_calculations[current_fft_calc_index - 1].second;
			fft_calculations[current_fft_calc_index++] = std::pair<byte, uint64_t>(static_cast<byte>(middle_expr_value), middle_expr_value >> 8);

			for (size_t i = 1; i < _right; i++)
			{
				uint64_t value = fft::calculate_expr_value(mul_vector_a, mul_vector_b, i, _right);
				value += fft_calculations[current_fft_calc_index - 1].second;

				fft_calculations[current_fft_calc_index++] = std::pair<byte, uint64_t>(static_cast<byte>(value), value >> 8);
			}

			fft_calculations[current_fft_calc_index] = std::pair<byte, uint64_t>(
				static_cast<byte>(last_expr_value + fft_calculations[current_fft_calc_index - 1].second),
				(last_expr_value + fft_calculations[current_fft_calc_index - 1].second) >> 8
				);

			std::vector<byte> result_mul_vector; result_mul_vector.reserve(total_multipliers_size * 2);
			for (std::pair<byte, uint64_t>& fft_calculation : fft_calculations)
				result_mul_vector.push_back(fft_calculation.first);

			const uint64_t last_carry = fft_calculations.back().second;
			std::vector<byte> carry_bytes = fast_big_integer::ulong_to_bytes(last_carry);
			if (!carry_bytes.empty())
			{
				result_mul_vector.reserve(result_mul_vector.size() + carry_bytes.size());
				result_mul_vector.insert(result_mul_vector.end(), carry_bytes.begin(), carry_bytes.end());
			}

			fast_big_integer::clear_zero_bytes(result_mul_vector);

			this->bytes_ = result_mul_vector;
		}
		/**
		 * \param multiplier multiplier
		 * \return product between this and parameter
		 */
		[[nodiscard]] fast_big_integer product(const fast_big_integer& multiplier) const
		{
			fast_big_integer result = *this;
			result.mul(multiplier);

			return result;
		}

		/**
		 * \param divisor divisor
		 * \return quotient
		 */
		[[nodiscard]] fast_big_integer division(const fast_big_integer& divisor) const
		{
			fast_big_integer quotient, remainder;
			fast_big_integer::div_classic(*this, divisor, quotient, remainder);

			return quotient;
		}
		/**
		 * \param divisor divisor
		 * \param remainder will write by remainder
		 * \return quotient
		 */
		[[nodiscard]] fast_big_integer division_reminder(const fast_big_integer& divisor, fast_big_integer& remainder) const
		{
			fast_big_integer quotient;
			fast_big_integer::div_classic(*this, divisor, quotient, remainder);

			return quotient;
		}
		
		/**
		 * \return if this is zero -> true, else -> false
		 */
		[[nodiscard]] bool is_zero() const
		{
			return this->bytes_.empty();
		}
		/**
		 * \return if this is zero -> false, else -> true
		 */
		[[nodiscard]] bool is_not_zero() const
		{
			return !this->is_zero();
		}

		/**
		 * \return if this is one -> true, else -> false
		 */
		[[nodiscard]] bool is_one() const
		{
			return this->bytes_.size() == 1 && bytes_.front() == 1;
		}
		/**
		 * \return if this is two -> true, else -> false
		 */
		[[nodiscard]] bool is_two() const
		{
			return this->bytes_.size() == 1 && bytes_.front() == 2;
		}
		/**
		 * \return if this is power of two -> true, else -> false
		 */
		[[nodiscard]] bool is_power_of_two() const
		{
			if (this->is_zero())
				return false;

			for (size_t i = 0; i < bytes_.size() - 1; i++)
			{
				if (this->bytes_[i] != 0)
					return false;
			}

			return (this->bytes_.back() & this->bytes_.back() - 1) == 0;
		}

		/**
		 * \return bits count in number (bytes count × 8)
		 */
		[[nodiscard]] size_t num_bits() const
		{
			// in one byte eight bits :)
			return this->bytes_.size() << 3; // * 8
		}
		/**
		 * \return if this is odd -> true, else -> false
		 */
		[[nodiscard]] bool is_odd() const
		{
			return !this->bytes_.empty() && this->bytes_.front() & 1;
		}
		/**
		 * \return if this is even -> true, else -> false
		 */
		[[nodiscard]] bool is_even() const
		{
			return !this->is_odd();
		}

		/**
		 * \brief set value of this number to zero
		 */
		void set_zero()
		{
			this->bytes_.clear();
		}

		/**
		 * \brief perform prefix increment
		 * \return this incremented value
		 */
		fast_big_integer& operator ++ ()
		{
			if (this->is_zero())
			{
				this->bytes_.push_back(1);
				return *this;
			}

			auto byte_iterator = this->bytes_.begin();
			const auto end = this->bytes_.end();

			while (++*byte_iterator == 0)
			{
				if (++byte_iterator == end)
					break;
			}

			if (byte_iterator == end)
				this->bytes_.push_back(1);

			return *this;
		}
		/**
		 * \brief perform prefix decrement
		 * \return this decremented value
		 */
		fast_big_integer& operator-- ()
		{
			if (this->is_zero())
				throw std::invalid_argument("number can not be less then zero");

			auto bytes_iterator = this->bytes_.begin();

			if (--*bytes_iterator == 0xFF)
			{
				while (*++bytes_iterator == 0)
					*bytes_iterator = 0xFF;

				--*bytes_iterator;
			}

			if (this->bytes_.back() == 0)
				this->bytes_.pop_back();

			return *this;
		}

		/**
		 * \brief perform postfix increment
		 * \return this incremented value
		 */
		fast_big_integer& operator ++ (int)
		{
			return ++*this;
		}
		/**
		 * \brief perform postfix decrement
		 * \return this decremented value
		 */
		fast_big_integer& operator-- (int)
		{
			return --*this;
		}

		fast_big_integer operator + (const fast_big_integer& number) const
		{
			return this->sum(number);
		}
		fast_big_integer operator - (const fast_big_integer& number) const
		{
			return this->difference(number);
		}
		fast_big_integer operator * (const fast_big_integer& number) const
		{
			return this->product(number);
		}
		fast_big_integer operator / (const fast_big_integer& number) const
		{
			return this->division(number);
		}
		fast_big_integer operator % (const fast_big_integer& number) const
		{
			fast_big_integer q, r;
			fast_big_integer::div_classic(*this, number, q, r);

			return r;
		}

		fast_big_integer& operator = (const fast_big_integer& number) = default;

		/**
		 * \param shift_count shift count
		 * \return shifted to left value
		 */
		fast_big_integer operator << (size_t shift_count) const
		{
			fast_big_integer copy = *this;
			copy.fast_shl(shift_count);

			return copy;
		}
		/**
		 * \brief perform shift to left
		 * \param shift_count shift count
		 */
		void operator <<= (size_t shift_count)
		{
			this->fast_shl(shift_count);
		}
		/**
		 * \param shift_count shift count
		 * \return shifted to right value
		 */
		fast_big_integer operator >> (size_t shift_count) const
		{
			fast_big_integer copy = *this;
			copy.fast_shr(shift_count);

			return copy;
		}
		/**
		 * \brief perform shift to right
		 * \param shift_count shift count
		 */
		void operator >>= (size_t shift_count)
		{
			this->fast_shr(shift_count);
		}

		/**
		 * \brief AND operator
		 * \param number number
		 * \return result of "AND" operation
		 */
		fast_big_integer operator & (const fast_big_integer& number) const
		{
			return this-> and (number);
		}
		/**
		 * \brief OR operator
		 * \param number number
		 * \return result of "OR" operation
		 */
		fast_big_integer operator | (const fast_big_integer& number) const
		{
			return this-> xor (number);
		}
		/**
		 * \brief XOR operator
		 * \param number number
		 * \return result of "XOR" operation
		 */
		fast_big_integer operator ^ (const fast_big_integer& number) const
		{
			return this-> or (number);
		}

		/**
		 * \param exponent exponent
		 * \return this in power of parameter
		 */
		[[nodiscard]] fast_big_integer pow(const fast_big_integer& exponent) const
		{
			if (exponent.is_zero())
				return fast_big_integer::one();
			if (exponent.is_one())
				return *this;

			fast_big_integer power = exponent;
			fast_big_integer result = fast_big_integer::one();
			fast_big_integer x = *this;

			while (power.is_not_zero())
			{
				if (power.is_even())
				{
					x = x * x;
					power >>= 1;
				}
				else
				{
					result = result * x;
					x = x * x;
					--power >>= 1;
				}
			}

			return result;
		}
		/**
		 * \return factorial of this number
		 */
		[[nodiscard]] fast_big_integer factorial() const
		{
			struct factorial
			{
				static fast_big_integer prod_tree(const fast_big_integer& l, const fast_big_integer& r)
				{
					const sbyte compare_result = fast_big_integer::compare(l, r);
					if (compare_result == 1)
						return fast_big_integer::one();
					if (compare_result == 0)
						return l;
					if ((r - l).is_one())
						return r * l;

					fast_big_integer m = l; m.add(r); m.fast_shr(1);
					fast_big_integer m_plus_one = m; ++m_plus_one;

					return factorial::prod_tree(l, m) * factorial::prod_tree(m_plus_one, r);
				}
			};

			if (this->is_zero())
				return fast_big_integer::one();

			if (this->is_one() || this->is_two())
				return *this;

			return factorial::prod_tree(fast_big_integer::two(), *this);
		}
		[[nodiscard]] fast_big_integer factorial_parallel() const
		{
			struct factorial_parallel
			{
				// calculate tree on one thread
				static fast_big_integer prod_tree_one_thread(const fast_big_integer& l, const fast_big_integer& r)
				{
					const sbyte compare_result = fast_big_integer::compare(l, r);
					if (compare_result == 1)
						return fast_big_integer::one();
					if (compare_result == 0)
						return l;
					if ((r - l).is_one())
						return r * l;

					fast_big_integer m = l; m.add(r); m.fast_shr(1);
					fast_big_integer m_plus_one = m; ++m_plus_one;

					return factorial_parallel::prod_tree_one_thread(l, m) * factorial_parallel::prod_tree_one_thread(m_plus_one, r);
				}

				static std::vector<std::pair<fast_big_integer, fast_big_integer>>
					split_factorial_tree(const fast_big_integer& l, const fast_big_integer& r, const size_t thread_count)
				{
					std::vector<std::pair<fast_big_integer, fast_big_integer>> splitted_tree;
					splitted_tree.reserve(thread_count);

					const fast_big_integer step = (r - l) / thread_count;
					sbyte compare_result = -1;

					for (fast_big_integer i = l; compare_result == -1; )
					{
						fast_big_integer next_step_value = i + step;

#ifdef _MSC_VER
						splitted_tree.emplace_back(i, next_step_value);
#elif
						splitted_tree.push_back(
							std::make_pair(i, next_step_value)
						);
#endif
						i = ++next_step_value;

						compare_result = fast_big_integer::compare(i, r);
					}

					if (compare_result == 1)
						splitted_tree.back().second = r;

					return splitted_tree;
				}

				static fast_big_integer parallel_prod_tree(const fast_big_integer& l, const fast_big_integer& r)
				{
					hpc::thread_pool<fast_big_integer>& thread_pool = hpc::thread_pool<fast_big_integer>::get_instance();

					const size_t thread_count = thread_pool.threads_capacity();

					std::vector<std::future<fast_big_integer>> future_results(thread_count);

					auto tree = factorial_parallel::split_factorial_tree(l, r, thread_count);

					for (size_t i = 0; i < thread_count; i++)
					{
						fast_big_integer& f = tree[i].first;
						fast_big_integer& s = tree[i].second;

						future_results[i] = thread_pool.run([&f, &s]()
						{
							return factorial_parallel::prod_tree_one_thread(f, s);
						});
					}

					thread_pool.wait_all_jobs();

					// copy results
					std::vector<fast_big_integer> numbers_for_mul(thread_count);
					for (size_t i = 0; i < thread_count; i++)
						numbers_for_mul[i] = future_results[i].get();

					// clear temporary arrays
					future_results.clear();

					// resize
					future_results.resize(thread_count / 2);

					size_t mul_numbers_tree_count = thread_count / 2;
					while (mul_numbers_tree_count >= 2)
					{
						std::vector<fast_big_integer> next_calc_numbers; next_calc_numbers.reserve(mul_numbers_tree_count);

						for (size_t i = 0; i < numbers_for_mul.size() / 2; i++)
						{
							fast_big_integer& f = numbers_for_mul[i * 2];
							fast_big_integer& s = numbers_for_mul[i * 2 + 1];

							future_results[i] = thread_pool.run([&f, &s]()
							{
								return f * s;
							});
						}

						if (numbers_for_mul.size() % 2 == 1)
							next_calc_numbers.push_back(numbers_for_mul.back());

						thread_pool.wait_all_jobs();

						for (std::future<fast_big_integer>& future_result : future_results)
							next_calc_numbers.push_back(future_result.get());

						numbers_for_mul = next_calc_numbers;

						future_results.clear();

						future_results.resize(numbers_for_mul.size() / 2);

						mul_numbers_tree_count /= 2;
					}

					fast_big_integer result = numbers_for_mul.front() * numbers_for_mul.back();

					//if(numbers_for_mul.size() == 3)
					//	result = numbers_for_mul[1] * result;

					return result;
				}
			};

			if (this->is_zero())
				return fast_big_integer::one();

			if (this->is_one() || this->is_two())
				return *this;

			if (this->bytes_.size() == 1 || std::thread::hardware_concurrency() < 2)
				return this->factorial();

			return factorial_parallel::parallel_prod_tree(fast_big_integer::two(), *this);
		}
		/**
		 * \return sqrt of this number
		 */
		[[nodiscard]] fast_big_integer sqrt() const
		{
			fast_big_integer x0 = *this;
			fast_big_integer x1 = *this; ++x1; x1.fast_shr(1);

			while (x1 < x0)
			{
				x0 = x1;
				x1 = x1 + (*this / x1); x1.fast_shr(1);
			}

			return x0;
		}
		/**
		 * \param n base
		 * \return the base n logarithm of this
		 */
		[[nodiscard]] fast_big_integer log_n(size_t n) const
		{
			size_t log = 0u;
			fast_big_integer tmp_this = *this;

			while (!tmp_this.is_one() && tmp_this.is_not_zero())
			{
				++log;
				
				if(tmp_this >= n)
					tmp_this = tmp_this / n;
				else
					break;
			}

			return log;
		}
		/**
		 * \return the base two logarithm of this
		 */
		[[nodiscard]] fast_big_integer log2() const
		{
			uint64_t log = 0u;
			fast_big_integer tmp_this = *this;

			while (!tmp_this.is_one() && tmp_this.is_not_zero())
			{
				++log;
				tmp_this >>= 1;
			}

			return log;
		}

		std::strong_ordering operator <=> (const fast_big_integer& n) const
		{
			return fast_big_integer::compare(*this, n) <=> 0;
		}
		bool operator == (const fast_big_integer& n) const
		{
			return fast_big_integer::compare(*this, n) == 0;
		}
		bool operator != (const fast_big_integer& n) const
		{
			return fast_big_integer::compare(*this, n) != 0;
		}
	};
}
