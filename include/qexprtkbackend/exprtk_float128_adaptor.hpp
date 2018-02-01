/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Custom Real type Adaptor                                   *
 * Authors: Arash Partow                                      *
 * URL: http://www.partow.net/programming/exprtk/index.html   *
 *                                                            *
 * Copyright notice:                                          *
 * Free use of the Mathematical Expression Toolkit Library is *
 * permitted under the guidelines and in accordance with the  *
 * most current version of the Common Public License.         *
 * http://www.opensource.org/licenses/cpl1.0.php              *
 *                                                            *
 **************************************************************
*/


#ifndef EXPRTK_FLOAT128_ADAPTOR_HPP
#define EXPRTK_FLOAT128_ADAPTOR_HPP

#include <string>
//#include "real_type.hpp"


namespace exprtk
{
namespace details
{
namespace numeric
{
namespace details
{
struct float128_real_type_tag;
}
}

inline bool is_true(const _Float128 &v);
inline bool is_false(const _Float128 v);

template <typename Iterator>
inline bool string_to_real(Iterator &itr_external, const Iterator end,
                           _Float128 &t, details::numeric::details::float128_real_type_tag);
}

namespace helper
{
namespace details
{
inline void print_type(const std::string &fmt, const _Float128 &v,
                       exprtk::details::numeric::details::float128_real_type_tag);
}
}

using details::is_true;
}

#include "exprtk.hpp"

