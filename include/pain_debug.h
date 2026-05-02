#ifndef PAIN_DEBUG_H
#define PAIN_DEBUG_H

#include <iostream>

#if defined(_MSC_VER)
# define PAIN_ALWAYS_INLINE_ATTR  __forceinline
# define PAIN_NOINLINE_ATTR       __declspec(noinline)
# define PAIN_NORETURN_ATTR       __declspec(noreturn)
# define PAIN_COLD_ATTR           __attribute__((__cold__))

#elif defined(__GNUC__) || defined(__clang__)
# define PAIN_ALWAYS_INLINE_ATTR  __attribute__((__always_inline)) inline
# define PAIN_NOINLINE_ATTR       __attribute__((__noinline__))
# define PAIN_NORETURN_ATTR       __attribute__((__noreturn__))

# define PAIN_CONST_ATTR          __attribute__((__const__))
# define PAIN_COLD_ATTR           __attribute__((__cold__))
# define PAIN_MALLOC_ATTR         __attribute__((__malloc__))

# define nothing
#endif

#define __PAIN_LINE __LINE__
#define __PAIN_FILE __FILE__

#define ensure(result, msg) \
  if ((result) != VK_SUCCESS) \
    throw std::runtime_error((msg));

inline void PAIN_NORETURN_ATTR PAIN_COLD_ATTR
_pain_assert_fail(const char* _msg, const char* _file, int _line) {
  std::cerr << "PAIN::ASSERTION::ERROR:: " << _msg << " at " << _file << ":" << _line;
  std::abort();
};

#define pain_assert(expr, msg) \
  if(!(expr)) { \
    _pain_assert_fail(msg, __PAIN_FILE, __PAIN_LINE); \
  }

#endif // PAIN_DEBUG_H