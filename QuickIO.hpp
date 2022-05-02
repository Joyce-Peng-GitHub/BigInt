#ifndef QUICKIO_HPP
#define QUICKIO_HPP

#include "TypeName.hpp"
#include <cstdio>
#include <string>
#include "BigInt.hpp"

namespace qio
{
	namespace fio
	{
		using file_stream_t = std::FILE *;
		using file_stream_reference_t = std::FILE *;
		using mode_t = uint32_t;
		using offset_t = int32_t;
		using origin_t = uint8_t;
	}

	namespace fio
	{
		static const mode_t APP = 0b1;	  // append
		static const mode_t BIN = 0b10;	  // binary
		static const mode_t IN = 0b100;	  // read
		static const mode_t OUT = 0b1000; // write

		static const origin_t SET = 0, CUR = 1, END = 2;

		inline file_stream_t open(const_c_string_t file,
								  mode_t m_mode = IN | OUT)
		{
			char8_t s_mode[] = {((m_mode & APP) ? 'a' : ((m_mode & IN) ? 'r' : 'w')),
								((m_mode & BIN) ? 'b' : 't'),
								((((m_mode & IN) && (m_mode & OUT)) ||
								  ((m_mode & IN) && (m_mode & OUT)))
									 ? '+'
									 : '\0'),
								'\0'};
			return (std::fopen(file, s_mode));
		}
		inline bool open(file_stream_t &file_stream, const_c_string_t file,
						 mode_t m_mode = IN | OUT)
		{
			return (file_stream = open(file, m_mode));
		}
		inline bool redirect(file_stream_reference_t file_stream,
							 const_c_string_t s_new_file,
							 mode_t m_mode = IN | OUT)
		{
			char8_t s_mode[] = {((m_mode & APP) ? 'a' : ((m_mode & IN) ? 'r' : 'w')),
								((m_mode & BIN) ? 'b' : 't'),
								((((m_mode & IN) && (m_mode & OUT)) ||
								  ((m_mode & IN) && (m_mode & OUT)))
									 ? '+'
									 : '\0'),
								'\0'};
			return std::freopen(s_new_file, s_mode, file_stream);
		}

		inline offset_t get_pos(file_stream_t file_stream)
		{
			return std::ftell(file_stream);
		}
		inline bool set_pos(file_stream_t file_stream, offset_t pos, origin_t origin = SEEK_SET)
		{
			return !std::fseek(file_stream, pos, origin);
		}
		inline bool refresh(file_stream_t file_stream)
		{
			return !std::fseek(file_stream, 0L, SEEK_CUR);
		}

		inline bool close(file_stream_t &file_stream)
		{
			bool ret = !std::fclose(file_stream);
			file_stream = nullptr;
			return ret;
		}
	}

	template <typename T = int>
	T read(fio::file_stream_t file = stdin);

	inline void skip_blank(fio::file_stream_t file = stdin)
	{
		char8_t ch = std::fgetc(file);
		while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')
		{
			ch = std::fgetc(file);
		}
		std::ungetc(ch, file);
	}
	template <typename int_t>
	inline int _read_int(int_t &x, fio::file_stream_t file = stdin)
	{
		using std::fgetc;
		x = 0;
		int ret = 0, nega = 0;
		char8_t ch = fgetc(file);
		while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')
		{
			ch = fgetc(file);
		}
		if (~ch)
		{
			if (ch == '-')
			{
				nega = true;
				ch = fgetc(file);
			}
			else if (ch == '+')
			{
				ch = fgetc(file);
			}
		}
		else
		{
			ret = EOF;
			goto _RET;
		}
		while ('0' <= ch && ch <= '9')
		{
			x = (x << 1) + (x << 3) + (ch ^ '0');
			ret = 1;
			ch = fgetc(file);
		}
		if (nega)
		{
			x = -x;
		}
	_RET:
		std::ungetc(ch, file);
		return ret;
	}
	template <typename uint_t>
	inline int _read_uint(uint_t &x, fio::file_stream_t file = stdin)
	{
		using std::fgetc;
		x = 0;
		int ret = 0;
		char8_t ch = fgetc(file);
		while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')
		{
			ch = fgetc(file);
		}
		if (~ch)
		{
			if (ch == '+')
			{
				ch = fgetc(file);
			}
			else if (ch < '0' || ch > '9')
			{
				goto _RET;
			}
		}
		else
		{
			ret = EOF;
			goto _RET;
		}
		while ('0' <= ch && ch <= '9')
		{
			x = (x << 1) + (x << 3) + (ch ^ '0');
			ret = 1;
			ch = fgetc(file);
		}
	_RET:
		std::ungetc(ch, file);
		return ret;
	}
#define DEFINE_TRIVIAL_READ_INT(INT)                         \
	inline int read(INT &x, fio::file_stream_t file = stdin) \
	{                                                        \
		return _read_int(x, file);                           \
	}                                                        \
	template <>                                              \
	inline INT read<INT>(fio::file_stream_t file)            \
	{                                                        \
		INT x;                                               \
		return (~_read_int(x, file) ? x : EOF);              \
	}
#define DEFINE_TRIVIAL_READ_UINT(UINT)                        \
	inline int read(UINT &x, fio::file_stream_t file = stdin) \
	{                                                         \
		return _read_uint(x, file);                           \
	}                                                         \
	template <>                                               \
	inline UINT read<UINT>(fio::file_stream_t file)           \
	{                                                         \
		UINT x;                                               \
		return (~_read_uint(x, file) ? x : EOF);              \
	}
	DEFINE_TRIVIAL_READ_INT(int8_t)
	DEFINE_TRIVIAL_READ_INT(int16_t)
	DEFINE_TRIVIAL_READ_INT(int32_t)
	DEFINE_TRIVIAL_READ_INT(int64_t)
	DEFINE_TRIVIAL_READ_UINT(uint8_t)
	DEFINE_TRIVIAL_READ_UINT(uint16_t)
	DEFINE_TRIVIAL_READ_UINT(uint32_t)
	DEFINE_TRIVIAL_READ_UINT(uint64_t)

