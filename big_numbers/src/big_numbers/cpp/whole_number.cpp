#ifndef WHOLE_NUMBER_CPP
#define WHOLE_NUMBER_CPP

#include <string> // use std::string
#include <algorithm> // use std::transform
#include <tuple> // std::tuple
#include <vector> // std::vector
#include <random> // use for random

#include "../include/whole_number.h" // use numbers::whole_number

using namespace numbers;

const converters::base_converter numbers::whole_number::hex2dec = converters::base_converter::hex_to_decimal_converter();
const converters::base_converter numbers::whole_number::dec2hex = converters::base_converter::decimal_to_hex_converter();

std::vector<byte> whole_number::uint_to_bytes(const uint32_t number)
{
	if (number == 0)
		return std::vector<byte>();

	constexpr uint32_t int_byte_size = sizeof(int32_t); // is 4
	std::vector<byte> bts(int_byte_size); // reserve 4 bytes_

	for (uint32_t i = 0; i < int_byte_size; i++)
		bts[3 - i] = number >> (i * 8);

	std::reverse(bts.begin(), bts.end());

	while (bts.back() == 0)
		bts.pop_back();

	return bts;
}
std::vector<byte> whole_number::ulong_to_bytes(const uint64_t number)
{
	if (number == 0)
		return std::vector<byte>();

	constexpr uint32_t int64_byte_size = sizeof(int64_t); // is 8
	std::vector<byte> bts(int64_byte_size); // reserve 4 bytes_

	for (uint32_t i = 0; i < int64_byte_size; i++)
		bts[7 - i] = static_cast<byte>(number >> (i * 8)) & 0xFFFFFFFF;

	std::reverse(bts.begin(), bts.end());

	while (bts.back() == 0)
		bts.pop_back();

	return bts;
}

void whole_number::clear_zero_bytes(std::vector<byte>& bytes)
{
	while (!bytes.empty())
	{
		if (bytes.back() != 0)
			break;

		bytes.pop_back();
	}
}
void whole_number::clear_zero_bytes(whole_number& number)
{
	whole_number::clear_zero_bytes(number.bytes_);
}

sbyte whole_number::compare_optimized(const whole_number& a, const whole_number& b)
{
	if (a.bytes_.size() > b.bytes_.size())
		return 1;
	if (a.bytes_.size() < b.bytes_.size())
		return -1;

	if (a.bytes_.empty())
		return 0;

	const byte* a_data = a.bytes_.data();
	const byte* b_data = b.bytes_.data();
	size_t index = a.bytes_.size() - 1;

	for (; index % 16 != 0; index--)
	{
		if (a.bytes_[index] > b.bytes_[index])
			return 1;
		if (a.bytes_[index] < b.bytes_[index])
			return -1;
	}

	if (index == 0)
		return 0;

	for (index -= 16; index != 0; index -= 16)
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

	const int64_t& a_front_part = a_data[0 + 8];
	const int64_t& b_front_part = b_data[0 + 8];

	if (a_front_part > b_front_part)
		return 1;
	if (a_front_part < b_front_part)
		return -1;

	const int64_t& a_back_part = a_data[0];
	const int64_t& b_back_part = b_data[0];

	if (a_back_part > b_back_part)
		return 1;
	if (a_back_part < b_back_part)
		return -1;

	return 0;
}
sbyte whole_number::compare(const whole_number& a, const whole_number& b)
{
	const size_t a_size = a.bytes_.size();
	const size_t b_size = b.bytes_.size();

	if (a_size < b_size)
		return -1;
	if (a_size > b_size)
		return 1;

	if (a_size == 0)
		return 0;

	for (size_t i = a_size - 1; i != 0; i--)
	{
		if (a.bytes_[i] > b.bytes_[i])
			return 1;
		if (a.bytes_[i] < b.bytes_[i])
			return -1;
	}

	if (a.bytes_[0] > b.bytes_[0])
		return 1;
	if (a.bytes_[0] < b.bytes_[0])
		return -1;

	return 0;
}

