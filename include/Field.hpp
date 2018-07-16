#ifndef FIELD_HPP_
#define FIELD_HPP_

#pragma warning (disable : 4146)
#include <NTL/ZZ_pEX.h>

#include <variant>
#include <iostream>
#include <string>
#include <sstream>

namespace gadgetlib
{
	
	template <uint64_t x>
	struct SignificantBits {
		static const uint64_t n = SignificantBits<x /2>::n + 1;
	};

	template <>
	struct SignificantBits<0>
	{
		static const uint64_t n = 0;
	};

	template<uint64_t characteristics>
	class Field
	{
	public:
		NTL::ZZ_p num_;
		static bool initialized_;

	private:
		void initialize()
		{
			if (!initialized_)
			{
				initialized_ = true;
				std::string str_rep = std::to_string(characteristics);
				NTL::ZZ chp = NTL::conv<NTL::ZZ>(str_rep.data());
				NTL::ZZ_p::init(chp);
			}
		}

		NTL::ZZ hexToZZ(const std::string& hexVal)
		{
			bool decimal = (hexVal[0] == 'd');
			
			auto convert_ch = [](char c) -> int
			{
				if (c >= '0' && c <= '9')
					return (c - '0');
				if (c >= 'a' && c <= 'f')
					return (c - 'a' + 10);

			};
			
			NTL::ZZ val;
			val = NTL::to_ZZ(0);	//initialise the value to zero
			
			for (unsigned i = (decimal ? 1: 0); i < hexVal.length(); i++)
			{
				val *= (decimal ? 10 : 16);
				val += convert_ch(hexVal[i]);
				
			}
			return val;
		}
		
	public:
		static constexpr uint64_t safe_bitsize = SignificantBits<characteristics>::n;
		static constexpr uint64_t chr = characteristics;

		Field(size_t num)
		{
			initialize();
			NTL::ZZ temp = NTL::conv<NTL::ZZ>(num);
			num_ = NTL::conv<NTL::ZZ_p>(temp);
		}

		Field()
		{
			initialize();
		}

		Field(const NTL::ZZ_p& num)
		{
			initialize();
			num_ = num;
		}

		Field(const std::variant<uint32_t, std::string>& v)
		{
			initialize();
			//uint32_t n;
			switch (v.index())
			{
			case 0:
			{
				NTL::ZZ num = NTL::conv<NTL::ZZ>(std::get<0>(v));
				num_ = NTL::conv<NTL::ZZ_p>(num);;
				break;
			}
			case 1:
				NTL::ZZ int_num = hexToZZ(std::get<1>(v));
				num_ = NTL::conv<NTL::ZZ_p>(int_num);
				break;
			};
		}

		Field& operator+=(const Field& rhs)
		{
			this->num_ += rhs.num_;
			return *this;
		}


		Field& operator-=(const Field& rhs)
		{
			this->num_ -= rhs.num_;
			return *this;
		}

		Field& operator*=(const Field& rhs)
		{
			this->num_ *= rhs.num_;
			return *this;
		}

		Field& operator-()
		{
			this->num_ = -this->num_;
			return *this;
		}


		Field& operator/=(const Field& rhs)
		{
			this->num_ = NTL::conv<NTL::ZZ_p>(NTL::rep(num_) / NTL::rep(rhs.num_));
			return *this;
		}

		static Field one()
		{
			return 1;
		}

		operator bool() const
		{
			return num_ != 0;
		}

		std::string to_string() const
		{
			std::stringstream buffer;
			buffer << num_;
			return buffer.str();
		}
	};

	template<uint64_t characteristics>
	Field<characteristics> operator+(const Field<characteristics>& left,
		const Field<characteristics>& right)
	{
		return Field<characteristics>(left.num_ + right.num_);
	}

	template<uint64_t characteristics>
	Field<characteristics> operator-(const Field<characteristics>& left,
		const Field<characteristics>& right)
	{
		return Field<characteristics>(left.num_ - right.num_);
	}

	template<uint64_t characteristics>
	Field<characteristics> operator*(const Field<characteristics>& left,
		const Field<characteristics>& right)
	{
		return Field<characteristics>(left.num_ * right.num_);
	}

	template<uint64_t characteristics>
	bool operator==(const Field<characteristics>& left,
		const Field<characteristics>& right)
	{
		return (left.num_ == right.num_);
	}

	template<uint64_t characteristics>
	bool operator!=(const Field<characteristics>& left,
		const Field<characteristics>& right)
	{
		return (left.num_ != right.num_);
	}

	template<uint64_t characteristics>
	Field<characteristics> operator%(const Field<characteristics>& left,
		int right)
	{
		assert(right == 2);
		return NTL::rem(rep(left.num_), right);
	}

	template<uint64_t characteristics>
	std::ostream& operator<< (std::ostream& stream, const Field<characteristics>& elem)
	{
		stream << elem.num_;
		return stream;
	}

	template<uint64_t characteristics>
	bool Field<characteristics>::initialized_ = false;
}

#endif
