#pragma once

#include <vector>
#include <string>

#include <algorithm>

#include <iostream>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using s8 = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;

// работает как std::memcmp, только сравнивает с конца
// сравнивает по 8 байт
s64 memcmp_reverse(const void* buf1, const void* buf2, size_t n) {

	// u8
	{
		size_t k = n % sizeof(u64);

		n -= k;

		auto a = reinterpret_cast<const u8*>(buf1) + k;
		auto b = reinterpret_cast<const u8*>(buf2) + k;

		while (k) {
			--k;
			--a;
			--b;

			if (*a != *b) {
				return static_cast<s64>(*a) - *b;
			}
		}
	}

	// u64
	{
		n /= sizeof(u64);

		auto a = reinterpret_cast<const u64*>(buf1) + n;
		auto b = reinterpret_cast<const u64*>(buf2) + n;

		while (n) {
			--n;
			--a;
			--b;

			if (*a != *b) {

				auto c = reinterpret_cast<const u32*>(a) + 1;
				auto d = reinterpret_cast<const u32*>(b) + 1;

				if (*c == *d) {
					--c;
					--d;
				}
				return static_cast<s64>(*c) - *d;
			}
		}
	}

	return 0;
}

/*
* ULONG 2.0
*
* var/half_var is type of unsigned integer
* var is twice as long as half_var
*/
template<typename var, typename half_var, size_t len>
class ulong {

	var word[len];

	static const size_t var_bits = sizeof(var) * 8;
	static const size_t half_var_bits = sizeof(half_var) * 8;

	static const size_t byte_cnt = sizeof(var) * len;
	static const size_t bit_cnt = byte_cnt * 8;

public:

	/*
	* STATIC ASSERTS FOR TEMPLATE
	*/

	static_assert(sizeof(half_var) * 2 == sizeof(var)); // verification of lengths of number types

	static_assert(len != 0); // verification for empty word

private:

	var& operator [](size_t i) {
		return word[i];
	}
	const var& operator [](size_t i) const {
		return word[i];
	}

	void clear() {
		std::memset(word, 0, byte_cnt);
	}

public:

	/*
	* CONSTRUCTORS
	*/

private:

	template<typename T>
	void copy(const T& value) {
		if constexpr (sizeof(T) < byte_cnt) {
			clear();

			*reinterpret_cast<T*>(word) = value;
		}
		else {
			std::memcpy(word, &value, byte_cnt);
		}
	}

public:

	ulong() {
		clear();
	}
	template<typename T>
	ulong(const T& value) {
		if constexpr (std::is_signed_v<T>) {
			if (value < 0) {
				clear();
				*this -= -value;
			}
			else {
				copy(value);
			}
		}
		else {
			copy(value);
		}
	}
	ulong(const std::vector<var>& data) {
		clear();
		std::memcpy(word, data.data(), sizeof(var) * std::min(len, data.size()));
	}

	/*
	* BOOL OPERATIONS
	*/

private:

	// сравнивает 2 числа
	// вернет 0, если они равны
	// < 0, если *this < Rhs
	// > 0, иначе
	auto word_cmp(const ulong& Rhs) const {
		return memcmp_reverse(word, Rhs.word, byte_cnt);
	}

public:

	bool operator == (const ulong& Rhs) const {
		return word_cmp(Rhs) == 0;
	}
	bool operator != (const ulong& Rhs) const {
		return !(*this == Rhs);
	}

	bool operator < (const ulong& Rhs) const {
		return word_cmp(Rhs) < 0;
	}
	bool operator > (const ulong& Rhs) const {
		return word_cmp(Rhs) > 0;
	}

	bool operator <= (const ulong& Rhs) const {
		return !(*this > Rhs);
	}
	bool operator >= (const ulong& Rhs) const {
		return !(*this < Rhs);
	}

	/*
	* ARITHMETIC OPERATIONS
	*/

	ulong& operator += (const ulong& add) {
		return *this = *this + add;
	}
	ulong operator + (const ulong& add) const {
		ulong res;

		bool overflow = false;
		for (size_t i = 0; i < len; ++i) {

			var x = add[i] + overflow;

			res[i] = word[i] + x;

			overflow = res[i] < word[i] || res[i] < x || 
				x < add[i] || x < overflow;
		}

		return res;
	}

	ulong& operator -= (const ulong& sub) {
		return *this = *this - sub;
	}
	ulong operator - (const ulong& sub) const {
		ulong res;

		bool overflow = false;
		for (size_t i = 0; i < len; ++i) {

			var x = sub[i] + overflow;

			res[i] = word[i] - x;

			overflow = word[i] < x || x < sub[i] || x < overflow;
		}

		return res;
	}