void whole_number::add_logic_gate(whole_number& destination, const whole_number& source)
{
	whole_number carry_value = source;  // NOLINT(performance-unnecessary-copy-initialization)

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
		carry.shl(1);

		carry_value = carry;
	}
}
void whole_number::add_classic(whole_number& destination, const whole_number& source)
{
	if (destination.bytes_.size() < source.bytes_.size())
		destination.bytes_.resize(source.bytes_.size());

	byte carry = 0;

	auto destination_data = &destination.bytes_.front();
	auto source_data = &source.bytes_.front();

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

void whole_number::sub_logic_gate(whole_number& destination, const whole_number& source)
{
	if (destination.bytes_.size() < source.bytes_.size())
		throw std::invalid_argument("source");

	whole_number borrow_value = source;

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
		borrow.shl(1);

		borrow_value = borrow;
	}

	whole_number::clear_zero_bytes(destination);
}
void whole_number::sub_classic(whole_number& destination, const whole_number& source)
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

	whole_number::clear_zero_bytes(destination);
}

void whole_number::div(const whole_number& dividend, const whole_number& divisor, whole_number& quotient, whole_number& remainder)
{
	if (divisor.is_zero())
		throw std::invalid_argument("divisor is zero");

	const whole_number _dividend = dividend, _divisor = divisor;
	const size_t k = _dividend.num_bits() + _divisor.num_bits();

	whole_number pow2 = whole_number::one();
	pow2 <<= k + 1;

	whole_number x = _dividend - _divisor;
	whole_number last_x, last_last_x;

	while (true)
	{
		x = x * (pow2 - x * _divisor);
		x >>= k;

		if (whole_number::compare(x, last_x) == 0 ||
			whole_number::compare(x, last_last_x) == 0) break;

		last_last_x = last_x;
		last_x = x;
	}

	whole_number _quotient = _dividend * x;
	_quotient >>= k;

	if (whole_number::compare(_quotient * _divisor,_dividend) == -1)
		++_quotient;

	if (whole_number::compare(_quotient * _divisor, _dividend) == 1) 
		--_quotient, remainder = _dividend - (_quotient * _divisor);
	else
		remainder.set_zero();

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

	for (size_t i = (hex_string.length() >> 1) - 1; i != 0; i--)
	{
		const byte b1 = static_cast<byte>(hex_map.rfind(hex_string[i * 2])) << 4;
		const byte b2 = static_cast<byte>(hex_map.rfind(hex_string[i * 2 + 1]));
		bytes_.push_back(b1 + b2);
	}

	const byte b1 = static_cast<byte>(hex_map.rfind(hex_string[0 * 2])) << 4;
	const byte b2 = static_cast<byte>(hex_map.rfind(hex_string[0 * 2 + 1]));
	bytes_.push_back(b1 + b2);
}
whole_number::whole_number(std::vector<byte> bytes) : bytes_(std::move(bytes)) { }
whole_number::whole_number(std::initializer_list<byte> bytes) : whole_number()
{
	std::vector<byte> bts = bytes;
	whole_number::clear_zero_bytes(bts);
	this->bytes_ = bts;
}

whole_number::whole_number(const uint32_t number) : whole_number()
{
	if (number == 0)
		return;

	this->bytes_ = whole_number::uint_to_bytes(number);
}
whole_number::whole_number(const uint64_t number) : whole_number()
{
	if (number == 0)
		return;

	this->bytes_ = whole_number::ulong_to_bytes(number);
}
whole_number::~whole_number()
{
	this->bytes_.~vector();
}

whole_number whole_number::zero()
{
	return std::vector<byte>();
}
whole_number whole_number::one()
{
	whole_number one = { 1 };
	return one;
}
whole_number whole_number::two()
{
	whole_number one = { 2 };
	return one;
}

