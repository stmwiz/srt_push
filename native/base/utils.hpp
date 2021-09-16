//
// Created by x on 2021/8/30.
//

#pragma once

#include <string>

#ifndef TO_STR
#define TO_STR(x) (#x)
#endif

#ifndef TO_CHAR
#define TO_CHAR(x) (static_cast<const char(&)[2]>(#x)[0])
#endif

#ifndef TO_FMT_STR
#define TO_FMT_STR(x)  (std::string(#x"="+std::to_string(x)))
#endif

#ifndef TO_PAIR
#define TO_PAIR(x) {x,#x}
#endif

#ifndef CMB
#define CMB(x, y)  x##y
#endif

#ifndef CLEAR
#define CLEAR(x) memset(&(x), 0, sizeof(x))
#endif

#ifndef COPY_ARRY
#define COPY_ARRY(x, y) memcpy(x, y, sizeof(y)/sizeof(y[0]))
#endif

#ifndef TO_BOOL
#define TO_BOOL(x) (!!x)
#endif

#ifndef FULLY_EQUAL
#define FULLY_EQUAL(x, y, x1, y1) ((x==x1 && y==y1) || (x==y1&&y==x1))
#endif

#ifndef IS_RANGE
#define IS_RANGE(val, min, max) ( (val) <= (min) || (val) >= (max) )
#endif

#ifndef LIMT_RANGE
#define LIMT_RANGE(val, min, max, defval) if( (val) <= (min) || (val) >= (max) )  (val) = (defval)
#endif

