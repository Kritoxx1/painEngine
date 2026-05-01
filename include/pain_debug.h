#ifndef PAIN_DEBUG_H
#define PAIN_DEBUG_H

#include <iostream>

#define ensure(result, msg) \
  if ((result) != VK_SUCCESS) \
    throw std::runtime_error((msg));

#define __PAIN_LINE __LINE__
#define __PAIN_FILE __FILE__

inline void __attribute__((__noreturn__, __cold__))
_pain_assert_fail(const char* _msg, const char* _file, int _line) {
  std::cerr << "PAIN::ASSERTION::ERROR:: " << _msg << " at " << _file << ":" << _line;
  std::abort();
};

#define pain_assert(expr, msg) \
  if(!(expr)) { \
    _pain_assert_fail(msg, __PAIN_FILE, __PAIN_LINE); \
  }

#endif // PAIN_DEBUG_H