std::string whole_number::to_string() const
{
	const std::string hex_str_view = this->to_string_hex();
	return hex2dec.convert(hex_str_view);
}
std::string whole_number::to_string_hex() const
{
	if (this->bytes_.empty())
		return "00";

	const std::string hex_map = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

	std::string s(bytes_.size() * 2, ' ');
	for (size_t i = 0; i < bytes_.size(); ++i) {
		s[2 * i] = hex_map[(bytes_[i] & 0xF0) >> 4];
		s[2 * i + 1] = hex_map[bytes_[i] & 0x0F];
	}

	std::string result;
	for (size_t i = static_cast<int64_t>(s.length()) - 2; i != 0; i -= 2)
	{
		result.push_back(s[i]);
		result.push_back(s[i + 1]);
	}

	result.push_back(s[0]);
	result.push_back(s[0 + 1]);

	return result;
}
uint64_t numbers::whole_number::to_uint64_t() const
{
	if (this->bytes_.size() > 8)
		throw std::exception("is very big for uint64_t");
	if (bytes_.empty())
		return 0u;

	std::vector<byte> bts = this->bytes_; bts.resize(8);
	uint64_t* dt = reinterpret_cast<uint64_t*>(bts.data());
	return *dt;
}
std::vector<byte> numbers::whole_number::to_bytes() const
{
	return this->bytes_;
}

whole_number whole_number::and (const whole_number& number) const
{
	const size_t result_size = std::min(this->bytes_.size(), number.bytes_.size());
	std::vector<byte> result_bytes(result_size);

	for (size_t i = 0; i < result_size; i++)
		result_bytes[i] = number.bytes_[i] & this->bytes_[i];

	whole_number::clear_zero_bytes(result_bytes);

	return result_bytes;
}
whole_number whole_number::or (const whole_number& number) const
{
	const size_t calculated_part_size = std::min(this->bytes_.size(), number.bytes_.size());
	const size_t filled_part_size = std::max(this->bytes_.size(), number.bytes_.size()) - calculated_part_size;
	auto& greater_number_bytes = this->bytes_.size() > number.bytes_.size() ? this->bytes_ : number.bytes_;
	std::vector<byte> result_bytes(calculated_part_size + filled_part_size);

	for (size_t i = 0; i < calculated_part_size; i++)
		result_bytes[i] = number.bytes_[i] | this->bytes_[i];

	for (size_t i = calculated_part_size; i < filled_part_size + calculated_part_size; i++)
		result_bytes[i] = greater_number_bytes[i];

	whole_number::clear_zero_bytes(result_bytes);

	return result_bytes;
}
whole_number whole_number::xor (const whole_number& number) const
{
	const size_t calculated_part_size = std::min(this->bytes_.size(), number.bytes_.size());
	const size_t filled_part_size = std::max(this->bytes_.size(), number.bytes_.size()) - calculated_part_size;
	auto& greater_number_bytes = this->bytes_.size() > number.bytes_.size() ? this->bytes_ : number.bytes_;
	std::vector<byte> result_bytes(calculated_part_size + filled_part_size);

	for (size_t i = 0; i < calculated_part_size; i++)
		result_bytes[i] = number.bytes_[i] ^ this->bytes_[i];

	for (size_t i = calculated_part_size; i < filled_part_size + calculated_part_size; i++)
		result_bytes[i] = greater_number_bytes[i];

	whole_number::clear_zero_bytes(result_bytes);

	return result_bytes;
}
whole_number whole_number::not() const
{
	std::vector<byte> result_bytes(this->bytes_.size());

	for (size_t i = 0; i < this->bytes_.size(); i++)
		result_bytes[i] = ~this->bytes_[i];

	return result_bytes;
}

