#ifndef BIG_INT_HPP
#define BIG_INT_HPP

#include "TypeName.hpp"
#include <string>
#include <vector>

namespace ds
{
	class big_uint_t
	{
	public:
		enum
		{
			DIGIT_LEN = 9,
			BASE = static_cast<uint32_t>(1e9)
		};

		big_uint_t() : last_len(), num() {}
		big_uint_t(uint64_t x) : last_len(), num()
		{
			while (x)
			{
				this->num.push_back(x % BASE);
				x /= BASE;
			}
			this->calc_last_len();
		}
		big_uint_t(const string_t &s) : last_len(), num()
		{
			uint64_t x = 0;
			size_t i = s.length(), j;
			while (i >= DIGIT_LEN)
			{
				for (j = i - DIGIT_LEN; j ^ i; ++j)
				{
					x = (x << 1) + (x << 3) + (s[j] ^ '0');
				}
				this->num.push_back(x);
				x = 0;
				i -= DIGIT_LEN;
			}
			for (j = 0; j ^ i; ++j)
			{
				x = (x << 1) + (x << 3) + (s[j] ^ '0');
			}
			if (x)
			{
				this->num.push_back(x);
			}
			while (this->num.size() && !this->num.back())
			{
				this->num.pop_back();
			}
			this->calc_last_len();
		}

		inline static void swap(big_uint_t &a, big_uint_t &b)
		{
			a.num.swap(b.num);
			if (a.last_len ^ b.last_len)
			{
				a.last_len ^= b.last_len;
				b.last_len ^= a.last_len;
				a.last_len ^= b.last_len;
			}
		}
		inline void swap(big_uint_t &x)
		{
			swap(*this, x);
		}

		inline size_t last_elem_length() const { return this->last_len; }
		inline size_t count_elems() const { return this->num.size(); }
		inline size_t count_digits() const { return (this->num.size() + this->last_len); }

		inline dynamic_array_t<uint32_t> &data() { return this->num; }
		inline const dynamic_array_t<uint32_t> &data() const { return this->num; }

		inline uint32_t &at(size_t index) { return this->num.at(index); }
		inline const uint32_t &at(size_t index) const { return this->num.at(index); }
		inline uint32_t &operator[](size_t index) { return this->num[index]; }
		inline const uint32_t &operator[](size_t index) const { return this->num[index]; }

		inline bool to_bool() const { return this->last_len; }
		inline operator bool() const { return this->last_len; }
		inline bool empty() const { return (!this->last_len); }
		inline bool nil() const { return (!this->last_len); }
		inline bool operator!() const { return (!this->last_len); }
		inline uint64_t to_uint64() const
		{
			uint64_t res = 0;
			for (size_t i = this->num.size() - 1; ~i; --i)
			{
				res = res * BASE + this->num[i];
			}
			return res;
		}
		inline operator uint64_t() const { return this->to_uint64(); }
		inline string_t to_string() const
		{
			if (!this->last_len)
			{
				return "0";
			}
			string_t res = std::to_string(this->num.back()), sub;
			for (size_t i = this->num.size() - 2; ~i; --i)
			{
				sub = std::to_string(this->num[i]);
				if (sub.length() ^ DIGIT_LEN)
				{
					res += string_t(DIGIT_LEN - sub.length(), '0');
				}
				res += std::to_string(this->num[i]);
			}
			return res;
		}
		inline operator string_t() const { return this->to_string(); }