	// karatsuba multiplication
	// not used. very slow
/*private:

	using two_ulong = ulong<var, half_var, len * 2>;
	using half_ulong = ulong<var, half_var, len / 2>;

	half_ulong first_half() const {
		half_ulong res;
		std::memcpy(&res, word, byte_cnt / 2);
		return res;
	}
	half_ulong second_half() const {
		half_ulong res;
		std::memcpy(&res, word + len / 2, byte_cnt / 2);
		return res;
	}

	static ulong naive_mul(const ulong& x, const ulong& y) {
		ulong result;

		auto a_ptr = reinterpret_cast<const half_var*>(x.word);
		auto b_ptr = reinterpret_cast<const half_var*>(y.word);

		auto res_ptr = reinterpret_cast<half_var*>(result.word);

		for (size_t i = 0; i < 2 * len; ++i) {

			var c = 0;
			for (size_t j = 0; i + j < 2 * len; ++j) {

				var k = static_cast<var>(a_ptr[i]) * b_ptr[j] +
					static_cast<var>(res_ptr[i + j]) +
					c;

				c = k >> half_var_bits;

				res_ptr[i + j] = k - (c << half_var_bits);
			}
		}

		return result;
	}

	ulong word_shift(size_t n) const {
		ulong res;
		for (size_t i = 0; i + n < len; ++i) {
			res[i + n] = word[i];
		}
		return res;
	}

	static two_ulong karatsuba_mul(const ulong& x, const ulong& y) {
		if (len <= 8) {
			return two_ulong::naive_mul(x, y);
		}
		else {
			half_ulong Xl = x.first_half(), Xr = x.second_half(), Yl = y.first_half(), Yr = y.second_half();

			ulong sumX = ulong(Xl) + Xr, sumY = ulong(Yl) + Yr;

			two_ulong P1 = ulong<var, half_var, len / 2>::karatsuba_mul(Xl, Yl);
			two_ulong P2 = ulong<var, half_var, len / 2>::karatsuba_mul(Xr, Yr);
			two_ulong P3;

			if (sumX[len / 2] != 0 || sumY[len / 2] != 0) {
				P3 = karatsuba_mul(sumX, sumY);
			}
			else {
				P3 = half_ulong::karatsuba_mul(sumX, sumY);
			}

			return P1 + (P3 - P1 - P2).word_shift(len / 2) + P2.word_shift(len);
		}
	}

public:*/

	ulong& operator *= (const ulong& mult) {
		return *this = *this * mult;
	}
	ulong operator * (const ulong& mult) const {
		ulong result;

		auto a_ptr = reinterpret_cast<const half_var*>(word);
		auto b_ptr = reinterpret_cast<const half_var*>(mult.word);

		auto res_ptr = reinterpret_cast<half_var*>(result.word);

		for (size_t i = 0; i < 2 * len; ++i) {

			var overflow = 0;
			for (size_t j = 0; i + j < 2 * len; ++j) {

				var k = static_cast<var>(a_ptr[i]) * b_ptr[j] +
					static_cast<var>(res_ptr[i + j]) +
					overflow;

				overflow = k >> half_var_bits;

				res_ptr[i + j] = static_cast<half_var>(k - (overflow << half_var_bits));
			}
		}

		return result;
	}

private:
	// номер старшего единичного бита с нуля
	int leading_bit() const {
		int bit = bit_cnt; // сдвиг, не нарушающий целостность div

		int block = len - 1;
		for (; block >= 0 && word[block] == 0; --block) {}

		if (block != -1) {

			int i = var_bits * (block + 1) - 1;

			for (; i >= 0 && !(word[block] & (static_cast<var>(1) << i % var_bits)); --i) {}

			bit = i;
		}

		return bit;
	}

public:

	ulong& operator /= (const ulong& div) {
		return *this = *this / div;
	}
	ulong operator / (const ulong& div) const {
		ulong res;

		ulong num = *this;
		for (int bit = bit_cnt - div.leading_bit() - 1; bit >= 0; --bit) {
			auto cur = div << bit;
			if (cur <= num) {
				num -= cur;
				res[bit / var_bits] |= (static_cast<var>(1) << (bit % var_bits));
			}
		}

		return res;
	}

	ulong& operator %= (const ulong& div) {
		return *this = *this / div;
	}
	ulong operator % (const ulong& div) const {
		ulong num = *this;

		for (int bit = bit_cnt - div.leading_bit() - 1; bit >= 0; --bit) {
			auto cur = div << bit;
			if (cur <= num) {
				num -= cur;
			}
		}
		return num;
	}

	/*
	* BITS OPERATIONS
	*/

	ulong& operator >>= (size_t pos) {
		pos = std::min(pos, bit_cnt);

		const int Wordshift = pos / var_bits;
		if (Wordshift != 0) {
			for (int Wpos = 0; Wpos < len; ++Wpos) {
				word[Wpos] = Wordshift < len - Wpos ? word[Wpos + Wordshift] : 0;
			}
		}

		if ((pos %= var_bits) != 0) {
			for (int Wpos = 0; Wpos < len - 1; ++Wpos) {
				word[Wpos] = (word[Wpos] >> pos) | (word[Wpos + 1] << (var_bits - pos));
			}

			word[len - 1] >>= pos;
		}
		return *this;
	}
	ulong operator >> (size_t pos) const {
		return ulong(*this) >>= pos;
	}

