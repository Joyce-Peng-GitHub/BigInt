#ifndef TYPENAME_HPP
#define TYPENAME_HPP

#include <cstdint>
#include <bits/c++config.h>

using std::size_t;
#define USING_STD_INT_T(CNT_BITS) \
    using std::int##CNT_BITS##_t;
#define USING_STD_UINT_T(CNT_BITS) \
    using std::uint##CNT_BITS##_t;
USING_STD_INT_T(8)
USING_STD_INT_T(16)
USING_STD_INT_T(32)
USING_STD_INT_T(64)
USING_STD_UINT_T(8)
USING_STD_UINT_T(16)
USING_STD_UINT_T(32)
USING_STD_UINT_T(64)

using char8_t = char;

using c_string_t = char *;
using c_string_reference_t = char *const;
using const_c_string_t = const char *const;
using const_c_string_reference_t = const char *const;

#define string_t std::string

#define initializer_list_t std::initializer_list
#define array_t std::array
#define bitset_t std::bitset
#define dynamic_array_t std::vector
#define deque_t std::deque
#define forward_list_t std::forward_list
#define list_t std::list
#define pair_t std::pair
#define set_t std::set
#define map_t std::map
#define multiset_t std::multiset
#define multimap_t std::multimap
#define unordered_set_t std::unordered_set
#define unordered_map_t std::unordered_map
#define unordered_multiset_t std::unordered_multiset
#define unordered_multimap_t std::unordered_multimap
#define stack_t std::stack
#define queue_t std::queue
#define heap_t std::priority_queue

#define file_t std::FILE
#define ofstream_t std::ofstream
#define ifstream_t std::ifstream
#define fstream_t std::fstream
#define ostream_t std::ostream
#define istream_t std::istream
#define iostream_t std::iostream

#endif