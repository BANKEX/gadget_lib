#ifndef FIELD2_HPP_
#define FIELD2_HPP_

#pragma warning (disable : 4146)
#include <NTL/ZZ_pEX.h>

#include <variant>
#include <iostream>
#include <string>

namespace gadgetlib
{
	// helper function
	constexpr unsigned c_strlen(char const* str, unsigned count = 0)
	{
		return ('\0' == str[0]) ? count : c_strlen(str + 1, count + 1);
	}

	// destination "template string" type
	template < char... tt_c >
	struct exploded_string
	{
	};

	// struct to explode a `char const*` to an `exploded_string` type
	template < typename T_StrProvider, unsigned t_len, char... tt_c >
	struct explode_impl
	{
		using result =
			typename explode_impl < T_StrProvider, t_len - 1,
			T_StrProvider::str()[t_len - 1],
			tt_c... > ::result;
	};

	template < typename T_StrProvider, char... tt_c >
	struct explode_impl < T_StrProvider, 0, tt_c... >
	{
		using result = exploded_string<tt_c...>;
	};

	// syntactical sugar
	template < typename T_StrProvider >
	using explode =
		typename explode_impl < T_StrProvider,
		c_strlen(T_StrProvider::str()) > ::result;


	template<typename STR_PROVIDER>
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
				NTL::ZZ chp = NTL::conv<NTL::ZZ>(STR_PROVIDER::str());
				NTL::ZZ_p::init(chp);
			}

		}

		NTL::ZZ hexToZZ(const std::string& hexVal)
		{
			auto convert_ch = [](char c) -> int
			{
				if (c >= '0' && c <= '9')
					return (c - '0');
				if (c >= 'a' && c <= 'f')
					return (c - 'a' + 10);

			};

			NTL::ZZ val;
			val = NTL::to_ZZ(0);	//initialise the value to zero

			for (unsigned i = 0; i < hexVal.length(); i++)
			{
				val *= 16;
				val += convert_ch(hexVal[i]);

			}
			return val;
		}

	public:
		//static constexpr uint64_t safe_bitsize = SignificantBits<characteristics>::n;
		//static constexpr uint64_t chr = characteristics;

		Field(size_t num)
		{
			initialize();
			num_ = num;
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
	};

	template<typename STR_PROVIDER>
	Field<STR_PROVIDER> operator+(const Field<STR_PROVIDER>& left,
		const Field<STR_PROVIDER>& right)
	{
		return Field<STR_PROVIDER>(left.num_ + right.num_);
	}

	template<typename STR_PROVIDER>
	Field<STR_PROVIDER> operator-(const Field<STR_PROVIDER>& left,
		const Field<STR_PROVIDER>& right)
	{
		return Field<STR_PROVIDER>(left.num_ - right.num_);
	}

	template<typename STR_PROVIDER>
	Field<STR_PROVIDER> operator*(const Field<STR_PROVIDER>& left,
		const Field<STR_PROVIDER>& right)
	{
		return Field<STR_PROVIDER>(left.num_ * right.num_);
	}

	template<typename STR_PROVIDER>
	bool operator==(const Field<STR_PROVIDER>& left,
		const Field<STR_PROVIDER>& right)
	{
		return (left.num_ == right.num_);
	}

	template<typename STR_PROVIDER>
	bool operator!=(const Field<STR_PROVIDER>& left,
		const Field<STR_PROVIDER>& right)
	{
		return (left.num_ != right.num_);
	}

	template<typename STR_PROVIDER>
	Field<STR_PROVIDER> operator%(const Field<STR_PROVIDER>& left,
		int right)
	{
		assert(right == 2);
		return NTL::rem(rep(left.num_), right);
	}

	template<typename STR_PROVIDER>
	std::ostream& operator<< (std::ostream& stream, const Field<STR_PROVIDER>& elem)
	{
		stream << elem.num_;
		return stream;
	}

	template<typename STR_PROVIDER>
	bool Field<STR_PROVIDER>::initialized_ = false;
}

#endif
