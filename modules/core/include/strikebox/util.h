#pragma once

#include <stdint.h>
#include <string>
#include <cctype>

namespace strikebox {

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

#define ALIGN_DOWN_SIZE(length, size) \
    ((uint32_t)(length) & ~(size - 1))

#define ALIGN_UP_SIZE(length, size) \
    (ALIGN_DOWN_SIZE(((uint32_t)(length) + size - 1), size))


struct ci_wchar_traits : public ::std::char_traits<wchar_t> {
	static bool eq(wchar_t c1, wchar_t c2) { return ::std::toupper(c1) == ::std::toupper(c2); }
	static bool ne(wchar_t c1, wchar_t c2) { return ::std::toupper(c1) != ::std::toupper(c2); }
	static bool lt(wchar_t c1, wchar_t c2) { return ::std::toupper(c1) <  ::std::toupper(c2); }
	static int compare(const wchar_t* s1, const wchar_t* s2, size_t n) {
		while (n-- != 0) {
			if (::std::toupper(*s1) < ::std::toupper(*s2)) return -1;
			if (::std::toupper(*s1) > ::std::toupper(*s2)) return 1;
			++s1; ++s2;
		}
		return 0;
	}
	static const wchar_t* find(const wchar_t* s, int n, wchar_t a) {
		while (n-- > 0 && std::toupper(*s) != std::toupper(a)) {
			++s;
		}
		return s;
	}
};

typedef ::std::basic_string<wchar_t, ci_wchar_traits> ci_wstring;

inline bool ends_with(ci_wstring const &value, ci_wstring const &ending) {
	if (ending.size() > value.size()) return false;
	return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

}

