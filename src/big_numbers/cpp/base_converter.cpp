#ifndef BASE_CONVERTER_CPP
#define BASE_CONVERTER_CPP

#include <stdexcept>

namespace converters
{
	class base_converter
	{
	public:
		std::string get_source_base_set() const { return source_base_set_; }
		std::string get_target_base_set() const { return target_base_set_; }
		unsigned int get_source_base() const { return static_cast<unsigned int>(source_base_set_.length()); }
		unsigned int get_target_base() const { return static_cast<unsigned int>(target_base_set_.length()); }

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="source_base_set">Characters used for source base</param>
		/// <param name="target_base_set">Characters used for target base</param>
		base_converter(const std::string& source_base_set, const std::string& target_base_set)
			: source_base_set_(source_base_set)
			, target_base_set_(target_base_set)
		{
			if (source_base_set.empty() || target_base_set.empty())
				throw std::invalid_argument("Invalid base character set");
		}

		/// <summary>
		/// Get a base converter for decimal to binary numbers
		/// </summary>
		static const base_converter& decimal_to_binary_converter()
		{
			static const base_converter dec2bin(decimal_set_, binary_set_);
			return dec2bin;
		}

		/// <summary>
		/// Get a base converter for binary to decimal numbers
		/// </summary>
		static const base_converter& binary_to_decimal_converter()
		{
			static const base_converter bin2dec(binary_set_, decimal_set_);
			return bin2dec;
		}

		/// <summary>
		/// Get a base converter for decimal to binary numbers
		/// </summary>
		static const base_converter& decimal_to_hex_converter()
		{
			static const base_converter dec2hex(decimal_set_, hex_set_);
			return dec2hex;
		}

		/// <summary>
		/// Get a base converter for binary to decimal numbers
		/// </summary>
		static const base_converter& hex_to_decimal_converter()
		{
			static const base_converter hex2dec(hex_set_, decimal_set_);
			return hex2dec;
		}

		/// <summary>
		/// Convert a value in the source number base to the target number base.
		/// </summary>
		/// <param name="value">Value in source number base.</param>
		/// <returns>Value in target number base.</returns>
		std::string convert(std::string value) const
		{
			const auto number_base = get_target_base();
			std::string result;

			do
			{
				const auto remainder = divide(source_base_set_, value, number_base);
				result.push_back(target_base_set_[remainder]);
			} while (!value.empty() && !(value.length() == 1 && value[0] == source_base_set_[0]));

			std::reverse(result.begin(), result.end());
			return result;
		}


		/// <summary>
		/// Convert a value in the source number base to the target number base.
		/// </summary>
		/// <param name="value">Value in source number base.</param>
		/// <param name="minDigits">Minimum number of digits for returned value.</param>
		/// <returns>Value in target number base.</returns>
		std::string convert(const std::string& value, size_t minDigits) const
		{
			auto result = convert(value);
			if (result.length() < minDigits)
				return std::string(minDigits - result.length(), target_base_set_[0]) + result;
			
			return result;
		}

		/// <summary>
		/// Convert a decimal value to the target base.
		/// </summary>
		/// <param name="value">Decimal value.</param>
		/// <returns>Result in target base.</returns>
		std::string from_decimal(const unsigned int value) const
		{
			return dec2base(target_base_set_, value);
		}

		/// <summary>
		/// Convert a decimal value to the target base.
		/// </summary>
		/// <param name="value">Decimal value.</param>
		/// <param name="minDigits">Minimum number of digits for returned value.</param>
		/// <returns>Result in target base.</returns>
		std::string from_decimal(const unsigned int value, const size_t minDigits) const
		{
			std::string result = from_decimal(value);
			if (result.length() < minDigits)
				return std::string(minDigits - result.length(), target_base_set_[0]) + result;
			
			return result;
		}

		/// <summary>
		/// Convert value in source base to decimal.
		/// </summary>
		/// <param name="value">Value in source base.</param>
		/// <returns>Decimal value.</returns>
		unsigned to_decimal(const std::string value) const {
			return base2dec(source_base_set_, value);
		}

	private:
		/// <summary>
		/// Divides x by y, and returns the quotient and remainder.
		/// </summary>
		/// <param name="base_digits">Base digits for x and quotient.</param>
		/// <param name="x">Numerator expressed in base digits; contains quotient, expressed in base digits, upon return.</param>
		/// <param name="y">Denominator</param>
		/// <returns>Remainder of x / y.</returns>
		static unsigned int divide(const std::string& base_digits,
			std::string& x,
			const unsigned int y)
		{
			std::string quotient;

			const size_t length = x.length();
			for (size_t i = 0; i < length; ++i)
			{
				size_t j = i + 1 + x.length() - length;
				if (x.length() < j)
					break;

				const unsigned int value = base2dec(base_digits, x.substr(0, j));

				quotient.push_back(base_digits[value / y]);
				x = dec2base(base_digits, value % y) + x.substr(j);
			}

			// calculate remainder
			const unsigned int remainder = base2dec(base_digits, x);

			// remove leading "zeros" from quotient and store in 'x'
			const size_t n = quotient.find_first_not_of(base_digits[0]);
			
			if (n != std::string::npos)
				x = quotient.substr(n);
			else
				x.clear();

			return remainder;
		}

		static unsigned int base2dec(const std::string& baseDigits,
			const std::string& value)
		{
			const auto number_base = static_cast<unsigned int>(baseDigits.length());
			unsigned int result = 0;
			for (char i : value)
			{
				result *= number_base;
				const size_t c = baseDigits.find(i);
				if (c == std::string::npos)
					throw std::runtime_error("Invalid character");

				result += static_cast<unsigned int>(c);
			}

			return result;
		}

		static std::string dec2base(const std::string& base_digits, unsigned int value)
		{
			const unsigned int number_base = static_cast<unsigned int>(base_digits.length());
			std::string result;
			do
			{
				result.push_back(base_digits[value % number_base]);
				value /= number_base;
			} while (value > 0);

			std::reverse(result.begin(), result.end());
			return result;
		}

	private:
		static const char* binary_set_;
		static const char* decimal_set_;
		static const char* hex_set_;
		std::string        source_base_set_;
		std::string        target_base_set_;
	};

	const char* converters::base_converter::binary_set_ = "01";
	const char* converters::base_converter::decimal_set_ = "0123456789";
	const char* converters::base_converter::hex_set_ = "0123456789abcdef";

}

#endif // !1
