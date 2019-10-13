#pragma once

#include <string>
#include <sstream>
#include "big-int/big-int.h"
#include <list>

namespace converters
{
	// big base converter
	class bbc
	{
	public:
		constexpr bbc() = default;
		[[nodiscard]] virtual std::string convert(const std::string& number) const = 0;
		virtual ~bbc() = default;
	};

	// hex to decimal number converter
	class hex2dec_bbc : public bbc
	{
	public:
		constexpr hex2dec_bbc() = default;

		[[nodiscard]] std::string convert(const std::string& hex_number) const override
		{
			for (char symbol : hex_number)
				if (hex_letters.find(symbol) == std::string::npos)
					throw std::exception();

			unsigned_big_integer result;
			const unsigned_big_integer sixteen = 16u;
			
			for(size_t i = 0; i < hex_number.size(); i++)
			{
				const size_t index = hex2dec_bbc::hex_letters.find(hex_number[i]);
				result = result.sum(sixteen.pow(hex_number.size() - i - 1).mul(index));
			}

			return result.to_string();
		}
	private:
		inline static std::string hex_letters = "0123456789ABCDEF";
	};

	// decimal to hex number converter
	class dec2hex_bbc : public bbc
	{
	public:
		constexpr dec2hex_bbc() = default;

		[[nodiscard]] std::string convert(const std::string& dec_number) const override
		{
			unsigned_big_integer n = dec_number;
			std::list<uint8_t> reminders;
			std::stringstream hex_view;
			
			uint16_t rem = 0;
			size_t list_size = 0;
			
			while(n.is_not_zero())
			{
				n = n.div16_rem(rem);

				reminders.push_back(rem);

				list_size++;
			}

			if (list_size % 2 == 1)
				reminders.push_back(0);
			
			while(!reminders.empty())
			{
				hex_view << std::uppercase << std::hex << static_cast<uint16_t>(reminders.back());
				reminders.pop_back();
			}
			
			return hex_view.str();
		}
	};
}