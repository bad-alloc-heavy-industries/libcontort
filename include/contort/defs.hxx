//SPDX-License-Identifier: BSD-3-Clause
#ifndef CONTORT_DEFS__HXX
#define CONTORT_DEFS__HXX

#ifndef _WINDOWS
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#	define CONTORT_DEFAULT_VISIBILITY __attribute__((visibility("default")))
#else
#	ifdef BUILD_CONTORT
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#		define CONTORT_DEFAULT_VISIBILITY __declspec(dllexport)
#		pragma warning(disable: 4996)
#	else
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#		define CONTORT_DEFAULT_VISIBILITY __declspec(dlliport)
#	endif
#endif

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CONTORT_API extern CONTORT_DEFAULT_VISIBILITY
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CONTORT_CLS_API CONTORT_DEFAULT_VISIBILITY

#endif /*CONTORT_DEFS__HXX*/