void whole_number::shr(const size_t shift_count)
{
	if (this->is_zero())
		return;

	byte* data = nullptr;
	size_t i = 0;
	auto counter = shift_count;

	// for (auto [i, f, s] = std::tuple{...};;)
	while (data = &this->bytes_.front(), i = this->bytes_.size(), counter-- != 0)
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

	whole_number::clear_zero_bytes(this->bytes_);
}
void whole_number::shl(const size_t shift_count)
{
	if (this->is_zero())
		return;

	const byte eight = 0x8; // 8 bits in 1 byte
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

	whole_number::clear_zero_bytes(this->bytes_);
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
				sum += static_cast<uint64_t>(a[loc_left]) * static_cast<uint64_t>(b[loc_right]);

			return sum;
		}
	};

	if (this->is_zero() || number.is_zero())
	{
		this->bytes_.clear(); // set zero
		return;;
	}

	std::vector<byte> mul_vector_a(this->bytes_);
	std::vector<byte> mul_vector_b(number.bytes_);

	const size_t total_multipliers_size = std::max(this->bytes_.size(), number.bytes_.size());

	mul_vector_a.resize(total_multipliers_size);
	mul_vector_b.resize(total_multipliers_size);

	const size_t _left = 0, _right = total_multipliers_size - 1;
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

		this->bytes_ = result;

		return;
	}

	uint64_t middle_expr_value = fft::calculate_expr_value(mul_vector_a, mul_vector_b, _left, _right);

	std::vector<std::pair<byte, uint64_t>> fft_calculations; fft_calculations.resize(total_multipliers_size * 2 - 1);
	fft_calculations[0] = std::pair<byte, uint64_t> (static_cast<byte>(first_expr_value), first_expr_value >> 8);

	size_t current_fft_calc_index = 1; // because first is inserted

	for(size_t i = 1; i < _right; i++)
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
	std::vector<byte> carry_bytes = whole_number::ulong_to_bytes(last_carry);
	if (!carry_bytes.empty()) 
	{
		result_mul_vector.reserve(result_mul_vector.size() + carry_bytes.size());
		result_mul_vector.insert(result_mul_vector.end(), carry_bytes.begin(), carry_bytes.end());
	}

	whole_number::clear_zero_bytes(result_mul_vector);

	this->bytes_ = result_mul_vector;
}
void numbers::whole_number::fast_mul(const whole_number& number)
{
	struct fft_fast
	{
		__declspec(noinline) static uint64_t calculate_expr_value_fast(const std::vector<byte>& greater, const std::vector<byte>& lower,
			size_t left, size_t right)
		{
			if (lower.size() <= left)
				return 0;

			uint64_t sum = 0u;
			size_t tmp_left = left, tmp_right = right;
			size_t free_pairs_count = lower.size() == 1 ?
				2 : lower.size() > right ? (1ull + (lower.size() == right)) : lower.size() == right ? 2
				: (lower.size() - left + (lower.size() >= right ? -static_cast<int64_t>(lower.size() - right) : 1));

			while(--free_pairs_count != 0)
			{
				const uint32_t pair_value = greater[tmp_right] * lower[tmp_left];
				sum += pair_value;

				tmp_right--; tmp_left++;
			}

			if (tmp_left >= lower.size())
				return sum;
			
			while(tmp_left < tmp_right)
			{
				const uint32_t pair_value = greater[tmp_right] * lower[tmp_left] + greater[tmp_left] * lower[tmp_right];;
				sum += pair_value;

				tmp_right--; tmp_left++;
			}
			
			if (tmp_left == tmp_right && tmp_left < lower.size())
				sum += static_cast<uint64_t>(greater[tmp_right]) * static_cast<uint64_t>(lower[tmp_left]);

			return sum;
		}
	};

	if (this->is_zero() || number.is_zero())
	{
		this->bytes_.clear(); // set zero
		return;;
	}

	std::vector<byte> mul_vector_a(this->bytes_);
	std::vector<byte> mul_vector_b(number.bytes_);

	if (mul_vector_a.size() < mul_vector_b.size())
		mul_vector_a.swap(mul_vector_b);

	const size_t total_multipliers_size = std::max(this->bytes_.size(), number.bytes_.size());

	const size_t _left = 0, _right = total_multipliers_size - 1;
	const uint16_t first_expr_value = mul_vector_a[_left] * mul_vector_b[_left];

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

	std::vector<std::pair<byte, uint64_t>> fft_calculations; fft_calculations.resize(total_multipliers_size * 2 - 1);
	fft_calculations[0] = std::pair<byte, uint64_t>(static_cast<byte>(first_expr_value), first_expr_value >> 8);

	size_t current_fft_calc_index = 1; // because first is inserted

	for (size_t i = 1; i <= _right; i++)
	{
		uint64_t value = fft_fast::calculate_expr_value_fast(mul_vector_a, mul_vector_b, _left, i);
		value += fft_calculations[current_fft_calc_index - 1].second;

		fft_calculations[current_fft_calc_index++] = std::pair<byte, uint64_t>(static_cast<byte>(value), value >> 8);
	}

	for (size_t i = 1; i <= _right; i++)
	{
		uint64_t value = fft_fast::calculate_expr_value_fast(mul_vector_a, mul_vector_b, i, _right);
		value += fft_calculations[current_fft_calc_index - 1].second;

		fft_calculations[current_fft_calc_index++] = std::pair<byte, uint64_t>(static_cast<byte>(value), value >> 8);
	}

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

	this->bytes_ = result_mul_vector;
}
whole_number whole_number::product(const whole_number& multiplier) const
{
	whole_number result = *this;
	// result.fast_mul(multiplier);
	result.fast_mul(multiplier);

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
	return this->bytes_.empty();
}
bool whole_number::is_not_zero() const
{
	return !this->is_zero();
}

