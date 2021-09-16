//
// Created by x on 2021/9/6.
//

#pragma once

#include <stdbool.h>

namespace xlab ::base {

enum class XBool {
    X_UNKNOWN = -1,
    X_TRUE = 0,
    X_FALSE = 1,
};

static inline XBool toXBool(bool value) {
    return value ? XBool::X_TRUE : XBool::X_FALSE;
}

static inline XBool toXBool(int value) {
    if (value == -1) {
        return XBool::X_UNKNOWN;
    }

    return toXBool(!!value);
}

}