	ulong& operator <<= (size_t pos) {
		pos = std::min(pos, bit_cnt);

		const int Wordshift = pos / var_bits;
		if (Wordshift != 0) {
			for (int Wpos = len - 1; 0 <= Wpos; --Wpos) {
				word[Wpos] = Wordshift <= Wpos ? word[Wpos - Wordshift] : 0;
			}
		}

		if ((pos %= var_bits) != 0) {
			for (int Wpos = len - 1; 0 < Wpos; --Wpos) {
				word[Wpos] = (word[Wpos] << pos) | (word[Wpos - 1] >> (var_bits - pos));
			}

			word[0] <<= pos;
		}
		return *this;
	}
	ulong operator << (size_t pos) const {
		return ulong(*this) <<= pos;
	}

	ulong& operator &= (const ulong& val) {
		for (size_t i = 0; i < len; ++i) {
			word[i] &= val[i];
		}
		return *this;
	}
	ulong operator & (const ulong& val) const {
		return ulong(*this) &= val;
	}

	ulong& operator |= (const ulong& val) {
		for (size_t i = 0; i < len; ++i) {
			word[i] |= val[i];
		}
		return *this;
	}
	ulong operator | (const ulong& val) const {
		return ulong(*this) |= val;
	}

	ulong& operator ^= (const ulong& val) {
		for (size_t i = 0; i < len; ++i) {
			word[i] ^= val[i];
		}
		return *this;
	}
	ulong operator ^ (const ulong& val) const {
		return ulong(*this) ^= val;
	}

	ulong operator ~ () const {
		ulong res;
		for (size_t i = 0; i < len; ++i) {
			res[i] = ~word[i];
		}
		return res;
	}

	/*
	* OTHERS FUNCTION
	*/

	std::vector<var> get_words() const {
		return std::vector<var>(word, word + len);
	}

	template<typename T>
	explicit operator T() const { 
		T result = T{};
		for (size_t i = 0, shift = 0; i < len; ++i, shift += var_bits) {
			result += static_cast<T>(word[i]) << shift;
		}
		return result;
	}

	ulong sqrt() const {
		ulong x = 1;
		bool decreased = false;
		while (true) {
			ulong nx = (x + *this / x) >> 1;
			if (x == nx || nx > x && decreased) {
				break;
			}
			decreased = nx < x;
			x = nx;
		}
		return x;
	}

	// вернет бинарное представление числа
	// от младших битов к старшим
	std::string to_string() const {
		std::string s;
		s.reserve(bit_cnt);
		for (size_t i = 0; i < len; ++i) {

			for (size_t bit = 0; bit < var_bits; bit++) {
				
				s += '0' + ((word[i] >> bit) & 1);
			}
		}
		return s;
	}

	/*
	* OTHERS OPERATORS
	*/

private:

	// fast inc
	void inc() {
		size_t i = -1;
		do {
			++i;

			++word[i];

		} while (i < len - 1 && word[i] == static_cast<var>(0));
	}

	// fast dec
	void dec() {
		size_t i = -1;
		do {
			++i;

			--word[i];

		} while (i < len - 1 && word[i] == static_cast<var>(-1));
	}

public:

	ulong& operator ++ () {
		inc();
		return *this;
	}
	ulong operator ++ (int) {
		ulong temp = *this;
		inc();
		return temp;
	}

	ulong& operator -- () {
		dec();
		return *this;
	}
	ulong operator -- (int) {
		ulong temp = *this;
		dec();
		return temp;
	}
};

template<typename T, typename var, typename half_var, size_t len>
void read_ulong(std::istream& input, ulong<var, half_var, len>& val) {
	static_assert(sizeof(T) <= sizeof(val));

	T x;
	input >> x;
	val = x;
}

template<typename var, typename half_var, size_t len>
std::istream& operator >> (std::istream& input, ulong<var, half_var, len>& val) {

	if constexpr (sizeof(u64) <= sizeof(val)) {
		read_ulong<u64>(input, val);
	}
	else if constexpr (sizeof(u32) <= sizeof(val)) {
		read_ulong<u32>(input, val);
	}
	else if constexpr (sizeof(u16) <= sizeof(val)) {
		read_ulong<u16>(input, val);
	}
	else if constexpr (sizeof(u8) <= sizeof(val)) {
		read_ulong<u8>(input, val);
	}

	return input;
}

template<typename var, typename half_var, size_t len>
std::ostream& operator << (std::ostream& output, const ulong<var, half_var, len>& val) {
	return output << static_cast<u64>(val);
}

using u128 = ulong<u64, u32, 2>;
using u256 = ulong<u64, u32, 4>;
using u512 = ulong<u64, u32, 8>;
using u1024 = ulong<u64, u32, 16>;

