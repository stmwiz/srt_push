//
// Created by x on 2021/8/18.
//

#pragma once

#include <string>
#include <cstring>

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#else

#include <stdarg.h>
#include <sys/stat.h>
#include <sys/statfs.h>

#endif

#ifdef _WIN32
#define PATH_SEPARATOR   '\\'

#define ACCESS _access
#define MKDIR(a) _mkdir((a))
#else
#define PATH_SEPARATOR   '/'

#define ACCESS access
#define MKDIR(a) mkdir((a),0755)

#endif

namespace xlab::base::FileSystem {

static inline uint64_t GetSystemTFTotalSize(const char *path) {
    if (path == nullptr) {
        return -1;
    }

    struct statfs diskInfo{};
    statfs(path, &diskInfo);
    const uint64_t &totalBlocks = diskInfo.f_bsize;
    const uint64_t &totalDisk = diskInfo.f_blocks * totalBlocks;

    return totalBlocks;
}

static inline uint64_t GetSystemTFFree(const char *path) {
    if (path == nullptr) {
        return -1;
    }

    struct statfs diskInfo{};
    statfs(path, &diskInfo);
    const uint64_t &totalBlocks = diskInfo.f_bsize;
    const uint64_t &freeDisk = diskInfo.f_bfree * totalBlocks;

    return freeDisk;
}

static inline bool SystemTFIsFull(const char *path, uint64_t bsize) {
    return GetSystemTFFree(path) <= bsize;;
}

static inline bool AndroidSystemTFIsFull(uint64_t bsize) {
#ifdef ANDROID
    return SystemTFIsFull("/sdcard", bsize);
#endif

    return false;
}

static int tolower(int c) {
    if (c >= 'A' && c <= 'Z')
        c ^= 0x20;
    return c;
}

static int strncasecmp(const char *a, const char *b, size_t n) {
    uint8_t c1, c2;
    if (n <= 0)
        return 0;
    do {
        c1 = tolower(*a++);
        c2 = tolower(*b++);
    } while (--n && c1 && c1 == c2);
    return c1 - c2;
}

static inline int mkdir_p(const char *path) {
    int ret = 0;
    char *temp = ::strdup(path);
    char *pos = temp;
    char tmp_ch = '\0';

    if (!path || !temp) {
        return -1;
    }

    if (!strncasecmp(temp, "/", 1) || !strncasecmp(temp, "\\", 1)) {
        pos++;
    } else if (!strncasecmp(temp, "./", 2) || !strncasecmp(temp, ".\\", 2)) {
        pos += 2;
    }

    for (; *pos != '\0'; ++pos) {
        if (*pos == '/' || *pos == '\\') {
            tmp_ch = *pos;
            *pos = '\0';
            ret = MKDIR(temp);
            *pos = tmp_ch;
        }
    }

    if ((*(pos - 1) != '/') || (*(pos - 1) != '\\')) {
        ret = MKDIR(temp);
    }

    free(temp);
    return ret;
}

}

