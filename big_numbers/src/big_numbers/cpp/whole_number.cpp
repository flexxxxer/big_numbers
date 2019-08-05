#ifndef WHOLE_NUMBER_CPP
#define WHOLE_NUMBER_CPP

// #include <pmmintrin.h> // use SSE3
#include <string> // use std::string
#include <algorithm> // use std::transform
#include <tuple> // std::tuple
#include <utility>
#include <vector>
#include "../include/whole_number.h" // use numbers::whole_number

using namespace numbers;

const converters::base_converter numbers::whole_number::hex2dec = converters::base_converter::hex_to_decimal_converter();
const converters::base_converter numbers::whole_number::dec2hex = converters::base_converter::decimal_to_hex_converter();

std::vector<byte> whole_number::uint_to_bytes(const uint32_t number)
{
	if (number == 0)
		return std::vector<byte>();

	std::vector<byte> bts(4); // reserve 4 bytes

	for (uint32_t i = 0; i < 4; i++)
		bts[3 - i] = (number >> (i * 8));

	std::reverse(bts.begin(), bts.end());

	while (bts.back() == 0)
		bts.pop_back();

	return bts;
}
std::vector<byte> whole_number::ulong_to_bytes(const uint64_t number)
{
	if (number == 0)
		return std::vector<byte>();

	std::vector<byte> bts(8); // reserve 4 bytes

	for (uint32_t i = 0; i < 8; i++)
		bts[7 - i] = static_cast<byte>(number >> (i * 8)) & 0XFFFFFFFF;

	std::reverse(bts.begin(), bts.end());

	while (bts.back() == 0)
		bts.pop_back();

	return bts;
}

void whole_number::clear_zero_bytes(std::vector<byte>& bytes)
{
	while (bytes.size())
		if (bytes.back() != 0)
			break;
		else
			bytes.pop_back();
}
void whole_number::clear_zero_bytes(whole_number& number)
{
	whole_number::clear_zero_bytes(number.bytes);
}

sbyte whole_number::compare_optimized(const whole_number& a, const whole_number& b)
{
	if (a.bytes.size() > b.bytes.size())
		return 1;
	if (a.bytes.size() < b.bytes.size())
		return -1;

	const byte* a_data = a.bytes.data();
	const byte* b_data = b.bytes.data();
	int64_t index = a.bytes.size() - 1;

	for (; index % 16 != 0; index--)
	{
		if (a.bytes[index] > b.bytes[index])
			return 1;
		else if (a.bytes[index] < b.bytes[index])
			return -1;
	}

	for (index -= 16; index != -16; index -= 16)
	{
		const int64_t& a_front_part = a_data[index + 8];
		const int64_t& b_front_part = b_data[index + 8];

		if (a_front_part > b_front_part)
			return 1;
		
		if (a_front_part < b_front_part)
			return -1;

		const int64_t& a_back_part = a_data[index];
		const int64_t& b_back_part = b_data[index];

		if (a_back_part > b_back_part)
			return 1;
		if (a_back_part < b_back_part)
			return -1;
	}

	return 0;
}
sbyte whole_number::compare(const whole_number& a, const whole_number& b)
{
	const uint64_t a_size = a.bytes.size();
	const uint64_t b_size = b.bytes.size();

	if (a_size < b_size)
		return -1;
	if (a_size > b_size)
		return 1;

	for (int64_t i = a_size - 1; i > -1; i--)
	{
		if (a.bytes[i] > b.bytes[i])
			return 1;
		if (a.bytes[i] < b.bytes[i])
			return -1;
	}

	return 0;
}

void whole_number::add_logic_gate(whole_number& destination, const whole_number& source)
{
	whole_number carry_value = source;

	// Iterate till there is no carry
	while (carry_value.is_not_zero())
	{
		// carry now contains common  
		// set bits of x and y  
		auto carry = destination.and(carry_value);

		// Sum of bits of x and y where at  
		// least one of the bits is not set  
		destination = destination.xor(carry_value);

		// Carry is shifted by one so that adding  
		// it to x gives the required sum
		carry.shift_to_left(1);

		carry_value = carry;
	}
}
void whole_number::add_classic(whole_number& destination, const whole_number& source)
{
	if (destination.bytes.size() < source.bytes.size())
		destination.bytes.resize(source.bytes.size());

	byte carry = 0;

	auto destination_data = &destination.bytes.front();
	auto source_data = &source.bytes.front();

	auto counter = source.bytes.size();
	auto addition_iterations = destination.bytes.size() - counter;

	while (counter-- != 0)
	{
		const short sum = short(*destination_data + *source_data++ + carry);
		*destination_data++ = byte(sum & 0xFF);
		carry = byte(sum >> 8);
	}

	while (addition_iterations-- != 0)
	{
		const short sum = short(*destination_data + carry);
		*destination_data++ = byte(sum & 0xFF);
		carry = byte(sum >> 8);
	}

	if (carry != 0)
		destination.bytes.push_back(carry);
}