namespace exprtk
{
namespace details
{
namespace numeric
{
namespace details
{
struct float128_real_type_tag {};

template<> struct number_type<_Float128> {
	typedef float128_real_type_tag type;
};

template <>
struct epsilon_type<float128_real_type_tag> {
	static inline _Float128 value()
	{
		const _Float128 epsilon = _Float128(0.000000000001);
		return epsilon;
	}
};

inline bool is_nan_impl(const _Float128 &v, float128_real_type_tag)
{
	return std::isnan((double)v);
}

template <typename T> inline T   abs_impl(const T v, float128_real_type_tag)
{
	return abs(v);
}
template <typename T> inline T  acos_impl(const T v, float128_real_type_tag)
{
	return acos(v);
}
template <typename T> inline T acosh_impl(const T v, float128_real_type_tag)
{
	return log(v + sqrt((v * v) - _Float128(1)));
}
template <typename T> inline T  asin_impl(const T v, float128_real_type_tag)
{
	return asin(v);
}
template <typename T> inline T asinh_impl(const T v, float128_real_type_tag)
{
	return log(v + sqrt((v * v) + _Float128(1)));
}
template <typename T> inline T  atan_impl(const T v, float128_real_type_tag)
{
	return atan(v);
}
template <typename T> inline T atanh_impl(const T v, float128_real_type_tag)
{
	return (log(_Float128(1) + v) - log(_Float128(1) - v)) / _Float128(2);
}
template <typename T> inline T  ceil_impl(const T v, float128_real_type_tag)
{
	return ceil(v);
}
template <typename T> inline T   cos_impl(const T v, float128_real_type_tag)
{
	return cos(v);
}
template <typename T> inline T  cosh_impl(const T v, float128_real_type_tag)
{
	return cosh(v);
}
template <typename T> inline T   exp_impl(const T v, float128_real_type_tag)
{
	return exp(v);
}
template <typename T> inline T floor_impl(const T v, float128_real_type_tag)
{
	return floor(v);
}
template <typename T> inline T   log_impl(const T v, float128_real_type_tag)
{
	return log(v);
}
template <typename T> inline T log10_impl(const T v, float128_real_type_tag)
{
	return log10(v);
}
template <typename T> inline T  log2_impl(const T v, float128_real_type_tag)
{
	return log(v) / _Float128(exprtk::details::numeric::constant::log2);
}
template <typename T> inline T   neg_impl(const T v, float128_real_type_tag)
{
	return -v;
}
template <typename T> inline T   pos_impl(const T v, float128_real_type_tag)
{
	return  v;
}
template <typename T> inline T   sin_impl(const T v, float128_real_type_tag)
{
	return sin(v);
}
template <typename T> inline T  sinh_impl(const T v, float128_real_type_tag)
{
	return sinh(v);
}
template <typename T> inline T  sqrt_impl(const T v, float128_real_type_tag)
{
	return sqrt(v);
}
template <typename T> inline T   tan_impl(const T v, float128_real_type_tag)
{
	return tan(v);
}
template <typename T> inline T  tanh_impl(const T v, float128_real_type_tag)
{
	return tanh(v);
}
template <typename T> inline T   cot_impl(const T v, float128_real_type_tag)
{
	return _Float128(1) / tan(v);
}
template <typename T> inline T   sec_impl(const T v, float128_real_type_tag)
{
	return _Float128(1) / cos(v);
}
template <typename T> inline T   csc_impl(const T v, float128_real_type_tag)
{
	return _Float128(1) / sin(v);
}
template <typename T> inline T   r2d_impl(const T v, float128_real_type_tag)
{
	return (v * _Float128(exprtk::details::numeric::constant::_180_pi));
}
template <typename T> inline T   d2r_impl(const T v, float128_real_type_tag)
{
	return (v * _Float128(exprtk::details::numeric::constant::pi_180));
}
template <typename T> inline T   d2g_impl(const T v, float128_real_type_tag)
{
	return (v * _Float128(20.0 / 9.0));
}
template <typename T> inline T   g2d_impl(const T v, float128_real_type_tag)
{
	return (v * _Float128(9.0 / 20.0));
}
template <typename T> inline T  notl_impl(const T v, float128_real_type_tag)
{
	return (v != _Float128(0) ? _Float128(0) : _Float128(1));
}
template <typename T> inline T  frac_impl(const T v, float128_real_type_tag)
{
	return (v - static_cast<long long>(v));
}
template <typename T> inline T trunc_impl(const T v, float128_real_type_tag)
{
	return _Float128(static_cast<long long>(v));
}

template <typename T>
inline int to_int32_impl(const T v, float128_real_type_tag)
{
	return static_cast<int>(v);
}

template <typename T>
inline long long to_int64_impl(const T v, float128_real_type_tag)
{
	return static_cast<long long int>(v);
}

inline bool is_true_impl(const _Float128 v)
{
	return (0.0 != (double)v);
}

inline bool is_false_impl(const _Float128 v)
{
	return (0.0 == (double)v);
}

template <typename T>
inline T expm1_impl(const T v, float128_real_type_tag)
{
	if (abs(v) < T(0.00001)) {
		return v + (T(0.5) * v * v);
	} else {
		return exp(v) - T(1);
	}
}

template <typename T>
inline T nequal_impl(const T v0, const T v1, float128_real_type_tag)
{
	const T epsilon  = epsilon_type<T>::value();
	const T eps_norm = (std::max(T(1), std::max(abs_impl(v0,
	                             float128_real_type_tag()),
	                             abs_impl(v1, float128_real_type_tag()))) * epsilon);
	return (abs_impl(v0 - v1, float128_real_type_tag()) > eps_norm) ? T(1) : T(0);
}

template <typename T>
inline T sgn_impl(const T v, float128_real_type_tag)
{
	if (v > T(0)) {
		return T(+1);
	} else if (v < T(0)) {
		return T(-1);
	} else {
		return T(0);
	}
}

template <typename T>
inline T log1p_impl(const T v, float128_real_type_tag)
{
	if (v > T(-1)) {
		if (abs_impl(v, float128_real_type_tag()) > T(0.0001)) {
			return log_impl(T(1) + v, float128_real_type_tag());
		} else {
			return (T(-0.5) * v + T(1)) * v;
		}
	} else {
		return T(std::numeric_limits<T>::quiet_NaN());
	}
}

template <typename T>
inline T erf_impl(T v, float128_real_type_tag)
{
	const T t = T(1) / (T(1) + T(0.5) * abs_impl(v, float128_real_type_tag()));

	static const T c[] = {
	        T(1.26551223), T(1.00002368),
	        T(0.37409196), T(0.09678418),
	        T(-0.18628806), T(0.27886807),
	        T(-1.13520398), T(1.48851587),
	        T(-0.82215223), T(0.17087277)
	};

	T result = T(1) - t * exp_impl((-v * v) -
	                               c[0] + t * (c[1] + t *
	                                               (c[2] + t * (c[3] + t *
	                                                               (c[4] + t * (c[5] + t *
	                                                                               (c[6] + t * (c[7] + t *
	                                                                                               (c[8] + t * (c[9]))))))))), float128_real_type_tag());

	return (v >= T(0)) ? result : -result;
}

template <typename T>
inline T erfc_impl(T v, float128_real_type_tag)
{
	return T(1) - erf_impl(v, float128_real_type_tag());
}

template <typename T>
inline T ncdf_impl(T v, float128_real_type_tag)
{
	T cnd = T(0.5) * (T(1) + erf_impl(
	                          abs_impl(v, float128_real_type_tag()) /
	                          T(exprtk::details::numeric::constant::sqrt2), float128_real_type_tag()));
	return (v < T(0)) ? (T(1) - cnd) : cnd;
}

template <typename T>
inline T modulus_impl(const T v0, const T v1, float128_real_type_tag)
{
	return modulus(v0, v1);
}

template <typename T>
inline T pow_impl(const T v0, const T v1, float128_real_type_tag)
{
	return pow(v0, v1);
}

template <typename T>
inline T logn_impl(const T v0, const T v1, float128_real_type_tag)
{
	return log(v0) / log(v1);
}

template <typename T>
inline T sinc_impl(T v, float128_real_type_tag)
{
	if (abs_impl(v, float128_real_type_tag()) >=
	    std::numeric_limits<T>::epsilon()) {
		return (sin_impl(v, float128_real_type_tag()) / v);
	} else {
		return T(1);
	}
}

template <typename T>
inline T xor_impl(const T v0, const T v1, float128_real_type_tag)
{
	return (is_false_impl(v0) != is_false_impl(v1)) ? T(1) : T(0);
}

template <typename T>
inline T xnor_impl(const T v0, const T v1, float128_real_type_tag)
{
	const bool v0_true = is_true_impl(v0);
	const bool v1_true = is_true_impl(v1);
	if ((v0_true &&  v1_true) || (!v0_true && !v1_true)) {
		return T(1);
	} else {
		return T(0);
	}
}

template <typename T>
inline T equal_impl(const T v0, const T v1, float128_real_type_tag)
{
	const T epsilon = epsilon_type<T>::value();
	const T eps_norm = (max(T(1), max(abs_impl((double)v0,
	                                  float128_real_type_tag()),
	                                  abs_impl((double)v1,
	                                                  float128_real_type_tag()))) * epsilon);
	return (abs_impl(v0 - v1, float128_real_type_tag()) <= eps_norm) ? T(1) : T(0);
}

template <typename T>
inline T round_impl(const T v, float128_real_type_tag)
{
	return ((v < T(0)) ? ceil(v - T(0.5)) : floor(v + T(0.5)));
}

template <typename T>
inline T roundn_impl(const T v0, const T v1, float128_real_type_tag)
{
	const int index = std::max<int>(0, std::min<int>(pow10_size - 1,
	                                (int)floor(v1)));
	const T p10 = T(pow10[index]);
	if (v0 < T(0)) {
		return T(ceil((v0 * p10) - T(0.5)) / p10);
	} else {
		return T(floor((v0 * p10) + T(0.5)) / p10);
	}
}

template <typename T>
inline bool is_integer_impl(const T v, float128_real_type_tag)
{
	return (T(0) == modulus_impl(v, T(1), float128_real_type_tag()));
}

template <typename T>
inline T root_impl(const T v0, const T v1, float128_real_type_tag)
{
	return pow(v0, T(1) / v1);
}

template <typename T>
inline T hypot_impl(const T v0, const T v1, float128_real_type_tag)
{
	return sqrt((v0 * v0) + (v1 * v1));
}

template <typename T>
inline T atan2_impl(const T v0, const T v1, float128_real_type_tag)
{
	return std::atan2((double)v0, (double)v1);
}

template <typename T>
inline T shr_impl(const T v0, const T v1, float128_real_type_tag)
{
	return v0 * (T(1) / pow(T(2), v1));
}

template <typename T>
inline T shl_impl(const T v0, const T v1, float128_real_type_tag)
{
	return v0 * pow(T(2), v1);
}

template <typename T>
inline T and_impl(const T v0, const T v1, float128_real_type_tag)
{
	return (is_true_impl(v0) && is_true_impl(v1)) ? T(1) : T(0);
}

template <typename T>
inline T nand_impl(const T v0, const T v1, float128_real_type_tag)
{
	return (is_false_impl(v0) || is_false_impl(v1)) ? T(1) : T(0);
}

template <typename T>
inline T or_impl(const T v0, const T v1, float128_real_type_tag)
{
	return (is_true_impl(v0) || is_true_impl(v1)) ? T(1) : T(0);
}

template <typename T>
inline T nor_impl(const T v0, const T v1, float128_real_type_tag)
{
	return (is_false_impl(v0) && is_false_impl(v1)) ? T(1) : T(0);
}
}
}

template <typename Iterator>
inline bool string_to_real(Iterator &itr_external, const Iterator end,
                           _Float128 &t, details::numeric::details::float128_real_type_tag)
{
	typename numeric::details::number_type<double>::type num_type;
	return string_to_real<Iterator, double>(itr_external, end, (double)t, num_type);
}

inline bool is_true(const _Float128 &v)
{
	return is_true((double)v);
}
inline bool is_false(const _Float128 v)
{
	return is_false((double)v);
}
}

namespace helper
{
namespace details
{
inline void print_type(const std::string &fmt, const _Float128 v,
                       exprtk::details::numeric::details::float128_real_type_tag)
{
	printf(fmt.c_str(), (double)v);
}
}
}
}

#endif // EXPRTK_FLOAT128_ADAPTOR_HPP
