#ifndef FIELD3_HPP_
#define FIELD3_HPP_

#include <libff/algebra/fields/fp.hpp>

namespace gadgetlib
{
	template<mp_size_t n, const libff::bigint<n>& modulus>
	using FieldTy = libff::Fp_model<n, modulus>;
}

#endif