void whole_number::sub_logic_gate(whole_number& destination, const whole_number& source)
{
	if (destination.bytes.size() < source.bytes.size())
		throw std::invalid_argument("source");

	auto borrow_value = source;

	// Iterate till there is no carry
	while (borrow_value.is_not_zero())
	{
		// carry now contains common  
		// set bits of x and y  
		auto borrow = destination.not().and(borrow_value);

		// Sum of bits of x and y where at  
		// least one of the bits is not set  
		destination = destination. xor (borrow_value);

		// Carry is shifted by one so that adding  
		// it to x gives the required sum
		borrow.shift_to_left(1);

		borrow_value = borrow;
	}

	whole_number::clear_zero_bytes(destination);
}
void whole_number::sub_classic(whole_number& destination, const whole_number& source)
{
	for (uint64_t i = 0; i < source.bytes.size(); i++)
	{
		if (destination.bytes[i] < source.bytes[i])
		{
			auto data = destination.bytes.begin() + i;

			while (*++data == 0)
				*data = 0xFF;

			*data -= 1;
		}

		destination.bytes[i] -= source.bytes[i];
	}

	whole_number::clear_zero_bytes(destination);
}

void whole_number::div(const whole_number& dividend, const whole_number& divisor, whole_number& quotient, whole_number& remainder)
{
	if (divisor.is_zero())
		throw std::invalid_argument("divisor is zero");

	const whole_number _dividend = dividend, _divisor = divisor;
	uint64_t k = _dividend.num_bits() + _divisor.num_bits();

	whole_number pow2 = whole_number::one();
	pow2.shift_to_left(k + 1);

	whole_number x = _dividend.difference(_divisor);
	whole_number last_x, last_last_x;

	while (true)
	{
		x.mul(pow2.difference(x.product(_divisor))); // x = (x * (pow2 - x * _divisor));
		x.shift_to_right(k);

		if (whole_number::compare(x, last_x) == 0 ||
			whole_number::compare(x, last_last_x) == 0) break;

		last_last_x = last_x;
		last_x = x;
	}

	whole_number _quotient = _dividend.product(x);
	_quotient.shift_to_right(k);

	if (whole_number::compare(_quotient.product(_divisor),_dividend) == -1)
		_quotient.add(whole_number::one());

	if (whole_number::compare(_quotient.product(_divisor), _dividend) == 1) 
		_quotient.sub(whole_number::one()), remainder = _dividend.difference(_quotient.product(_divisor));
	else
		remainder.bytes.clear();

	quotient = _quotient;
}


whole_number::whole_number() = default;
whole_number::whole_number(std::string str) : whole_number()
{
	std::transform(str.begin(), str.end(), str.begin(),
		[](char symbol) -> char
		{
			if (symbol <= 'Z' && symbol >= 'A')
				return char(symbol - ('Z' - 'z'));
			else
				return symbol;
		}
	);

	if (str.length() == 0) throw std::invalid_argument("EMPTY NUMBER");
	if (str == "0") return;

	std::string hex_string = str;
	const std::string hex_map = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

	try
	{
		hex_string = whole_number::dec2hex.convert(str);
	}
	catch (...) {}

	const std::string numbers = "0123456789";
	const std::string characters = "abcdefABCDEF";

	for (char i : str)
	{
		if (numbers.find(i) == std::string::npos && characters.find(i) == std::string::npos)
		//if ((str[i] < 97 && str[i] > 122) && (str[i] < 48 && str[i] > 57))
			throw  std::invalid_argument("NOT CORRECT NUMBER");
	}

	if (hex_string.length() % 2) hex_string.insert(0, 1, '0');

	for (int64_t i = (hex_string.length() >> 1) - 1; i > -1; i--)
	{
		const byte b1 = static_cast<byte>(hex_map.rfind(hex_string[i * 2])) << 4;
		const byte b2 = static_cast<byte>(hex_map.rfind(hex_string[i * 2 + 1]));
		bytes.push_back(b1 + b2);
	}
}
whole_number::whole_number(std::vector<byte> bytes) : bytes(std::move(bytes)) { }
whole_number::whole_number(std::initializer_list<byte> bytes) : bytes(bytes) {}

whole_number::whole_number(const uint32_t number) : whole_number()
{
	if (number == 0)
		return;

	this->bytes = whole_number::uint_to_bytes(number);
}
whole_number::whole_number(const uint64_t number) : whole_number()
{
	if (number == 0)
		return;

	this->bytes = whole_number::ulong_to_bytes(number);
}
whole_number::~whole_number()
{
	this->bytes.~vector();
}