		inline static bool equal(const big_uint_t &a, const big_uint_t &b)
		{
			size_t as = a.num.size();
			if (a.last_len ^ b.last_len || as ^ b.num.size())
			{
				return false;
			}
			for (size_t i = 0; i ^ as; ++i)
			{
				if (a.num[i] ^ b.num[i])
				{
					return false;
				}
			}
			return true;
		}
		inline friend bool operator==(const big_uint_t &lhs, const big_uint_t &rhs)
		{
			return equal(lhs, rhs);
		}
		inline static bool not_equal(const big_uint_t &a, const big_uint_t &b)
		{
			if (a.last_len ^ b.last_len || a.num.size() ^ b.num.size())
			{
				return true;
			}
			for (size_t i = 0, size = a.num.size(); i ^ size; ++i)
			{
				if (a.num[i] ^ b.num[i])
				{
					return true;
				}
			}
			return false;
		}
		inline friend bool operator!=(const big_uint_t &lhs, const big_uint_t &rhs)
		{
			return not_equal(lhs, rhs);
		}
		inline static bool greater(const big_uint_t &a, const big_uint_t &b)
		{
			size_t as = a.num.size(), bs = b.num.size();
			if (as ^ bs)
			{
				return (as > bs);
			}
			if (a.last_len ^ b.last_len)
			{
				return (a.last_len > b.last_len);
			}
			for (size_t i = as - 1; ~i; --i)
			{
				if (a.num[i] ^ b.num[i])
				{
					return (a.num[i] > b.num[i]);
				}
			}
			return false;
		}
		inline friend bool operator>(const big_uint_t &lhs, const big_uint_t &rhs)
		{
			return greater(lhs, rhs);
		}
		inline static bool greater_equal(const big_uint_t &a, const big_uint_t &b)
		{
			size_t as = a.num.size(), bs = b.num.size();
			if (as ^ bs)
			{
				return (as > bs);
			}
			if (a.last_len ^ b.last_len)
			{
				return (a.last_len > b.last_len);
			}
			for (size_t i = as - 1; ~i; --i)
			{
				if (a.num[i] ^ b.num[i])
				{
					return (a.num[i] > b.num[i]);
				}
			}
			return true;
		}
		inline friend bool operator>=(const big_uint_t &lhs, const big_uint_t &rhs)
		{
			return greater_equal(lhs, rhs);
		}
		inline static bool less(const big_uint_t &a, const big_uint_t &b)
		{
			size_t as = a.num.size(), bs = b.num.size();
			if (as ^ bs)
			{
				return (as < bs);
			}
			if (a.last_len ^ b.last_len)
			{
				return (a.last_len < b.last_len);
			}
			for (size_t i = as - 1; ~i; --i)
			{
				if (a.num[i] ^ b.num[i])
				{
					return (a.num[i] < b.num[i]);
				}
			}
			return false;
		}
		inline friend bool operator<(const big_uint_t &lhs, const big_uint_t &rhs)
		{
			return less(lhs, rhs);
		}
		inline static bool less_equal(const big_uint_t &a, const big_uint_t &b)
		{
			size_t as = a.num.size(), bs = b.num.size();
			if (as ^ bs)
			{
				return (as < bs);
			}
			if (a.last_len ^ b.last_len)
			{
				return (a.last_len < b.last_len);
			}
			for (size_t i = as - 1; ~i; --i)
			{
				if (a.num[i] ^ b.num[i])
				{
					return (a.num[i] < b.num[i]);
				}
			}
			return true;
		}
		inline friend bool operator<=(const big_uint_t &lhs, const big_uint_t &rhs)
		{
			return less_equal(lhs, rhs);
		}