	template <>
	inline char8_t read<char8_t>(fio::file_stream_t file)
	{
		return std::fgetc(file);
	}
	inline int read(char8_t &ch, fio::file_stream_t file = stdin)
	{
		return (~(ch = std::fgetc(file)) ? 1 : EOF);
	}

	inline int read(bool &b, fio::file_stream_t file = stdin)
	{
		b = false;
		int x, ret = read(x, file);
		if (x == 1)
		{
			b = true;
		}
		return ret;
	}
	template <>
	inline bool read<bool>(fio::file_stream_t file)
	{
		bool b;
		read(b, file);
		return b;
	}

	inline size_t read(c_string_reference_t s, fio::file_stream_t file = stdin)
	{
		using std::fgetc;
		if (s)
		{
			char8_t ch = fgetc(file), *p = s;
			if (~ch)
			{
				while (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
				{
					ch = fgetc(file);
				}
				if (ch == EOF)
				{
					return EOF;
				}
				while (~ch && ch ^ ' ' && ch ^ '\t' && ch ^ '\r' && ch ^ '\n')
				{
					*(p++) = ch;
					ch = fgetc(file);
				}
				std::ungetc(ch, file);
				*p = '\0';
				return (p - s);
			}
			else
			{
				return static_cast<size_t>(EOF);
			}
		}
		else
		{
			return 0;
		}
	}
	inline size_t read(c_string_reference_t s, size_t max_sz, fio::file_stream_t file = stdin)
	{
		using std::fgetc;
		if (s && max_sz)
		{
			char8_t ch = fgetc(file), *p = s;
			if (~ch)
			{
				while (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
				{
					ch = fgetc(file);
				}
				if (ch == EOF)
				{
					return EOF;
				}
				while ((--max_sz) && ~ch &&
					   ch ^ ' ' && ch ^ '\t' && ch ^ '\r' && ch ^ '\n')
				{
					*(p++) = ch;
					ch = fgetc(file);
				}
				std::ungetc(ch, file);
				*p = '\0';
				return (p - s);
			}
			else
			{
				return static_cast<size_t>(EOF);
			}
		}
		else
		{
			return 0;
		}
	}
	inline int read(string_t &s, fio::file_stream_t file = stdin)
	{
		using std::fgetc;
		s.clear();
		char8_t ch = fgetc(file);
		while (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
		{
			ch = fgetc(file);
		}
		if (ch == EOF)
		{
			return EOF;
		}
		while (~ch && ch ^ ' ' && ch ^ '\t' && ch ^ '\r' && ch ^ '\n')
		{
			s.push_back(ch);
			ch = fgetc(file);
		}
		std::ungetc(ch, file);
		return !s.empty();
	}
	inline int read(string_t &s, size_t max_len, fio::file_stream_t file = stdin)
	{
		using std::fgetc;
		s.clear();
		if (max_len)
		{
			char8_t ch = fgetc(file);
			while (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
			{
				ch = fgetc(file);
			}
			if (ch == EOF)
			{
				return EOF;
			}
			while ((max_len--) && ~ch &&
				   ch ^ ' ' && ch ^ '\t' && ch ^ '\r' && ch ^ '\n')
			{
				s.push_back(ch);
				ch = fgetc(file);
			}
			std::ungetc(ch, file);
			return !s.empty();
		}
		else
		{
			return 0;
		}
	}
	template <>
	inline string_t read<string_t>(fio::file_stream_t file)
	{
		string_t s;
		read(s, file);
		return s;
	}
	inline size_t read_line(c_string_reference_t s, fio::file_stream_t file = stdin)
	{
		using std::fgetc;
		if (s)
		{
			char8_t ch = fgetc(file), *p = s;
			if (~ch)
			{
				while (~ch && ch ^ '\r' && ch ^ '\n')
				{
					*(p++) = ch;
					ch = fgetc(file);
				}
				std::ungetc(ch, file);
				*p = '\0';
				return (p - s);
			}
			else
			{
				return static_cast<size_t>(EOF);
			}
		}
		else
		{
			return 0;
		}
	}
	inline size_t read_line(c_string_reference_t s, size_t max_sz, fio::file_stream_t file = stdin)
	{
		using std::fgetc;
		if (s && max_sz)
		{
			char8_t ch = fgetc(file), *p = s;
			if (~ch)
			{
				while ((--max_sz) && ~ch && ch ^ '\r' && ch ^ '\n')
				{
					*(p++) = ch;
					ch = fgetc(file);
				}
				std::ungetc(ch, file);
				*p = '\0';
				return (p - s);
			}
			else
			{
				return static_cast<size_t>(EOF);
			}
		}
		else
		{
			return 0;
		}
	}
	inline int read_line(string_t &s, fio::file_stream_t file = stdin)
	{
		using std::fgetc;
		s.clear();
		char8_t ch = fgetc(file);
		if (~ch)
		{
			while (~ch && ch ^ '\r' && ch ^ '\n')
			{
				s.push_back(ch);
				ch = fgetc(file);
			}
			return !s.empty();
		}
		else
		{
			return EOF;
		}
	}
	inline int read_line(string_t &s, size_t max_len, fio::file_stream_t file = stdin)
	{
		using std::fgetc;
		s.clear();
		if (max_len)
		{
			char8_t ch = fgetc(file);
			if (ch == EOF)
			{
				return EOF;
			}
			while ((max_len--) && ~ch &&
				   ch ^ ' ' && ch ^ '\t' && ch ^ '\r' && ch ^ '\n')
			{
				s.push_back(ch);
				ch = fgetc(file);
			}
			std::ungetc(ch, file);
			return !s.empty();
		}
		else
		{
			return 0;
		}
	}
	inline string_t read_line(fio::file_stream_t file = stdin)
	{
		string_t s;
		read_line(s, file);
		return s;
	}

	inline int write(char8_t ch, fio::file_stream_t file = stdout)
	{
		return (~std::fputc(ch, file) ? 1 : EOF);
	}

	template <typename int_t>
	int _write_int(int_t x, fio::file_stream_t file = stdout)
	{
		using std::fputc;
		if (x < 0)
		{
			if (~fputc('-', file))
			{
				x = -x;
			}
			else
			{
				return EOF;
			}
		}
		return ((x > 9)
					? ((~_write_int(x / 10, file) && ~fputc((x % 10) ^ 48, file))
						   ? 1
						   : EOF)
					: (~fputc(x ^ 48, file) ? 1 : EOF));
	}
	template <typename uint_t>
	int _write_uint(uint_t x, fio::file_stream_t file)
	{
		using std::fputc;
		return ((x > 9)
					? ((~_write_uint(x / 10, file) &&
						~fputc((x % 10) ^ 48, file))
						   ? 1
						   : EOF)
					: (~fputc(x ^ 48, file) ? 1 : EOF));
	}
	int _write_uint(const ds::big_uint_t &x, fio::file_stream_t file)
	{
		int res = 0;
		for (size_t i = x.count_elems() - 1; ~i; --i)
		{
			if ((res = _write_uint(x[i], file)) ^ 1)
			{
				return res;
			}
		}
		return (res ?: (x ? 0 : write('0', file)));
	}
	template <typename uint_t>
	int _write_bin(uint_t x, fio::file_stream_t file = stdout)
	{
		using std::fputc;
		return ((x > 1)
					? ((~_write_bin(x >> 1U, file) &&
						~fputc((x & 1) ^ 48, file))
						   ? 1
						   : EOF)
					: (~fputc(x ^ 48, file) ? 1 : EOF));
	}
	template <typename uint_t>
	int _write_oct(uint_t x, fio::file_stream_t file = stdout)
	{
		using std::fputc;
		return ((x > 7)
					? ((~_write_oct(x >> 3, file) && ~fputc((x & 7) ^ 48, file))
						   ? 1
						   : EOF)
					: (~fputc(x ^ 48, file) ? 1 : EOF));
	}
	template <typename uint_t>
	int _write_dec(uint_t x, fio::file_stream_t file = stdout)
	{
		using std::fputc;
		return ((x > 9)
					? ((~_write_dec(x / 10, file) &&
						~fputc((x % 10) ^ 48, file))
						   ? 1
						   : EOF)
					: (~fputc(x ^ 48, file) ? 1 : EOF));
	}
	template <typename uint_t>
	int _write_hex(uint_t x, fio::file_stream_t file = stdout)
	{
		using std::fputc;
		static const char8_t to_hex_digit[] =
			{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
			 'a', 'b', 'c', 'd', 'e', 'f'};
		return ((x > 15)
					? ((~_write_hex(x >> 4, file) &&
						~fputc(to_hex_digit[x & 15], file))
						   ? 1
						   : EOF)
					: (~fputc(to_hex_digit[x], file) ? 1 : EOF));
	}
#define DEFINE_TRIVIAL_WRITE_INT(INT)                         \
	inline int write(INT x, fio::file_stream_t file = stdout) \
	{                                                         \
		return _write_int(x, file);                           \
	}
#define DEFINE_TRIVIAL_WRITE_UINT(UINT)                        \
	inline int write(UINT x, fio::file_stream_t file = stdout) \
	{                                                          \
		return _write_uint(x, file);                           \
	}
	DEFINE_TRIVIAL_WRITE_INT(int8_t)
	DEFINE_TRIVIAL_WRITE_INT(int16_t)
	DEFINE_TRIVIAL_WRITE_INT(int32_t)
	DEFINE_TRIVIAL_WRITE_INT(int64_t)
	DEFINE_TRIVIAL_WRITE_UINT(uint8_t)
	DEFINE_TRIVIAL_WRITE_UINT(uint16_t)
	DEFINE_TRIVIAL_WRITE_UINT(uint32_t)
	DEFINE_TRIVIAL_WRITE_UINT(uint64_t)
	DEFINE_TRIVIAL_WRITE_UINT(ds::big_uint_t)

#define DEFINE_TRIVIAL_WRITE_BIN(UINT)                                                 \
	inline int write_bin(UINT x, bool prefix = true, fio::file_stream_t file = stdout) \
	{                                                                                  \
		int ret = (prefix ? write('0', file) : 1);                                     \
		if (ret ^ 1)                                                                   \
		{                                                                              \
			goto _RET;                                                                 \
		}                                                                              \
		if (prefix && (ret = write('b', file)) ^ 1)                                    \
		{                                                                              \
			goto _RET;                                                                 \
		}                                                                              \
		ret = _write_bin(x, file);                                                     \
	_RET:                                                                              \
		return ret;                                                                    \
	}
	DEFINE_TRIVIAL_WRITE_BIN(uint8_t)
	DEFINE_TRIVIAL_WRITE_BIN(uint16_t)
	DEFINE_TRIVIAL_WRITE_BIN(uint32_t)
	DEFINE_TRIVIAL_WRITE_BIN(uint64_t)
#define DEFINE_TRIVIAL_WRITE_OCT(UINT)                                                 \
	inline int write_oct(UINT x, bool prefix = true, fio::file_stream_t file = stdout) \
	{                                                                                  \
		int ret = (prefix ? write('0', file) : 1);                                     \
		if (ret ^ 1)                                                                   \
		{                                                                              \
			goto _RET;                                                                 \
		}                                                                              \
		ret = _write_oct(x, file);                                                     \
	_RET:                                                                              \
		return ret;                                                                    \
	}
	DEFINE_TRIVIAL_WRITE_OCT(uint8_t)
	DEFINE_TRIVIAL_WRITE_OCT(uint16_t)
	DEFINE_TRIVIAL_WRITE_OCT(uint32_t)
	DEFINE_TRIVIAL_WRITE_OCT(uint64_t)
#define DEFINE_TRIVIAL_WRITE_DEC(UINT)                             \
	inline int write_dec(UINT x, fio::file_stream_t file = stdout) \
	{                                                              \
		return _write_dec(x, file);                                \
	}
	DEFINE_TRIVIAL_WRITE_DEC(uint8_t)
	DEFINE_TRIVIAL_WRITE_DEC(uint16_t)
	DEFINE_TRIVIAL_WRITE_DEC(uint32_t)
	DEFINE_TRIVIAL_WRITE_DEC(uint64_t)
#define DEFINE_TRIVIAL_WRITE_HEX(UINT)                                                 \
	inline int write_hex(UINT x, bool prefix = true, fio::file_stream_t file = stdout) \
	{                                                                                  \
		int ret = (prefix ? write('0', file) : 1);                                     \
		if (ret ^ 1)                                                                   \
		{                                                                              \
			goto _RET;                                                                 \
		}                                                                              \
		if (prefix && (ret = write('x', file)) ^ 1)                                    \
		{                                                                              \
			goto _RET;                                                                 \
		}                                                                              \
		ret = _write_hex(x, file);                                                     \
	_RET:                                                                              \
		return ret;                                                                    \
	}
	DEFINE_TRIVIAL_WRITE_HEX(uint8_t)
	DEFINE_TRIVIAL_WRITE_HEX(uint16_t)
	DEFINE_TRIVIAL_WRITE_HEX(uint32_t)
	DEFINE_TRIVIAL_WRITE_HEX(uint64_t)
	inline int write(const void *p, fio::file_stream_t file = stdout)
	{
		return write_hex(uint64_t(p), file);
	}

	inline int write(bool b, fio::file_stream_t file = stdout)
	{
		return (~std::fputs((b ? "true" : "false"), file) ? 1 : EOF);
	}

	inline int write(const_c_string_reference_t s, fio::file_stream_t file = stdout)
	{
		return (s ? (~std::fputs(s, file) ? 1 : EOF) : 0);
	}
	inline int write(const string_t &s, fio::file_stream_t file = stdout)
	{
		for (const char8_t &ch : s)
		{
			if (fputc(ch, file) == EOF)
			{
				return EOF;
			}
		}
		return 1;
	}

#define DEFINE_TRIVAL_NO_CONST_REFERENCE_WRITE_LINE(TYPE)           \
	inline int write_line(TYPE t, fio::file_stream_t file = stdout) \
	{                                                               \
		return (write(t, file) | write('\n', file));                \
	}
	DEFINE_TRIVAL_NO_CONST_REFERENCE_WRITE_LINE(int8_t)
	DEFINE_TRIVAL_NO_CONST_REFERENCE_WRITE_LINE(int16_t)
	DEFINE_TRIVAL_NO_CONST_REFERENCE_WRITE_LINE(int32_t)
	DEFINE_TRIVAL_NO_CONST_REFERENCE_WRITE_LINE(int64_t)
	DEFINE_TRIVAL_NO_CONST_REFERENCE_WRITE_LINE(uint8_t)
	DEFINE_TRIVAL_NO_CONST_REFERENCE_WRITE_LINE(uint16_t)
	DEFINE_TRIVAL_NO_CONST_REFERENCE_WRITE_LINE(uint32_t)
	DEFINE_TRIVAL_NO_CONST_REFERENCE_WRITE_LINE(uint64_t)
	DEFINE_TRIVAL_NO_CONST_REFERENCE_WRITE_LINE(bool)
	DEFINE_TRIVAL_NO_CONST_REFERENCE_WRITE_LINE(const void *)

	inline int write_line(const_c_string_reference_t s, fio::file_stream_t file = stdout)
	{
		return (~write(s, file) ? (write('\n', file)) : EOF);
	}
#define DEFINE_TRIVAL_CONST_REFERENCE_WRITE_LINE(TYPE)                     \
	inline int write_line(const TYPE &t, fio::file_stream_t file = stdout) \
	{                                                                      \
		return (~write(t, file) ? (write('\n', file)) : EOF);              \
	}
	DEFINE_TRIVAL_CONST_REFERENCE_WRITE_LINE(string_t)
	DEFINE_TRIVAL_CONST_REFERENCE_WRITE_LINE(ds::big_uint_t)

	inline int blank(fio::file_stream_t file = stdout)
	{
		return write(' ', file);
	}
	inline int tab(fio::file_stream_t file = stdout)
	{
		return write('\t', file);
	}
	inline int endl(fio::file_stream_t file = stdout)
	{
		return write('\n', file);
	}
	inline int write(int (*p_func)(fio::file_stream_t), fio::file_stream_t file = stdout)
	{
		return p_func(file);
	}
}

#endif