whole_number whole_number::one()
{
	whole_number one = { 1 };
	return one;
}


std::string whole_number::to_string() const
{
	std::string hex_str_view = this->to_string_hex();
	return hex2dec.convert(hex_str_view.erase(0, 2));
}
std::string whole_number::to_string_hex() const
{
	const std::string hexmap = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

	std::string s(bytes.size() * 2, ' ');
	for (uint64_t i = 0; i < bytes.size(); ++i) {
		s[2 * i] = hexmap[(bytes[i] & 0xF0) >> 4];
		s[2 * i + 1] = hexmap[bytes[i] & 0x0F];
	}

	std::string result = "0x";
	for (int64_t i = static_cast<int>(s.length()) - 2; i > -1; i -= 2)
	{
		result.push_back(s[i]);
		result.push_back(s[i + 1]);
	}
	return result;
}

whole_number whole_number::and (const whole_number& number) const
{
	const uint64_t result_size = std::min(this->bytes.size(), number.bytes.size());
	std::vector<byte> result_bytes(result_size);

	for (int i = 0; i < result_size; i++)
		result_bytes[i] = number.bytes[i] & this->bytes[i];

	return result_bytes;
}
whole_number whole_number::or (const whole_number& number) const
{
	size_t calculated_part_size = std::min(this->bytes.size(), number.bytes.size());
	size_t filled_part_size = std::max(this->bytes.size(), number.bytes.size()) - calculated_part_size;
	auto& greater_number_bytes = this->bytes.size() > number.bytes.size() ? this->bytes : number.bytes;
	std::vector<byte> result_bytes(calculated_part_size + filled_part_size);

	for (int i = 0; i < calculated_part_size; i++)
		result_bytes[i] = number.bytes[i] | this->bytes[i];

	for (uint64_t i = calculated_part_size; i < filled_part_size + calculated_part_size; i++)
		result_bytes[i] = greater_number_bytes[i];

	return result_bytes;
}
whole_number whole_number::xor (const whole_number& number) const
{
	const size_t calculated_part_size = std::min(this->bytes.size(), number.bytes.size());
	const size_t filled_part_size = std::max(this->bytes.size(), number.bytes.size()) - calculated_part_size;
	auto& greater_number_bytes = this->bytes.size() > number.bytes.size() ? this->bytes : number.bytes;
	std::vector<byte> result_bytes(calculated_part_size + filled_part_size);

	for (int i = 0; i < calculated_part_size; i++)
		result_bytes[i] = number.bytes[i] ^ this->bytes[i];

	for (uint64_t i = calculated_part_size; i < filled_part_size + calculated_part_size; i++)
		result_bytes[i] = greater_number_bytes[i];

	return result_bytes;
}
whole_number whole_number::not() const
{
	std::vector<byte> result_bytes(this->bytes.size());

	for (uint64_t i = 0; i < this->bytes.size(); i++)
		result_bytes[i] = ~this->bytes[i];

	return result_bytes;
}

void whole_number::shift_to_right(const uint64_t shift_count)
{
	// if this is zero => return;

	byte* data = nullptr;
	int64_t i = 0;
	auto counter = shift_count;

	// for (auto [i, f, s] = std::tuple{...};;)
	while (data = &this->bytes.front(), i = this->bytes.size(), counter-- != 0)
	{
		*data >>= 1;
		while (data++, --i != 0)
		{
			*(data - 1) ^= *data << 7; // branchless version
			// eq to:
			//	if (*data & 0x01) // 0x01 = 1
			//		*(data - 1) ^= 0x80; // 0x80 = 128
			
			*data >>= 1;
		}
	}

	whole_number::clear_zero_bytes(this->bytes);
}
void whole_number::shift_to_left(const uint64_t shift_count)
{
	// if this is zero => return;

	const byte eight = 0x8; // 8 bits in 1 byte
	const auto byte_shift_count = shift_count / eight;
	this->bytes.insert(this->bytes.begin(), byte_shift_count, 0); // insert empty bytes in begin of vector
	
	auto counter = shift_count - byte_shift_count * eight;
	if (counter == 0) return; // why shift if you can not shift

	this->bytes.push_back(0); // insert last byte for shifting bits
	byte* data = nullptr;
	uint64_t i = 0;
	while (i = this->bytes.size() - byte_shift_count - 1, data = &(this->bytes.back()) - 1, counter-- != 0)
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

	whole_number::clear_zero_bytes(this->bytes);
}

