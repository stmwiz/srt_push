//
// Created by x on 2021/8/18.
//

#pragma once

#include <cstdio>

namespace xlab::base::SystemCmd {

/**运行命令
 *1. cmd    要执行的命令
 *2. 参数type可使用“r”代表读取，“w”代表写入。
 */

static inline int RunShell(const char *cmd, const char *type, char *resultBuffer, int len) {
    int ret;

    FILE *pp = popen(cmd, type);
    ret = fread(resultBuffer, 1, len, pp);
    pclose(pp);

    return ret;
}


}