bool whole_number::is_one() const
{
	return this->bytes_.size() == 1 && bytes_.front() == 1;
}
bool whole_number::is_two() const
{
	return this->bytes_.size() == 1 && bytes_.front() == 2;
}
bool whole_number::is_power_of_two() const
{
	if (this->is_zero())
		return false;

	if (this->is_one())
		return true;

	whole_number this_minus_one = *this; --this_minus_one;
	return (*this & this_minus_one).is_zero();
}

size_t whole_number::num_bits() const
{
	// in one byte eight bits :)
	return this->bytes_.size() << 3; // * 8
}
bool whole_number::is_odd() const
{
	return !this->bytes_.empty() && this->bytes_.front() & 1;
}
bool whole_number::is_even() const
{
	return !this->is_odd();
}

void whole_number::set_zero()
{
	this->bytes_.clear();
}

whole_number& whole_number::operator++()
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
		if(++byte_iterator == end)
			break;
	}
	
	if (byte_iterator == end) // возможно сравнение можно заменить, но это не точно
		this->bytes_.push_back(1);

	return *this;
}
whole_number& whole_number::operator--()
{
	if (this->bytes_.empty())
		throw std::invalid_argument("number can not be less then zero");

	auto bytes_iterator = this->bytes_.begin();

	if(--*bytes_iterator == 0xFF)
	{
		while (*++bytes_iterator == 0)
			*bytes_iterator = 0xFF;

		--*bytes_iterator;
	}

	if (this->bytes_.back() == 0)
		this->bytes_.pop_back();

	return *this;
}

whole_number& whole_number::operator++(int)
{
	return ++*this;
}
whole_number& whole_number::operator--(int)
{
	return --*this;
}

whole_number whole_number::operator + (const whole_number& number) const
{
	return this->sum(number);
}
whole_number whole_number::operator - (const whole_number& number) const
{
	return this->difference(number);
}
whole_number whole_number::operator * (const whole_number& number) const
{
	return this->product(number);
}
whole_number whole_number::operator / (const whole_number& number) const
{
	return this->division(number);
}
whole_number whole_number::operator % (const whole_number& number) const
{
	whole_number q, r;
	whole_number::div(*this, number, q, r);

	return r;
}

whole_number whole_number::operator << (const size_t shift_count) const
{
	whole_number copy = *this;
	copy.shl(shift_count);

	return copy;
}
void whole_number::operator <<= (const size_t shift_count)
{
	this->shl(shift_count);
}
whole_number whole_number::operator >> (const size_t shift_count) const
{
	whole_number copy = *this;
	copy.shr(shift_count);

	return copy;
}
void whole_number::operator >>= (const size_t shift_count)
{
	this->shr(shift_count);
}