void whole_number::add(const whole_number& number)
{
	whole_number::add_classic(*this, number);
}
whole_number whole_number::sum(const whole_number& number) const
{
	whole_number sum_result = *this;
	whole_number::add_classic(sum_result, number);

	return sum_result;
}

void whole_number::sub(const whole_number& number)
{
	whole_number::sub_classic(*this, number);
}
whole_number whole_number::difference(const whole_number& number) const
{
	whole_number sub_result = *this;
	whole_number::sub_classic(sub_result, number);

	return sub_result;
}

void whole_number::mul(const whole_number& number)
{
	struct fft
	{
		static uint64_t calculate_expr_value(const std::vector<byte>& a, const std::vector<byte>& b, uint64_t left, uint64_t right)
		{
			uint64_t sum = 0;

			while (left < right)
			{
				const uint32_t pairs_value = a[left] * b[right] + a[right] * b[left];
				sum += pairs_value;

				right--, left++;
			}

			if (left == right)
				sum += static_cast<uint64_t>(a[left]) * static_cast<uint64_t>(b[right]);

			return sum;
		}
	};
	if (this->is_zero() || number.is_zero())
	{
		this->bytes.clear(); // set zero
		return;;
	}

	std::vector<byte> mul_vector_a(this->bytes);
	std::vector<byte> mul_vector_b(number.bytes);

	const int64_t total_multipliers_size = std::max(this->bytes.size(), number.bytes.size());

	mul_vector_a.resize(total_multipliers_size);
	mul_vector_b.resize(total_multipliers_size);

	const uint64_t _left = 0, _right = total_multipliers_size - 1;
	const uint16_t first_expr_value = mul_vector_a[_left] * mul_vector_b[_left];
	const uint16_t last_expr_value = mul_vector_a[_right] * mul_vector_b[_right];

	if(total_multipliers_size == 1)
	{
		std::vector<byte> result = 
		{ 
			static_cast<byte>(first_expr_value), 
			static_cast<byte>(first_expr_value >> 8) 
		};

		if (result[1] == 0)
			result.pop_back();

		this->bytes = result;

		return;
	}

	uint64_t middle_expr_value = fft::calculate_expr_value(mul_vector_a, mul_vector_b, _left, _right);

	std::vector<std::pair<byte, uint64_t>> fft_calculations; fft_calculations.resize(total_multipliers_size * 2 - 1);
	fft_calculations[0] = std::pair<byte, uint64_t> (static_cast<byte>(first_expr_value), first_expr_value >> 8);

	uint64_t current_fft_calc_index = 1; // because first is inserted

	for(uint64_t i = 1; i < _right; i++)
	{
		const uint64_t value = fft::calculate_expr_value(mul_vector_a, mul_vector_b, _left, i) 
				+ fft_calculations[current_fft_calc_index - 1].second;

		fft_calculations[current_fft_calc_index++] = std::pair<byte, uint64_t>(static_cast<byte>(value), value >> 8);
	}

	middle_expr_value += fft_calculations[current_fft_calc_index - 1].second;
	fft_calculations[current_fft_calc_index++] = std::pair<byte, uint64_t>(static_cast<byte>(middle_expr_value), middle_expr_value >> 8);

	for (uint64_t i = 1; i < _right; i++)
	{
		const uint64_t value = fft::calculate_expr_value(mul_vector_a, mul_vector_b, i, _right)
			+ fft_calculations[current_fft_calc_index - 1].second;

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
	std::vector<byte> carry_bytes = whole_number::ulong_to_bytes(last_carry);
	if (!carry_bytes.empty()) 
	{
		result_mul_vector.reserve(result_mul_vector.size() + carry_bytes.size());
		result_mul_vector.insert(result_mul_vector.end(), carry_bytes.begin(), carry_bytes.end());
	}

	whole_number::clear_zero_bytes(result_mul_vector);

	this->bytes = result_mul_vector;
}
whole_number whole_number::product(const whole_number& multiplier) const
{
	whole_number result = *this;
	result.mul(multiplier);

	return result;
}

whole_number whole_number::division(const whole_number& divisor) const
{
	whole_number quotient, remainder;
	whole_number::div(*this, divisor, quotient, remainder);

	return quotient;
}

bool whole_number::is_zero() const
{
	return this->bytes.empty();
}
bool whole_number::is_not_zero() const
{
	return !this->is_zero();
}

bool whole_number::is_one() const
{
	return this->bytes.size() == 1 && bytes[0] == 1;
}
bool whole_number::is_two() const
{
	return this->bytes.size() == 1 && bytes[0] == 2;
}




uint64_t whole_number::num_bits() const
{
	// in one byte eight bits :)
	return this->bytes.size() * 8;
}


#endif