		inline static big_uint_t plus(const big_uint_t &a, const big_uint_t &b)
		{
			uint64_t carry = 0;
			big_uint_t res;
			size_t i = 0, as = a.num.size(), bs = b.num.size();
			for (size_t mins = ((as < bs) ? as : bs); i ^ mins; ++i)
			{
				res.num.push_back((carry += a.num[i] + b.num[i]) %
								  BASE);
				carry /= BASE;
			}
			while (i < as)
			{
				res.num.push_back((carry += a.num[i]) % BASE);
				carry /= BASE;
				++i;
			}
			while (i < bs)
			{
				res.num.push_back((carry += b.num[i]) % BASE);
				carry /= BASE;
				++i;
			}
			while (carry)
			{
				res.num.push_back(carry % BASE);
				carry /= BASE;
			}
			res.calc_last_len();
			return res;
		}
		inline friend big_uint_t operator+(const big_uint_t &lhs, const big_uint_t &rhs)
		{
			return plus(lhs, rhs);
		}
		inline big_uint_t &plus_equal(const big_uint_t &x)
		{
			uint64_t carry = 0;
			size_t i = 0, s = this->num.size(), xs = x.num.size();
			for (size_t mins = ((s < xs) ? s : xs); i ^ mins; ++i)
			{
				this->num[i] = (carry += this->num[i] + x.num[i]) % BASE;
				carry /= BASE;
			}
			while (i < s)
			{
				this->num[i] = (carry += this->num[i]) % BASE;
				carry /= BASE;
				++i;
			}
			while (i < xs)
			{
				this->num.push_back((carry += x.num[i]) % BASE);
				carry /= BASE;
				++i;
			}
			while (carry)
			{
				this->num.push_back(carry % BASE);
				carry /= BASE;
			}
			this->calc_last_len();
			return *this;
		}
		inline static big_uint_t &plus_equal(big_uint_t &a, const big_uint_t &b)
		{
			return a.plus_equal(b);
		}
		inline friend big_uint_t &operator+=(big_uint_t &lhs, const big_uint_t &rhs)
		{
			return plus_equal(lhs, rhs);
		}

		inline static big_uint_t _minus(const big_uint_t &a, const big_uint_t &b)
		{
			size_t i = 0;
			bool carry = false;
			big_uint_t res;
			while (i ^ b.num.size())
			{
				if (a.num[i] < b.num[i] + carry)
				{
					res.num.push_back(BASE - carry - b.num[i] + a.num[i]);
					carry = true;
				}
				else
				{
					res.num.push_back(a.num[i] - b.num[i] - carry);
					carry = false;
				}
				++i;
			}
			while (carry)
			{
				if (a.num[i] < carry)
				{
					res.num.push_back(BASE - 1);
				}
				else
				{
					res.num.push_back(a.num[i] - carry);
					carry = false;
				}
				++i;
			}
			while (i ^ a.num.size())
			{
				res.num.push_back(a.num[i]);
				++i;
			}
			return res;
		}
		inline static big_uint_t minus(const big_uint_t &a, const big_uint_t &b)
		{
			return ((a < b) ? _minus(b, a) : minus(a, b));
		}
		inline friend big_uint_t operator-(const big_uint_t &lhs, const big_uint_t &rhs)
		{
			return minus(lhs, rhs);
		}
		inline big_uint_t &_minus_equal(const big_uint_t &x)
		{
			size_t i = 0;
			bool carry = false;
			big_uint_t res;
			while (i ^ x.num.size())
			{
				if (this->num[i] < x.num[i] + carry)
				{
					this->num[i] = BASE - carry - x.num[i] + this->num[i];
					carry = true;
				}
				else
				{
					this->num[i] = this->num[i] - x.num[i] - carry;
					carry = false;
				}
				++i;
			}
			while (true)
			{
				if (this->num[i] < 1)
				{
					this->num[i] = BASE - 1;
				}
				else
				{
					--this->num[i];
					break;
				}
			}
			return *this;
		}
		inline big_uint_t &minus_equal(const big_uint_t &x)
		{
			return ((*this < x) ? (*this = _minus(x, *this)) : this->_minus_equal(x));
		}
		inline static big_uint_t &minus_equal(big_uint_t &a, const big_uint_t &b)
		{
			return a.minus_equal(b);
		}
		inline friend big_uint_t &operator-=(big_uint_t &lhs, const big_uint_t &rhs)
		{
			return minus_equal(lhs, rhs);
		}

		inline void calc_last_len()
		{
			if (this->num.size())
			{
				uint32_t x = this->num.back();
				this->last_len = 0;
				while (x)
				{
					++this->last_len;
					x /= 10;
				}
			}
		}

		inline static big_uint_t gcd(big_uint_t a, big_uint_t b)
		{
			while (a && b && a != b)
			{
				if (a > b)
				{
					a._minus_equal(b);
				}
				else
				{
					b._minus_equal(a);
				}
			}
			return (a ?: b);
		} // unfinished

	protected:
		size_t last_len;
		dynamic_array_t<uint32_t> num;

	private:
	};
}; // unfinished

#endif