whole_number whole_number::operator & (const whole_number& number) const
{
	return this-> and (number);
}
whole_number whole_number::operator ^ (const whole_number& number) const
{
	return this-> xor (number);
}
whole_number whole_number::operator | (const whole_number& number) const
{
	return this-> or (number);
}

whole_number whole_number::pow(const whole_number& exponent) const
{
	if (exponent.is_zero())
		return whole_number::one();
	if (exponent.is_one())
		return *this;

	whole_number power = exponent;
	whole_number result = whole_number::one();
	whole_number x = *this;

	while( power.is_not_zero() )
	{
		if( power.is_even() )
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
whole_number whole_number::factorial() const
{
	struct factorial
	{
		static whole_number prod_tree(const whole_number& l, const whole_number& r)
		{
			const sbyte compare_result = whole_number::compare(l, r);
			if (compare_result == 1)
				return whole_number::one();
			if (compare_result == 0)
				return l;
			if ((r - l).is_one())
				return r * l;

			whole_number m = l; m.add(r); m.shr(1);
			whole_number m_plus_one = m; ++m_plus_one;

			return factorial::prod_tree(l, m) * factorial::prod_tree(m_plus_one, r);
		}
	};

	if (this->is_zero())
		return whole_number::one();

	if (this->is_one() || this->is_two())
		return *this;

	return factorial::prod_tree(whole_number::two(), *this);
}
whole_number whole_number::sqrt() const
{
	whole_number x0 = *this;
	whole_number x1 = *this; ++x1; x1.shr(1);
	
	while(whole_number::compare(x1, x0) == -1)
	{
		x0 = x1;
		x1 = x1 + (*this / x1); x1.shr(1);
	}

	return x0;
	
}
whole_number whole_number::log_n(uint64_t n) const
{
	uint64_t log = 0u; 
	whole_number tmp_this = *this;

	while( !tmp_this.is_one() && tmp_this.is_not_zero() )
	{
		++log;
		tmp_this = tmp_this / n;
	}

	return log;
}
whole_number whole_number::log2() const
{
	uint64_t log = 0u;
	whole_number tmp_this = *this;

	while (!tmp_this.is_one() && tmp_this.is_not_zero())
	{
		++log;
		tmp_this >>= 1;
	}

	return log;
}

whole_number::extensions::extensions() = default;

whole_number whole_number::extensions::max(const whole_number& a, const whole_number& b)
{
	const sbyte compare_result = whole_number::compare(a, b);

	if (compare_result == 1)
		return a;
		
	return b;
}
whole_number whole_number::extensions::min(const whole_number& a, const whole_number& b)
{
	const sbyte compare_result = whole_number::compare(a, b);

	if (compare_result == -1)
		return a;

	return b;
}

whole_number whole_number::extensions::gcd(const whole_number& a, const whole_number& b)
{
	whole_number tmp_a = a, tmp_b = b;

	while( tmp_b.is_not_zero() )
	{
		whole_number rem, q;
		whole_number::div(tmp_a, tmp_b, q, rem);

		tmp_a = tmp_b;
		tmp_b = rem;
	}

	return tmp_a;
}
whole_number whole_number::extensions::lcm(const whole_number& a, const whole_number& b)
{
	return (a * b / whole_number::extensions::gcd(a, b));
}

whole_number whole_number::extensions::random(uint32_t max_byte_count)
{
	if (max_byte_count == 0)
		return whole_number::zero();

	std::mt19937 rnd(std::random_device().operator()());
	const std::uniform_int_distribution<std::mt19937::result_type> rand_size(0, max_byte_count);

	const uint32_t size = rand_size(rnd);
	if (size == 0) return whole_number::zero();

	std::vector<byte> random_bytes(size);
	const std::uniform_int_distribution<std::mt19937::result_type> rand_byte(0, 255);

	for (size_t i = 0; i < size; i++)
		random_bytes[i] = static_cast<byte>(rand_byte(rnd));

	whole_number::clear_zero_bytes(random_bytes);

	return random_bytes;
}

#endif